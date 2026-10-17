# Reproduction evidence and limits

## Distinguish the runs

| Record | What it establishes | What it does not establish |
| --- | --- | --- |
| Separate source rebuild, 17 October | Pinned source/submodules plus patch/header integration, successful fresh RPCS3 compile, 38 compute-shader variants, 46 recipe/runtime checks, loader exit 0, exact SP/MP Times output hashes | The original close-out did not start the game; not a fresh OS or cross-vendor test |
| Test 4 follow-up | Reported menu startup, matching PPU hash and 3840×2160 fusion log; user gameplay with initial visual issues, then normal behaviour reported | Full campaign, fixed FPS in every scene, long-term stability and a proven cause of initial issues |
| Gemini final-state notes | Author-reported tests and tuning, including reported 45–55+ FPS in some sessions | Independent measurement or support for “rock-solid 60 FPS” |
| Gallery exports | Development capture replay imagery | Latest rebuild, side-by-side original comparison, latency, HDR or FPS |

The rebuild compiled new RPCS3 objects/EXE, while reusing installed build tools,
static LLVM and an upstream FFmpeg dependency package. Installed firmware,
matching game/update data, configuration and private save state were inputs.
No old RPCS3 EXE/objects or shader/SPU caches were used to fake the build.

## Actual build issue found

The first clean link failed with 1287 LNK2038 runtime-library mismatches:
LLVM used static release CRT, RPCS3 was generated with dynamic CRT.
Reconfiguring with MultiThreaded/static MSVC runtime and rebuilding corrected
it. No /FORCE link or error suppression was used. The current guide includes it.

## Recorded binary identities

- Existing protected reference EXE:
  `E7A48F69F06EC319E1D6A04CE073C71E9C1132F3F0B062EDAEA4951D4751C6D2`
- Independent recipe rebuild EXE:
  `70197FF0A42DD708B36A6D57EC6BA15C0BC372505AC42CC52E68193CA1A22FFC`

Different executable hashes do not prove different visuals: paths, toolchains
and build states can change bytes. Matching asset hashes establish those asset
bytes, not rendering correctness. A reproducible build claim should identify
whether it means same source/recipe or bit-identical binaries.

## New checks for publication, 18 October

The public package is reviewed for secret-like tokens, excluded binary/game
files, internal inventories, personal profiles and broken links in current
entry documents. A source-only hash manifest identifies shipped code inputs.
This publication does not rebuild or alter the working game, and does not
perform a new GPU performance benchmark.

The asset-patcher portability fix requires explicit source/target arguments,
validates clean archive hashes and output hash before writing, refuses existing
outputs, and preserves original LTN formatting. It does not change shader or
LOD delta values.
