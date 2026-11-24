# CommonLib F4 VR

This is a VR split of CommonLibF4 from [alandtse/CommonLibF4](https://github.com/alandtse/CommonLibF4) as
1. VR and pre-NG stopped getting updates.
2. Almost all CommonLibF4 updates are in [libxse/commonlibf4](https://github.com/libxse/commonlibf4) that doesn't support VR.
3. Multi-targetting while VR speciffic fixes is hard and not worth it at this point (IMHO).


## What works on VR

A relocation's line is tagged `// VR: tested` (called in game on VR and seen to work), `// VR: checked`
(address and signature read off the VR binary) or `// VR: broken (why)` (known not to work on VR), and untagged
when it is unknown on VR.
[VRAddressCheck](VRAddressCheck/README.md) checks every address against a decrypted Fallout4VR.exe and the VR
address library, and reports the tags.

VR-only engine pieces with no address-library ID live in `RE/Bethesda/VR*.h`: `VRSettings` (the [VR] settings
the game reads), `VRWorldData` (the room rotation turning uses), `VRRenderCamera` (the per-eye render camera)
and `VRHookSites` (call and patch sites mods hook, each with what the unpatched game has there).


## General Requirements

- [Visual Studio Community](https://visualstudio.microsoft.com/vs/preview/)
- [clang-format 12.0.0](https://github.com/llvm/llvm-project/releases)
- [CMake](https://cmake.org/)
- [vcpkg](https://github.com/microsoft/vcpkg)


## F4SE Plugin Requirements

### End User Dependencies

- [VR Address Library for F4SEVR Plugins](https://www.nexusmods.com/fallout4/mods/64879)
- [F4SEVR](https://f4se.silverlock.org/)

### Development

- [Address Library for F4SE Plugins](https://www.nexusmods.com/fallout4/mods/47327)
- [VR Address Library for F4SEVR Plugins](https://www.nexusmods.com/fallout4/mods/64879)

## Credits

- Thanks to [Ryan McKenzie](https://github.com/Ryan-rsm-McKenzie) for original code.
- Thanks to [Alan Tse](https://github.com/alandtse) for the fork code.
- Thanks to [CharmedBaryon](https://github.com/CharmedBaryon) for [NG code](https://github.com/CharmedBaryon/CommonLibSSE-NG) and initial porting to F4 by [alandtse](https://github.com/alandtse)
- Thanks to maintainers of active forks:
  [po3](https://github.com/powerof3/CommonLibF4)
  [shad0wshayd3](https://github.com/shad0wshayd3/CommonLibF4)
