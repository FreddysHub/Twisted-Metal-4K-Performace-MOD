# Reproduction evidence and limits

## Current playability status — 18 October 2026

FreddysHub confirms that the 4K version is **fully playable when installed and
configured according to the current instructions** (3840 × 2160 internal
rendering; 1280 × 720 base at 300% scale). This is a hands-on maintainer report,
not a claim that the mod is unplayable pending further development. The
repository supplies source/build instructions rather than a bundled game.

The known Metro Square visibility issue remains listed in both READMEs.
1080p and 1440p internal profiles are not supported yet. This report does not
establish bug-free play, a completed independent campaign test, identical
results on every GPU or a guaranteed frame rate.

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

### Published patch encoding corrected, 18 October 2026

A user-reported independent rebuild encountered `No valid patches in input`.
The published patch was confirmed to be UTF-16 LE with BOM and CRLF; the
parsing failure was reproduced. It is now UTF-8 without BOM with LF endings.
Strict decoding and an exact text comparison confirm that only encoding and
line endings changed, not the patch instructions or code.

`git apply --check` passes against a separate, clean checkout of RPCS3 commit
`1707d7fc883ef48ff21bdcbb0141a3211ae09cb2`, covering all 11 patched files.
The source checksum manifest was updated and `.gitattributes` enforces LF for
patch files. [Machine-readable verification](evidence/patch-encoding-check.json).

The user also reports that, after converting the old patch, the independent
build and game startup succeeded on the first attempt. That report is separate
from this encoding verification; no new compile or gameplay test was performed
as part of the encoding-only fix. The encoding issue is unrelated to CPU/GPU
performance or rendering settings.

### Static/dynamic MSVC runtime mismatch in the earlier source rebuild

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
