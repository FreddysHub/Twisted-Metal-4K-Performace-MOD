# Publication privacy and game-file boundary

## What the public package contains

Mod source changes, shader code, patch instructions, generic game configuration,
documentation and explicitly labelled project artwork/development images.
Downloading this repository gives no access to the maintainer's PC.

The game is NOT included. Supply your own matching Twisted Metal PS3 game dump,
update and firmware. No game executables, archives, keys, firmware, saves or
RenderDoc captures are published in the current source tree.

## Review performed on 18 October 2026

- The current tracked tree and locally reachable Git history were checked for
  known credential/token/private-key patterns, MAC-address forms, explicit
  machine/serial/device-identity values, GUID forms and local user paths.
- No credential pattern, MAC-address form or explicit machine/serial identity
  value was found by that scan. This is a limited check, not a security guarantee.
- Remaining user-specific example paths in the current guides were neutralized.
- The redundant hardware JSON export was removed from the current public tree.
  Generic CPU/GPU model descriptions may remain for test context; these are not
  unique machine serial numbers.
- Reviewed historical GUIDs belonged to Visual Studio project metadata and
  an internal analysis entry, not login tokens or computer-access credentials.
  Version-number strings matched by the IP-shaped pattern are not IP evidence.

Game serials, PPU/SPU hashes, source commit hashes, patch addresses and archive
checksums identify the game/code being patched. They are deliberately retained
for reproducibility; they are not the maintainer's PC IDs or access keys.

## History is different from the current file tree

Earlier published commits can still contain old local paths and internal
metadata, even after those files are removed from the latest version. A normal
cleanup commit does not erase that history. Do not claim that GitHub has purged
cached content or existing clones.

A history rewrite requires separate authorization, a local backup and careful
coordination. This current-tree cleanup does not by itself perform that rewrite.
If an actual secret is ever exposed, removing its text is insufficient: revoke
or rotate it with the relevant service.
