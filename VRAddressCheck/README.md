# VR address check

`vr_address_check.py` checks every address CommonLibF4VR uses on Fallout 4 VR against the game, with no game
running. It needs Python 3 and nothing else.

```
python vr_address_check.py --exe Fallout4VR-decrypted.exe --csv version-1-2-72-0.csv --db fo4_database.csv --out report.md
```

- `--exe`: Fallout4VR.exe 1.2.72 with its code decrypted. The Steam copy's `.text` is encrypted on disk, so
  decrypt it with [Steamless](https://github.com/atom0s/Steamless) first.
- `--csv`: the `version-1-2-72-0.csv` of [VR Address Library for F4SEVR Plugins](https://www.nexusmods.com/fallout4/mods/64879),
  the file plugins load at runtime.
- `--db` (optional): `fo4_database.csv` from [fallout_vr_address_library](https://github.com/alandtse/fallout_vr_address_library),
  which names the functions in the report.
- `--include` defaults to `../CommonLibF4/include`.

## What it checks

Each relocation in the headers is resolved the way `REL` resolves it on VR: an ID through the CSV, the VR
offset of a `VariantID` or `VariantOffset` as is. Then:

| Result | Meaning |
|---|---|
| `ID missing in VR CSV` | the ID resolves to 0 on VR, so calling it crashes. Give it a `VariantID` with the VR offset. |
| `not a function start` | a function's address is inside another function, or between functions. |
| `function in .data` | a function's ID maps to data on VR: the ID is wrong for VR. |
| `ok`, `ok (leaf)` | a function at a `.pdata` entry, or a frameless leaf function right after `int3` / `ret` padding. |
| `ok (data)`, `ok (code)` | anything else that resolves inside the image. |

The call sites in `VRHookSites.h` must still be `call rel32` to their callee, and the Setting objects in
`VRSettings.h` must carry the name they are declared for.

The exit code is 1 when a tagged relocation, a hook site or a setting fails, so the check can gate a change.

## VR markers

A relocation's line carries a marker comment that says how far it is known to work on Fallout 4 VR:

- `// VR: tested`: called on VR in game and seen to work (a mod that ships it counts).
- `// VR: checked`: address and signature read off the VR binary (function start, the arguments it reads,
  what it returns), but not called in game.
- `// VR: broken (why)`: known not to work on VR, with the reason (usually an ID the VR address library
  lacks, with no VR address found for it yet). Calling it on VR crashes.
- no marker: unknown on VR.

Tag a relocation in the same change that adds or verifies it. The report counts the markers, lists the
known-broken ones with their reasons apart from new problems, and lists every tested or checked relocation
with the game's own name for its address.

The exit code is also 1 when a relocation tagged broken resolves after all, so its tag gets a look.
