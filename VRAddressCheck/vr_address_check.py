"""
Check every address CommonLibF4VR uses on Fallout 4 VR against the game, with no game running.

For each relocation in the headers (REL::ID, RelocationID, VariantID, Offset, VariantOffset), the VR address is
resolved the way REL does it at runtime (an ID through the VR address library CSV, an offset as is) and checked
against a decrypted Fallout4VR.exe:

- an ID the VR address library has no row for resolves to 0: calling it crashes;
- a function must be at a function start (an entry of .pdata, or a leaf function after int3 / ret padding);
- the VRHookSites call sites must still be `call rel32` to their callee, and VRSettings' Setting objects must
  carry the name they are declared for.

It also reads the `// VR: tested` / `// VR: checked` marker on each relocation's line and reports what the tags
cover. Usage:

    python vr_address_check.py --exe Fallout4VR-decrypted.exe --csv version-1-2-72-0.csv [--db fo4_database.csv]
        [--include ../CommonLibF4/include] [--out report.md]

The Steam copy of Fallout4VR.exe has its .text encrypted on disk: decrypt it first (Steamless). --db is the
fallout_vr_address_library's fo4_database.csv, which names functions in the report.
"""
import argparse
import bisect
import csv
import os
import re
import struct
import sys
from collections import Counter, defaultdict

IMAGE_BASE = 0x140000000
ID_FILES = {"RTTI_IDs.h", "VTABLE_IDs.h", "NiRTTI_IDs.h"}

RE_ID = re.compile(r"REL::ID\s*[({]\s*(\d+)\s*[)}]")
RE_RELOCATION_ID = re.compile(r"REL::RelocationID\s*[({]\s*(\d+)\s*,\s*(\d+)\s*(?:,\s*(\d+)\s*)?[)}]")
RE_VARIANT_ID = re.compile(r"REL::VariantID\s*[({]\s*(\d+)\s*,\s*(\d+)\s*,\s*(0x[0-9A-Fa-f]+|\d+)\s*[)}]")
RE_OFFSET = re.compile(r"REL::Offset\s*[({]\s*(0x[0-9A-Fa-f]+)\s*[)}]")
RE_VARIANT_OFFSET = re.compile(r"REL::VariantOffset\s*[({]\s*(0x[0-9A-Fa-f]+|\d+)\s*,\s*(0x[0-9A-Fa-f]+|\d+)\s*,\s*(0x[0-9A-Fa-f]+)\s*[)}]")
RE_MARKER = re.compile(r"//.*\bVR:\s*(tested|checked)\b")
RE_FUNC_NAME = re.compile(r"decltype\(\s*&\s*([\w:]+)\s*\)")
RE_FUNC_T = re.compile(r"using\s+func_t\b|_t\s*=\s*[\w:<>*&\s]+\(\s*\*\s*\)")
RE_CALL_SITE = re.compile(r"CallSite\s+(\w+)\s*\{\s*(0x[0-9A-Fa-f]+)\s*,\s*(0x[0-9A-Fa-f]+)\s*\}")
RE_SETTING = re.compile(r"inline Setting\*\s*(\w+)\(\)\s*\{\s*return detail::At\((0x[0-9A-Fa-f]+)\);\s*\}\s*//\s*([\w:]+)")


class PE:
    def __init__(self, path):
        self.data = data = open(path, "rb").read()
        pe = struct.unpack_from("<I", data, 0x3C)[0]
        count = struct.unpack_from("<H", data, pe + 6)[0]
        optional = struct.unpack_from("<H", data, pe + 20)[0]
        self.sections = []
        for i in range(count):
            o = pe + 24 + optional + i * 40
            name = data[o:o + 8].rstrip(b"\0").decode()
            vsize, va, rsize, raw = struct.unpack_from("<IIII", data, o + 8)
            self.sections.append((name, va, max(vsize, rsize), raw))
        exception_rva, exception_size = struct.unpack_from("<II", data, pe + 24 + 112 + 3 * 8)
        o = self.file_offset(exception_rva)
        funcs = sorted(struct.unpack_from("<II", data, o + i * 12) for i in range(exception_size // 12))
        self.func_starts = [f[0] for f in funcs]
        self.func_ends = [f[1] for f in funcs]

    def section(self, rva):
        for name, va, size, _ in self.sections:
            if va <= rva < va + size:
                return name
        return None

    def file_offset(self, rva):
        for _, va, size, raw in self.sections:
            if va <= rva < va + size:
                return raw + (rva - va)
        return None

    def byte(self, rva):
        o = self.file_offset(rva)
        return self.data[o] if o is not None and o < len(self.data) else None

    def function_start_kind(self, rva):
        """'start' for a .pdata entry, 'leaf' for a frameless function after padding, else 'inside' / 'gap'."""
        i = bisect.bisect_right(self.func_starts, rva) - 1
        if i >= 0 and self.func_starts[i] == rva:
            return "start"
        inside = i >= 0 and rva < self.func_ends[i]
        if inside:
            return "inside"
        previous = self.byte(rva - 1)
        return "leaf" if previous in (0xCC, 0xC3) else "gap"

    def call_target(self, rva):
        o = self.file_offset(rva)
        if o is None or self.data[o] != 0xE8:
            return None
        return rva + 5 + struct.unpack_from("<i", self.data, o + 1)[0]

    def qword(self, rva):
        o = self.file_offset(rva)
        return struct.unpack_from("<Q", self.data, o)[0] if o is not None else None

    def cstring(self, va):
        o = self.file_offset(va - IMAGE_BASE)
        if o is None or o >= len(self.data):
            return None
        end = self.data.find(b"\0", o, o + 256)
        return self.data[o:end].decode("latin1") if end > o else None


def load_address_library(path):
    table = {}
    with open(path, encoding="utf-8", errors="replace") as f:
        for row in csv.reader(f):
            if len(row) >= 2 and row[0].isdigit():
                try:
                    table[int(row[0])] = int(row[1], 16)
                except ValueError:
                    pass
    return table


def load_names(path):
    names = {}
    if not path:
        return names
    with open(path, encoding="utf-8", errors="replace") as f:
        for row in csv.reader(f):
            if len(row) >= 5 and row[2].startswith("0x"):
                names.setdefault(int(row[2], 16) - IMAGE_BASE, row[4])
    return names


def context_name(lines, index):
    """The function a relocation belongs to: the decltype(&X) nearby, else the nearest declaration above."""
    for j in range(index, max(-1, index - 6), -1):
        m = RE_FUNC_NAME.search(lines[j])
        if m:
            return m.group(1), True
    is_function = any(RE_FUNC_T.search(lines[j]) for j in range(index, max(-1, index - 4), -1))
    for j in range(index - 1, max(-1, index - 8), -1):
        m = re.search(r"([\w~]+)\s*\([^;]*\)\s*(const)?\s*(noexcept)?\s*$", lines[j].strip())
        if m and not lines[j].strip().startswith(("//", "if", "for", "while", "return")):
            return m.group(1), is_function
    return "?", is_function


def scan_headers(include_dir):
    """Every relocation in the headers, as dicts of file, line, name, kind and its VR target."""
    found = []
    for root, _, files in os.walk(include_dir):
        for file in files:
            if not file.endswith(".h"):
                continue
            path = os.path.join(root, file)
            rel = os.path.relpath(path, include_dir).replace("\\", "/")
            lines = open(path, encoding="utf-8", errors="replace").read().split("\n")
            for i, line in enumerate(lines):
                if line.lstrip().startswith("//"):
                    continue
                entries = []
                for m in RE_RELOCATION_ID.finditer(line):
                    vr = m.group(3) or m.group(1)
                    entries.append(("id", int(vr)))
                for m in RE_VARIANT_ID.finditer(line):
                    entries.append(("offset", int(m.group(3), 0)))
                for m in RE_VARIANT_OFFSET.finditer(line):
                    entries.append(("offset", int(m.group(3), 16)))
                for m in RE_OFFSET.finditer(line):
                    entries.append(("offset", int(m.group(1), 16)))
                if not entries:
                    for m in RE_ID.finditer(line):
                        entries.append(("id", int(m.group(1))))
                if not entries:
                    continue
                marker = RE_MARKER.search(line)
                name, is_function = ("", False) if file in ID_FILES else context_name(lines, i)
                for kind, value in entries:
                    found.append({
                        "file": rel, "line": i + 1, "name": name, "function": is_function and file not in ID_FILES,
                        "kind": kind, "value": value, "marker": marker.group(1) if marker else None,
                        "table": file in ID_FILES,
                    })
    return found


def main():
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("--exe", required=True, help="decrypted Fallout4VR.exe 1.2.72")
    parser.add_argument("--csv", required=True, help="VR address library version-1-2-72-0.csv")
    parser.add_argument("--db", help="fallout_vr_address_library fo4_database.csv, for names")
    parser.add_argument("--include", default=os.path.join(os.path.dirname(__file__), "..", "CommonLibF4", "include"))
    parser.add_argument("--out", help="write the markdown report here")
    args = parser.parse_args()

    pe = PE(args.exe)
    library = load_address_library(args.csv)
    names = load_names(args.db)
    entries = scan_headers(args.include)

    problems = []
    status_counts = Counter()
    marker_counts = Counter()
    table_missing = 0
    for e in entries:
        if e["kind"] == "id":
            if e["value"] == 0:
                e["status"] = "no VR address"
                status_counts[e["status"]] += 1
                continue
            rva = library.get(e["value"])
            if rva is None:
                e["status"] = "ID missing in VR CSV"
                if e["table"]:
                    table_missing += 1
                else:
                    problems.append(e)
                status_counts[e["status"]] += 1
                continue
        else:
            rva = e["value"]
        e["rva"] = rva
        section = pe.section(rva)
        if section is None:
            e["status"] = "outside the image"
            problems.append(e)
        elif e["function"]:
            if section != ".text":
                e["status"] = f"function in {section}"
                problems.append(e)
            else:
                kind = pe.function_start_kind(rva)
                e["status"] = {"start": "ok", "leaf": "ok (leaf)"}.get(kind, f"not a function start ({kind})")
                if kind not in ("start", "leaf"):
                    problems.append(e)
        else:
            e["status"] = "ok (data)" if section != ".text" else "ok (code)"
        status_counts[e["status"]] += 1
        if not e["table"]:
            marker_counts[e["marker"] or "untagged"] += 1

    # VRHookSites and VRSettings carry their own checks
    special = []
    hooks_path = os.path.join(args.include, "RE", "Bethesda", "VRHookSites.h")
    for m in RE_CALL_SITE.finditer(open(hooks_path, encoding="utf-8").read()):
        site, target = int(m.group(2), 16), int(m.group(3), 16)
        actual = pe.call_target(site)
        ok = actual == target
        special.append((f"VRHookSites::{m.group(1)}", site, "ok" if ok else f"call target {actual and hex(actual)} != {target:#x}"))
    settings_path = os.path.join(args.include, "RE", "Bethesda", "VRSettings.h")
    for m in RE_SETTING.finditer(open(settings_path, encoding="utf-8").read()):
        rva, expected = int(m.group(2), 16), m.group(3)
        name_ptr = pe.qword(rva + 0x10)
        actual = pe.cstring(name_ptr) if name_ptr else None
        special.append((f"VRSettings::{m.group(1)}", rva, "ok" if actual == expected else f"name '{actual}' != '{expected}'"))

    out = []
    out.append("# CommonLibF4VR VR address check\n")
    out.append(f"{len(entries)} relocations in the headers, {sum(1 for e in entries if e['table'])} of them in the RTTI / vtable ID tables.\n")
    out.append("| Result | Count |\n|---|---|")
    for status, count in status_counts.most_common():
        out.append(f"| {status} | {count} |")
    out.append(f"\nRTTI / vtable IDs with no row in the VR CSV: {table_missing} (they only matter when used).\n")
    out.append("## VR markers (outside the ID tables)\n")
    out.append("| Marker | Count |\n|---|---|")
    for marker in ("tested", "checked", "untagged"):
        out.append(f"| {marker} | {marker_counts[marker]} |")
    out.append("\n## Problems\n")
    if problems:
        out.append("| Where | Name | VR | Result | Marker |\n|---|---|---|---|---|")
        for e in sorted(problems, key=lambda e: (e["file"], e["line"])):
            target = f"ID {e['value']}" if e["kind"] == "id" else f"{e['value']:#x}"
            out.append(f"| {e['file']}:{e['line']} | {e['name']} | {target} | {e['status']} | {e['marker'] or ''} |")
    else:
        out.append("None.")
    out.append("\n## Hook sites and settings\n")
    out.append("| Name | VR | Result |\n|---|---|---|")
    for name, rva, result in special:
        out.append(f"| {name} | {rva + IMAGE_BASE:#x} | {result} |")
    out.append("\n## Tagged\n")
    out.append("| Where | Name | VR | Marker | Game name |\n|---|---|---|---|---|")
    for e in sorted((e for e in entries if e["marker"]), key=lambda e: (e["marker"], e["file"], e["line"])):
        rva = e.get("rva")
        target = f"{rva + IMAGE_BASE:#x}" if rva is not None else e.get("status", "")
        out.append(f"| {e['file']}:{e['line']} | {e['name']} | {target} | {e['marker']} | {names.get(rva, '') if rva is not None else ''} |")

    report = "\n".join(out) + "\n"
    if args.out:
        open(args.out, "w", encoding="utf-8", newline="\n").write(report)
    summary = ", ".join(f"{s}: {c}" for s, c in status_counts.most_common())
    print(f"{len(entries)} relocations - {summary}")
    print(f"markers - tested {marker_counts['tested']}, checked {marker_counts['checked']}, untagged {marker_counts['untagged']}")
    print(f"problems: {len(problems)}; hook sites / settings failing: {sum(1 for s in special if s[2] != 'ok')} of {len(special)}")
    return 1 if any(e["marker"] for e in problems) or any(s[2] != "ok" for s in special) else 0


if __name__ == "__main__":
    sys.exit(main())
