# Build TM4K from source

This guide takes precedence over older development notes. Reference target:
Windows x64, Vulkan, Twisted Metal US BCUS98106 update 01.06. This is not an
installer and does not download the game, updates, firmware or private caches.
Preserve your existing installation and saves.

> **Current release: internal 4K only.** Use 1280 × 720 base resolution with
> 300% scale (3840 × 2160). Maintainer testing reports texture/layer misalignment
> when changing internal rendering to 1080p or 1440p. Dedicated profiles for those
> resolutions are planned but not yet supported. Do not lower scale as a drop-in
> performance preset for the current reconstruction pipeline.

## 1. Prerequisites

- Your own game dump and installed matching update. Expected PPU:
  `6bb4db395550102f567fb7b1dbbf81583417150f`.
- PS3 firmware installed through RPCS3's normal firmware installation flow.
- Git, Python 3.10+, Visual Studio 2026 C++ x64 tools, CMake 4.2+ for that generator.
- Reference tool versions: Qt 6.11.2 MSVC package including required modules,
  Vulkan SDK 1.4.341.1, compatible static RPCS3 LLVM libraries built with `/MT`.
- Enough disk space for source/submodules, dependencies, build products and your
  game data. A Vulkan-capable GPU alone does not establish performance compatibility.

See the pinned upstream [build reference](03_QUELLCODE/build-reference/BUILDING.md)
for dependency details. A full independent fresh-machine build remains to be
validated. The recorded source rebuild reused installed toolchains and LLVM.

Do not disable antivirus, Smart App Control or other system protections.
A Defender exclusion is not a signing solution; PowerShell execution policy
does not resolve a blocked native EXE. Diagnose any specific block separately.

## 2. Pin and patch the source

Use a new, short directory, for example `C:\tm4k`. In Developer PowerShell:

```powershell
$work = 'C:\tm4k'
# Set this to YOUR clone/download of this mod repository:
$mod = 'C:\path\to\Twisted-Metal-4K-Performace-MOD'
$src = Join-Path $work 'rpcs3'
$build = Join-Path $work 'build'
$bin = Join-Path $work 'bin'
if (Test-Path -LiteralPath $src) { throw 'Choose a fresh source directory.' }
New-Item -ItemType Directory -Path $work -Force | Out-Null
git clone https://github.com/RPCS3/rpcs3.git $src
if ($LASTEXITCODE) { throw 'Clone failed.' }
git -C $src checkout 1707d7fc883ef48ff21bdcbb0141a3211ae09cb2
if ($LASTEXITCODE) { throw 'Checkout failed.' }
git -C $src -c core.longpaths=true submodule update --init --recursive --jobs 4
if ($LASTEXITCODE) { throw 'Submodule checkout failed.' }
git -C $src apply --check "$mod\03_QUELLCODE\tracked-changes.patch"
if ($LASTEXITCODE) { throw 'Patch does not match the pinned source.' }
git -C $src apply "$mod\03_QUELLCODE\tracked-changes.patch"
if ($LASTEXITCODE) { throw 'Patch failed.' }
Copy-Item "$mod\03_QUELLCODE\overlay\rpcs3\Emu\RSX\VK\TM4K*.hpp" "$src\rpcs3\Emu\RSX\VK"
```

Apply the full tracked patch, not just VKPresent.cpp. It includes renderer,
shader-prologue and MSVC build changes. The eight additional TM4K headers are
not contained in the tracked-file patch and must also be copied.

## 3. Configure, compile and deploy dependencies

Replace the three dependency paths with your installation. `$llvm` must point
to the directory containing `LLVMConfig.cmake`, not an arbitrary LLVM root.

```powershell
$qt = 'C:\SDK\Qt\6.11.2\msvc2022_64'
$llvm = 'C:\SDK\rpcs3-llvm\lib\cmake\llvm'
$vulkan = 'C:\VulkanSDK\1.4.341.1'
cmake -S $src -B $build -G 'Visual Studio 18 2026' -A x64 `
  -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded -DUSE_STATIC_MSVC_RUNTIME=ON `
  -DABSL_MSVC_STATIC_RUNTIME=ON -Dprotobuf_MSVC_STATIC_RUNTIME=ON `
  -DBUILD_LLVM=OFF -DWITH_LLVM=ON -DSTATIC_LINK_LLVM=ON `
  -DLLVM_USE_INTEL_JITEVENTS=ON "-DLLVM_DIR=$llvm" `
  "-DQt6_ROOT=$qt" "-DVulkan_ROOT=$vulkan" `
  -DUSE_PRECOMPILED_HEADERS=ON -DUSE_NATIVE_INSTRUCTIONS=ON -DUSE_LTO=ON `
  -DUSE_SDL=OFF -DUSE_FAUDIO=OFF -DUSE_SYSTEM_CURL=OFF `
  -DUSE_SYSTEM_OPENCV=OFF -DUSE_SYSTEM_ZLIB=OFF -DUSE_SYSTEM_OPENAL=OFF
if ($LASTEXITCODE) { throw 'CMake failed.' }
msbuild "$build\rpcs3\Emu\rpcs3_emu.vcxproj" /p:Configuration=Release /p:Platform=x64 /m:2 /p:CL_MPCount=4
if ($LASTEXITCODE) { throw 'Emulator core build failed.' }
msbuild "$build\rpcs3\rpcs3.vcxproj" /p:Configuration=Release /p:Platform=x64 /m:2 /p:CL_MPCount=4
if ($LASTEXITCODE) { throw 'Application build failed.' }
```

`USE_NATIVE_INSTRUCTIONS=ON` is for a local build on that CPU, not a portable
binary release. For distribution, review CPU-target and dependency settings.
Lower concurrency for machines with less memory. The recorded MSVC CRT failure
was fixed by matching the static runtime above, not by ignoring LNK2038 errors.

Find the newly linked EXE in the generated build output. Copy it to a **new**
`$bin` directory, along with its required FFmpeg/other runtime dependencies.
Do not take an EXE or compiled RPCS3 objects from an older mod installation.
Deploy Qt from the same SDK:

```powershell
# First copy the newly linked EXE and its build-produced dependencies into $bin.
& "$qt\bin\windeployqt.exe" --release --no-translations "$bin\rpcs3.exe"
if ($LASTEXITCODE) { throw 'Qt deployment failed.' }
& "$bin\rpcs3.exe" --version
if ($LASTEXITCODE) { throw 'Loader check failed.' }
```

Qt deployment alone does not install PS3 firmware or all non-Qt dependencies.
The upstream build layout and generated dependency locations must be checked.
Do not copy directories through junctions to a working emulator.

## 4. Configure the isolated runtime

Initialize firmware and your matching game/update using RPCS3's normal flows.
Do not accept or suppress first-run terms on someone else's behalf.

- Copy the five `03_QUELLCODE/flags/*.flag` files beside the new EXE/config root.
- Copy `03_QUELLCODE/configs/custom_configs/config_BCUS98106.yml` to
  `$bin\config\custom_configs\config_BCUS98106.yml`.
- Copy both `03_QUELLCODE/configs/patches/*.yml` definitions to `$bin\patches`.
- Copy `03_QUELLCODE/configs/patch_config.yml` to `$bin\config\patch_config.yml`.
- Preserve existing controller profiles. On a fresh install use **Pads** to
  select your own handler/device, save, and test actual inputs.

The supplied configuration uses Vulkan, 300% scale, normal Async Recompiler,
async texture streaming disabled and Minimum Scalable Dimension 256.
The GPU adapter field is empty to avoid tying the profile to the development
GPU name; confirm the actual selected device on multi-GPU systems.

Do not enable Raw 4K Scene or the global +50%, +100%, x4, zero-distance LOD
experiments. Keep their reference values disabled. This package is not a
guarantee for any other game or game executable hash.

## 5. Rebuild the level overrides

Use clean extracted disc inputs and a NEW staging output, not the only copy
of your working game files. The patcher requires explicit paths and checks
clean archive hashes before writing:

```powershell
python "$mod\03_QUELLCODE\apply_times_ngp_delta.py" `
  --disc-dir 'D:\YourGameDump\PS3_GAME\USRDIR' `
  --target-dir 'C:\tm4k\asset-staging'
if ($LASTEXITCODE) { throw 'Asset reconstruction failed.' }
```

Review all four generated files, then install them under the isolated game's
`dev_hdd0\game\BCUS98106\USRDIR`, retaining originals. No PSARC archive is supplied.

| Archive | Expected output SHA256 |
| --- | --- |
| levels/times.psarc | `9129889C700E23CB61569EA90722E0E8F0A64B00CB2D4F4750BAA51DD407A22C` |
| mplevels/times.psarc | `757C01A688C74918AB9A1CBBBE59BF1D8787D0C5BA44391059E798E96159E90A` |

The two `levels/times.ltn` and `levels/suburbs.ltn` overrides change only
Distance Blur Enable 1 → 0. The PSARC deltas target selected Times nodes;
they are not a universal all-level 5-kilometre streaming switch.

## 6. Launch and verify

Use the supplied generic launcher, replacing paths with your isolated runtime
and game. It sets seven process-local TM4K variables, not global Windows settings:

```powershell
& "$mod\scripts\Start-TwistedMetal-4K.ps1" -BinDir $bin -GamePath 'D:\YourGame.iso'
```

Close other RPCS3 instances first. Run the setup check separately:

```powershell
& "$mod\03_QUELLCODE\Verify-TM4K-Setup.ps1" -TargetDir $bin
```

This is a limited setup diagnostic, not certification. Inspect the runtime log
for the correct PPU hash, active patches, shader mode and 3840 × 2160 fusion.
Test controller inputs, logos/menu, vehicle selection, fire, bright/dark levels,
fast movement and distant roads/buildings. Repeat with warmed caches.

Cold-cache builds can stutter or temporarily miss shaders. Repeated black roads,
white flashes, wrong colours or crashes are defects to investigate, not something
to dismiss because an arbitrary cache-file count or elapsed time was reached.
Windows Auto HDR is optional and separate; test SDR first.
