# VR layouts: lights, the Pip-Boy manager and the camera

Layout fixes and additions made from static reverse engineering of **Fallout4VR.exe 1.2.72**. They
came out of work on F4VR-ImmersiveFlashlight: keeping the flashlight on while the Pip-Boy is open,
and changing a live light's color, radius, FOV and gobo without turning it off and on.

Addresses below are Fallout4VR.exe RVAs (image base `0x140000000`), the same form the header
comments use.

## Summary

| Header | Change | Kind |
| --- | --- | --- |
| `Bethesda/IMenu.h` | `Inventory3DManager` VR layout (`0x150`, not `0x140`) | bug fix |
| `Bethesda/PipboyManager.h` | VR offset and size asserts (members `0x10` higher, `0x200`) | fixed by the above |
| `NetImmerse/NiCamera.h` | VR layout: a multi-view camera, `0x230`, `viewFrustum` is a pointer | bug fix, **API change on VR** |
| `Bethesda/IMenu.h` | seven menus no longer assert their flat size on VR | honesty fix, layouts still wrong on VR |
| `Bethesda/PlayerCharacter.h` | VR asserts for `pipboyLight` / `niPipboyLight` | verification |
| `NetImmerse/NiLight.h` | new `NiLight` | addition |
| `NetImmerse/NiPointLight.h` | new `NiPointLight` | addition |
| `Bethesda/BSLight.h` | new `BSLight`: layout, two virtuals, `GetLuminanceAtPoint`, `Shape` | addition |
| `Bethesda/TESBoundAnimObjects.h` | `TESObjectLIGH::GenDynamic` (ID 30546) | addition |
| `RE/Fallout.h` | includes the three new headers | |

VR-only layouts use the guard the library already uses for them
(`ENABLE_FALLOUT_VR && !ENABLE_FALLOUT_NG && !ENABLE_FALLOUT_F4`). The other configurations keep
the flat layout.

## How it was found

- **Disassembly.** Python with capstone and pefile over the mapped Fallout4VR.exe image. Function
  bounds came from `.pdata`, following chained unwind entries, since many functions keep their
  prologue in a separate entry. Cross-references came from scanning rel32 `call`/`jmp` targets and
  RIP-relative operands. Nothing was run in the game during the investigation.
- **Names.** From the VR address library's `fo4_database.csv` (the `fallout_vr_address_library`
  repo): address-library ID, flat VA, VR VA and a symbol name. Functions with no row there are named
  in the headers for what the code does, and marked as such.
- **Evidence.** Layouts are read from what the code does to the object: constructor stores,
  destructor releases, `CopyMembers`, and accessors whose meaning is clear from their name or use.
  Allocation sizes come from the size passed to the heap right before a constructor call. Each
  header comment cites the functions.
- **Cross-checks.** F4SE's flat `BSLight.h` (identical to LLFP-VR's VR copy), and
  libxse-commonlibf4's NG `NiLight.h`.
- **Verified in the build.** `RE/Fallout.h` compiles in the VR-only, flat-only and all-runtimes
  configurations, with every new `static_assert` holding. That proves the headers are consistent
  with the offsets read from the binary. It doesn't prove the offsets themselves; the binary does.
- **Tested in game, through the mod.** F4VR-ImmersiveFlashlight's Pip-Boy patch and live light
  refresh work on 1.2.72. The mod uses raw offsets rather than these headers, but they're the same
  offsets:
  - `PlayerCharacter::pipboyLight` and `niPipboyLight`;
  - `NiLight::diffuse`, `radius` and `dimmer`;
  - `BSLight::spot.cosHalfAngle`, `geometry`, `camera`, `goboTexture` and `shape` (value 6, spot);
  - `SetShape` and `SetCameraFrustum` as called.

  Everything else still rests on static evidence alone: `Inventory3DManager`, `PipboyManager`,
  `NiCamera`'s layout, and the members the mod doesn't touch.

**Confidence levels:**

- **High:** a VR instruction reads or writes that offset, and its meaning is clear from the code.
- **Medium:** consistent with the VR code, but the name or type comes from another build's headers
  or from inference.
- **Unknown:** the bytes are accounted for, and nothing more.

## The changes

### `Inventory3DManager` (bug fix)

VR inserts two members after `itemBase`: a ref-counted pointer at `0xE8`, released by the destructor
through `DeleteThis`, and a byte at `0xF0`. `disableInputUserCount` moves to `0xF1`, and every
member from `disableRendererUsers` on sits **8 higher** than on flat. The object is `0x150`.

| Member | Flat | VR | Evidence | Confidence |
| --- | --- | --- | --- | --- |
| `unkE8` | — | `0xE8` | constructor `0x13068B0` zeroes it; destructor `0x1306BC0` releases it | High (type unknown) |
| `unkF0` | — | `0xF0` | constructor; `AddLoadedModel` writes a byte there | High (meaning unknown) |
| `disableInputUserCount` | `0xE8` | `0xF1` | `ShouldHandleEvent` `0x13081D0` handles input only while it's 0; `PipboyManager::InitPipboy` increments it | High |
| `disableRendererUsers` | `0xF0` | `0xF8` | constructor stores `_capacity` at `0x104`, `_sentinel` at `0x110`, `_entries` at `0x120`; the destructor and Enable/DisableRendering work on the scatter-table base at `0x100` | High |
| `storedXRotation` | `0x120` | `0x128` | `StoreModelXRotation` writes `this+0x128` | High |
| `minZoomModifier` / `maxZoomModifier` | `0x12C` / `0x130` | `0x134` / `0x138` | written by `AddLoadedModel` | Medium |
| `hightlightedPart` | `0x134` | `0x13C` | `HighlightPart` writes it | High |
| `queueShowItem` … `addedLightsToScene` | `0x138`–`0x13C` | `0x140`–`0x144` | constructor stores; `AddLoadedModel` sets `0x144` | Medium |
| size | `0x140` | `0x150` | `PipboyManager` shift below | High |

A uniform +0x10 shift of the flat names does not fit the tail (`HighlightPart` writes `0x13C`).
The +8 layout above fits every store observed.

### `PipboyManager` (fixed by the above)

`PipboyManager` embeds an `Inventory3DManager` at `0x40`, so on VR every member after it sits 0x10
higher. No member changed, only the asserts. **Confidence: high.** Each asserted member is read or
written at its VR offset by a named function:

- `holotapeToLoad` `0x190`: `PlayPipboyLoadHolotapeAnim` `0xC32D50`.
- `menuToOpen` / `openAnimEvent` / `closeAnimEvent` `0x1A0` / `0x1A8` / `0x1B0`:
  `PlayPipboyGenericOpenAnim` `0xC32ED0`, `OnPipboyClosed` `0xC339B0`.
- `loweringReason` `0x1B8`: `ProcessLoweringReason` `0xC352C0`.
- `itemAnimOnClose` `0x1C0`: `PlayItemAnimOnClose` `0xC33DD0`.
- `fastTravelLocation` `0x1C8`: `OnPipboyClosed`.
- `pipboyOpening` `0x1D6`: the open-anim functions.
- `pipboyActive` `0x1D8`: `QPipboyActive` `0xC33D60` reads its value at `+8` under its lock at `+0xC`.
- `autoSaveOnClose` `0x1F1`: `OnPipboyClosed` and the close handler `0xC337A0`.
- `wasMotionBlurActive` / `wasPipboyLightActive` `0x1F4` / `0x1F5`: `InitPipboy` `0xC34780`.

`InitPipboy` stores `IsPipboyLightOn()` in `wasPipboyLightActive` and hides the light when it's set;
the close handler shows it again. That is why the Pip-Boy turns the flashlight off. In power armor
the flag is stored false.

### `NiCamera` (bug fix, API change on VR)

The flat layout put members at `0x120`. VR's `NiAVObject` is 0x40 larger (as `NiNode` already
models), and VR's camera is a different class: it renders several views, and the constructor takes
their count.

| Member | VR | Evidence | Confidence |
| --- | --- | --- | --- |
| `pad120[0x10]` | `0x120` | VR's `NiAVObject` extra 0x40 | High |
| `worldToCam` | `0x160` | `WorldPtToScreenPt3` `0x1C2D860` passes `this+0x160`; `WorldPtToScreenPtNoClip` `0x1C2D6F0` reads its rows | High |
| `viewFrustums` | `0x1A0` | `BSTArray` of `0x1C`-byte elements; constructor `0x1C2BAD0`, destructor `0x1C2BDC0`; `SetViewFrustum` `0x1C2BFA0` writes each one | High |
| `unk1B8` / `unk1D0` / `unk1E8` | | `BSTArray`s of 12, 12 and 0x30-byte elements; typed `NiPoint3` / `NiPoint3` / `NiMatrix3` by size only | Unknown (types guessed) |
| `viewFrustum` | `0x200` | pointer to the current view's frustum; the constructor points it at the last element, and `SetViewFrustum` writes through it | High |
| `numViews` | `0x208` | the constructor stores its count argument | Medium (name) |
| `minNearPlaneDist` / `maxFarNearRatio` | `0x20C` / `0x210` | constructor defaults 0.1 / 10000; `SetViewFrustum` clamps with them | High |
| `port` | `0x214` | `WorldPtToScreenPt3` passes `this+0x214`; constructor defaults 0 / 1 / 1 / 0 | High |
| `lodAdjust` | `0x224` | constructor default 1.0 | Medium |
| size | `0x230` | `BSShadowFrustumLight`'s constructor and the gobo projection camera (`0x286FC30`) allocate `0x230` | High |

After this change every direct `NiAVObject` subclass in the library accounts for VR's 0x40:
`NiNode` and `BSGeometry` already did, and `NiCamera` and `NiLight` now do.

### The seven menus that embed `Inventory3DManager`

`WorkshopMenu`, `ContainerMenuBase`, `ContainerMenu`, `BarterMenu`, `WorkbenchMenuBase`,
`ExamineMenu` and `PowerArmorModMenu` embed an `Inventory3DManager`, so fixing it grew them by 0x10.
Their flat size asserts then failed on VR. Those asserts had only held before because they never
checked VR. Fallout4VR.exe allocates these sizes:

| Menu | Flat | VR allocation | Where |
| --- | --- | --- | --- |
| `WorkshopMenu` | `0x440` | `0x470` | `0xC12D27` |
| `ContainerMenu` | `0x440` | `0x490` | `0x12EB060` |
| `BarterMenu` | `0x580` | `0x5A0` | `0xB21BB7` |
| `ExamineMenu` | `0x810` | `0x890` | `0xB520C7` |
| `PowerArmorModMenu` | `0x850` | `0x8D0` | `0xBB8D04` (then stores the `PowerArmorModMenu` vtable) |

On VR the size asserts are replaced by comments giving these numbers. **Confidence: high** for the
sizes. **The member layouts past `inv3DModelManager` are still wrong on VR:** the extra bytes
aren't only `Inventory3DManager`'s, and where they sit is unmapped. Another `ExamineMenu` subclass
is allocated `0x8B0` at `0xB34345`; it was not identified.

### `PlayerCharacter`

Asserts for `pipboyLight` at `0x10A8` and `niPipboyLight` at `0x10B0` (flat `0xC38` / `0xC40` plus
the existing 0x470 VR block). `IsPipboyLightOn` `0xF27790` tests the first, and `ShowPipboyLight`
`0xF277B0` writes both. **Confidence: high.**

### `NiLight` (new)

The NG layout from libxse-commonlibf4; on VR each member is 0x40 higher.

| Member | Flat | VR | Evidence | Confidence |
| --- | --- | --- | --- | --- |
| `ambient` | `0x120` | `0x160` | constructor `0x1C362A0`, `CopyMembers` `0x1C35F90` | High |
| `diffuse` | `0x12C` | `0x16C` | `TESObjectLIGH::GenDynamic` `0x306A50` writes the form color / 255; `BSLight::GetLuminanceAtPoint` `0x286EA60` reads it | High |
| `radius` | `0x138` | `0x178` | NetImmerse's specular slot. `GenDynamic` writes the form radius to all three components; `GetLuminanceAtPoint` divides distance by it | High |
| `dimmer` | `0x144` | `0x184` | `GenDynamic` writes the form's `fade`; `GetLuminanceAtPoint` scales by it | High |
| `modelBound` | `0x150` | `0x190` | constructor sets radius -1; `BSLight`'s shape setter copies the light volume's bound here | High |
| `rendererData` | `0x160` | `0x1A0` | constructor zeroes it | Medium (name) |
| `unk168` | `0x168` | `0x1A8` | constructor zeroes a byte | Unknown |
| size | `0x170` | `0x1B0` | `NiPointLight` members start at VR `0x1B0` | High (VR) / Medium (flat, from libxse) |

### `NiPointLight` (new)

Every `TESObjectLIGH` light is an `NiPointLight`, spot lights included (the `BSLight` gives a spot
its cone).

| Member | VR | Evidence | Confidence |
| --- | --- | --- | --- |
| `attenConstant` / `attenScalar` / `attenExponent` | `0x1B0` / `0x1B4` / `0x1B8` | constructor `0x1C27E80` (0, 1, 2), `CopyMembers` `0x1C27B90`; `GetLuminanceAtPoint` computes `constant + scalar * pow(distance / radius, exponent)` | High |
| `unk17C[0x14]` | `0x1BC` | nothing initializes or copies it | Unknown |
| size | `0x1D0` | `GenDynamic` allocates `0x1D0`; `NiSpotLight`'s members start at `0x1D0` (constructor `0x1C55150`) | High (VR) |

The flat size (`0x190`) is VR minus 0x40, **unverified**.

### `BSLight` (new)

Layout and most names from F4SE's flat `BSLight.h`, which is 0x190 on both flat and VR. The VR
constructor (`0x286D510`, allocated `0x190` by the light factory `0x27E9CD0`) initializes the same
fields, and every offset below was checked in VR code:

- `luminance` `0x14`: written by `GetLuminanceAtPoint`.
- `frustumCull` `0x18`: set by the shadow lights' per-frame update.
- `goboProjection` `0x20`: passed with the gobo camera to `0x286F350`, which presumably fills it;
  vtable slot 4 (`0x28734F0`) copies it out.
- `shapeRotation` `0x60`: set for box lights.
- `pointPosition` `0x90`: `SetLight` copies the light's position there for a dynamic point light.
- `spot` `0xA0`: `falloffExponent` is `max(form falloff, 1)`; `cosHalfAngle` is `cos(FOV / 2)`,
  read by `GetLuminanceAtPoint`.
- `light` `0xB8`.
- `geometry` / `camera` / `goboTexture` `0x148` / `0x150` / `0x158`: the light volume, the gobo
  projection camera made by `0x286FC30`, and the gobo. The per-frame light pass `0x2846D60` reads
  the gobo.
- The bools `0x170`–`0x17E`: filled by the factory from `TES_LIGHT_FLAGS` (`kPortalStrict`,
  `kNonSpecular`, `kAttenutationOnly`, `kIgnoreRougness`, `kNoRimLighting`, `kAmbientOnly`).
- `shape` `0x180`.
- `lensFlare` `0x188`.

**Confidence: high** for the offsets and the size. **Medium** for F4SE's names on the members not
listed above.

- **Virtuals.** Slot 2 is `SetLight(NiLight*)` (`0x286E870`): it assigns `light` and flags
  `pointLight` / `ambientLight` by NiRTTI. Slot 3 is `IsShadowLight()`: `BSLight` returns false
  (`0x28734D0`) and `BSShadowLight` true (`0x29110A0`); `ShadowSceneNode::UpdateQueuedLight`
  `0x27EB2C0` branches on it. **High.** Slot 4 is not declared: it copies `goboProjection` into its
  third argument, and the second argument's role is unknown.
- **`Shape`.** `SetShape` (`0x286F9B0`) jumps on the value. The factory assigns 2 (omni) by default,
  3 (hemi) for `kHemiShadow`, and 6 (spot: a cone from `BSShaderUtil::GenerateCone`) for spot flags.
  5 (box) is assigned when the reference links a primitive, whose extents it copies. 1 and 4 exist
  in the jump table with unknown meaning, so they aren't named. **High** for the named values;
  "box" is a name for the primitive case.
- **`GetLuminanceAtPoint`.** ID 170662 resolves to `0x286EA60`. It returns 0 when the light's
  `NiLight` is the one passed in. **High.**

### `TESObjectLIGH::GenDynamic` (new)

ID 30546 resolves to `0x306A50`. It builds an `NiPointLight` under the parent (or reuses one it
finds there) and its `BSLight`, and adds them to the scene. The parameter types come from the
address-library name.

- **Named with high confidence:** `a_dynamic` (ORed into the dynamic flag) and `a_noShadows` (turns
  shadowed spot/hemi/omni flags into their shadowless forms).
- **Medium:** `a_ignoreRefRadius`, which keeps the form's radius over the reference's.
- **Unknown:** `a_unk5` and `a_unk7`.

`PlayerCharacter::ShowPipboyLight` calls it as `(nullptr, node, true, true, false, &pipboyLight, <a
global>, false)`.

## Implications

- **Recompile to pick up the fixes.** On VR builds, `Inventory3DManager` (and so `PipboyManager`)
  and `NiCamera` members move. Plugins built against the old headers read and wrote the wrong
  memory through them.
- **`NiCamera::viewFrustum` is a pointer on VR.** Code that used it as a value stops compiling on VR
  builds. That is intentional: the value at the old offset was part of `worldToCam`.
- **`Inventory3DManager` gains `unkE8` / `unkF0` on VR.** Names are unchanged.
- **Don't use the seven menus' members past `inv3DModelManager` on VR.** Their layouts there are
  still wrong; only the false size assert is gone.
- **The all-runtimes configuration keeps flat layouts at compile time**, as `BSShaderMaterial`
  does. These classes are therefore VR-correct only in VR-only builds; only `NiNode` has a runtime
  accessor (`GetRuntimeData`).
- **The new classes are in `RE/Fallout.h`.** `BSLight` is now a complete type, so the
  `NiPointer<BSLight>` in `PlayerCharacter` instantiates normally.
- **Flat and NG are only partly checked.** Only Fallout4VR.exe was examined. `BSLight`'s flat layout
  rests on F4SE, `NiLight`'s on libxse-commonlibf4 (NG), and `NiPointLight`'s flat size on
  inference. The flat `Inventory3DManager` / `PipboyManager` / `NiCamera` layouts are unchanged from
  before.
- **Only the offsets the mod uses are confirmed in game.** Everything else here is static.

## Next worth doing

1. **Map the seven menus' VR layouts.** The sizes above say where to look: `ExamineMenu` and
   `PowerArmorModMenu` are 0x80 larger, which points at `WorkbenchMenuBase`.
2. **Find VR addresses for IDs missing from the VR address library**, and supply them with
   `REL::VariantID(f4, ng, vrOffset)`, which no header uses yet.
   - Hit here: `PipboyManager::OnPipboyCloseAnim` (1231000), `OnPipboyCloseAnimFailed`,
     `OnPipboyOpenAnimFailed`, `PlayPipboyOpenAnim`, `PlayPipboyOpenTerminalAnim`, `RaisePipboy`,
     `LowerPipboy`, `RefreshPipboyRenderSurface`, `UpdateCursorConstraint`, `DisablePipboyShader`,
     `EnablePipboyShader`; `Inventory3DManager::Begin3D` and `SetModelScale`. None of these has a row
     in `fo4_database.csv`, not even a flat address, so they need identifying by behavior. The close
     handler `0xC337A0` behaves like `OnPipboyCloseAnim`: `PlayPipboyCloseAnim` calls it, and it
     stops Pip-Boy camera mode, restores motion blur and the light, and queues the autosave. That
     match is unconfirmed.
   - A script checking every `REL::ID` in the headers against the VR database would list the rest.
3. **Add `BSShadowLight` / `BSShadowFrustumLight`.** Known on VR so far:
   - render data at `0x198`, with the shadow `NiCamera` at `+0x40`;
   - a float from `GenDynamic` at `0x1D0`;
   - the frustum light's near / far at `0x1F0` / `0x1F4`;
   - sizes `0x200` (frustum) and `0x1F0` (parabolic: hemi / omni, constructor `0x2915210`);
   - vtable slot 2 (`0x2912550`) re-applies the frustum on `SetLight`, and slot 15 (`0x2912D00`) is
     the per-frame shadow camera update. It rebuilds near/far each frame but keeps the frustum's
     sides, so a FOV change needs a `SetCameraFrustum` call.
4. **Add `ShadowSceneNode`.** `AddLight(NiLight*, bool)` 1029787, `AddLight(BSLight*)` 867668,
   `RemoveLight(NiPointer<BSLight>&)` 1410391 and `RemoveLight(NiLight*)` 1158402 all resolve on VR.
   The main scene node is the global at `0x6879520`, which `ShowPipboyLight` and the light factory
   use. The layout is the work.
5. **Add the `BSLight` functions without an address-library ID.** `SetShape` `0x286F9B0` (rebuilds
   the light volume; returns early for an unchanged type), `SetCameraFrustum` `0x286F180`, and the
   light factory `0x27E9CD0`. They live in F4VR-CommonFramework's `F4VROffsets.h` for now. They need
   flat IDs, or `VariantID` with only a VR offset.
6. **Fix `PlayerCharacter::TogglePipBoyLight(bool)`.** The function takes no argument (`520007`,
   `0xF27720` ignores `rdx`). Its signature was left alone to avoid breaking callers.
7. **Fill in the unknowns.** `NiCamera`'s three extra arrays, `numViews`; `NiPointLight`'s trailing
   0x14 bytes; `NiSpotLight` (VR constructor: a direction-like `NiPoint3` at `0x1D0`, zeros at
   `0x1DC` / `0x1E0`, 1.0 at `0x1E4`); `BSLight` vtable slot 4 and its `unk` members; the type of
   `Inventory3DManager::unkE8`.
8. **Check the rest in game.** For example: read `PipboyManager::pipboyOpening` / `wasPipboyLightActive`
   while opening the Pip-Boy, or a shadow light's camera `viewFrustum->near` against its
   `BSShadowFrustumLight` near.
9. **Add runtime accessors** like `NiNode::GetRuntimeData` for these VR layouts, if the
   all-runtimes build needs them to be correct on VR.
