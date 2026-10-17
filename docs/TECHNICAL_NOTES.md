# Technical notes: what is changed, and what is not

## PostFX and the performance path

The relevant local patch is **PostFX Sharp Fast**, version 0.5, for the exact
US 01.06 PPU hash. Its definition contains eleven PowerPC NOP replacements:

`0066aaf4, 0066ab30, 0066ab74, 0066ab84, 0066ab90, 0066ab9c,
0066aba8, 0066abb4, 0066abc0, 0066ac1c, 0066ac38`.

According to the selective patch's documented intent, expensive stages including
motion blur/MLAA are skipped while initialization, bloom prerequisites and
final tone-map/colour work are retained. It is NOT a blanket disable of every
PostFX job. The independent Raw 4K Scene experiment bypasses more stages and
remains disabled. The community 60 FPS patch and SPU MLAA-disable patch are
separate inputs. No controlled per-stage benchmark here assigns the full
historical improvement to one instruction or proves 60 FPS everywhere.

Source: [patch definition](../03_QUELLCODE/configs/patches/BCUS98106_patch.yml)
and [activation state](../03_QUELLCODE/configs/patch_config.yml).

## Resolution, colour and timing

RPCS3's 300% scale produces a 3840 × 2160 scene target from 1280 × 720.
The custom VKPresent/TM4K compute pipeline reconstructs presentation using
scene detail, lower-resolution colour/overlay data and particle inputs.

8,294,400 is the output pixel count, not the texel count of each game asset.
Earlier references to “8.1 million pixels without deviation” were comparison
metrics, not a user-adjustable per-texture density setting. MASTER_DENSITY is
a mode selector in VKPresent.cpp, not a numeric pixel-budget allocator.

Pairing and registration matter: mixing different frames can create lagging
contours; non-matching crop/scale can displace layers. A single offline still
does not demonstrate correct motion, menu alpha or frame-to-frame stability.
The retained image latch is a fallback, not creation of new game frames.

The reference inset crop maps source (39,24)..(1241,696), scaled with the scene,
to the full output. Matching the 2D and 3D transform is important. This removes
an inset border; it does not invent off-screen scene content.

Shader HDR/FP16 scene processing is distinct from Windows Auto HDR. The mod
changes RPCS3's user-space Vulkan backend, not AMD/NVIDIA/Intel's Windows driver.

## LOD and streaming

The game-patch reference extends two constants to 10000 and 16000 game units.
These are not independently measured physical kilometre distances.
Global HierLOD multiplier experiments at 0x0010e83c remain disabled.

The Times asset-delta recipe targets eligible nodes using term <= 3000 and
radius <= 80, with Near=5000, Term=5600, Load=5200, Unload=5500.
These values describe that recipe, not a universal all-map effect or a promise
that every distant tree, lamp or sign uses full near-detail textures.

Only SP and MP times.psarc are rebuilt by the bundled delta patcher. The
additional times/suburbs LTN overrides disable Distance Blur, not all fog.
Do not conflate distance fog, depth of field, motion blur, mipmaps and model LOD.

## Compatibility and limits

Reference hardware: Intel Core i9-11900K, AMD Radeon RX 7900 XT.
Other vendors, handheld profiles and alternate regions are not established
by that test. Empty Adapter avoids a hard-coded development GPU name; it
does not itself certify a universal fastest-device choice.

The setup verifier is an approximate file/configuration check. Its historical
8/8 count is not a proof that assets, DLLs, controller input or gameplay work.
Treat diagnostic hypotheses as hypotheses and capture actual repeatable defects.
