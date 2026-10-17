#!/usr/bin/env python3
"""
Reproduzierbares Asset-Patch-Skript für Twisted Metal (2012) BCUS98106.
100% autark: keine externen Abhängigkeiten, benötigt nur Standard-Python 3.

Erzeugt aus sauberen Disc-Dateien die verifizierten 5000m-Sichtweiten- und Anti-Blur-Dateien:
1. SP levels/times.psarc:   SHA256 9129889C700E23CB61569EA90722E0E8F0A64B00CB2D4F4750BAA51DD407A22C
2. MP mplevels/times.psarc: SHA256 757C01A688C74918AB9A1CBBBE59BF1D8787D0C5BA44391059E798E96159E90A
3. levels/times.ltn:        Distance Blur deaktiviert (1 -> 0)
4. levels/suburbs.ltn:      Distance Blur deaktiviert (1 -> 0)
"""

import sys, json, hashlib, struct, zlib, re, argparse
from pathlib import Path

class Entry:
    def __init__(self, digest: bytes, block_index: int, length: int, offset: int):
        self.digest = digest
        self.block_index = block_index
        self.length = length
        self.offset = offset

class Psarc:
    def __init__(self, path: Path):
        self.path = path
        self.data = path.read_bytes()
        if self.data[:4] != b'PSAR':
            raise ValueError(f"Keine gültige PSARC-Datei: {path}")
        (
            self.version,
            compression,
            self.toc_size,
            entry_size,
            entry_count,
            self.block_size,
            self.flags
        ) = struct.unpack('>I4sIIIII', self.data[4:32])
        if compression != b'zlib' or entry_size != 30:
            raise ValueError(f"Nicht unterstütztes PSARC-Format: {compression}, entry={entry_size}")
        self.entries: list[Entry] = []
        pos = 32
        for _ in range(entry_count):
            digest = self.data[pos:pos+16]
            block_idx = struct.unpack('>I', self.data[pos+16:pos+20])[0]
            length = int.from_bytes(self.data[pos+20:pos+25], 'big')
            offset = int.from_bytes(self.data[pos+25:pos+30], 'big')
            self.entries.append(Entry(digest, block_idx, length, offset))
            pos += 30
        width = max(1, ((self.block_size - 1).bit_length() + 7) // 8)
        self.zlengths = [
            int.from_bytes(self.data[i:i+width], 'big')
            for i in range(pos, self.toc_size, width)
        ]

    def read_entry(self, entry: Entry) -> bytes:
        remaining, offset, block_index, out = entry.length, entry.offset, entry.block_index, bytearray()
        while remaining:
            expected = min(self.block_size, remaining)
            packed = self.zlengths[block_index]
            stored = packed or expected
            chunk = self.data[offset:offset+stored]
            if packed and packed != expected:
                try:
                    chunk = zlib.decompress(chunk)
                except zlib.error:
                    pass
            out.extend(chunk)
            remaining -= expected
            offset += stored
            block_index += 1
        return bytes(out)

    def names(self) -> list[str]:
        manifest = self.read_entry(self.entries[0]).decode('utf-8', 'replace')
        return ['__manifest__', *[line.strip('\r') for line in manifest.splitlines() if line.strip('\r')]]

def repack(arc: Psarc, member_index: int, replacement: bytes) -> bytes:
    blocks, lengths, records = [], [], []
    for i, entry in enumerate(arc.entries):
        count = (entry.length + arc.block_size - 1) // arc.block_size
        begin = len(lengths)
        if i == member_index:
            assert len(replacement) == entry.length, "Ersatzinhalt muss exakt die gleiche unkomprimierte Länge haben"
            for p in range(0, len(replacement), arc.block_size):
                raw = replacement[p:p+arc.block_size]
                compressed = zlib.compress(raw, 6)
                stored = compressed if len(compressed) < len(raw) else raw
                blocks.append(stored)
                lengths.append(len(stored) % arc.block_size)
        else:
            p, rem = entry.offset, entry.length
            for k in range(count):
                length = arc.zlengths[entry.block_index + k]
                size = length or min(arc.block_size, rem)
                blocks.append(arc.data[p:p+size])
                lengths.append(length)
                p += size
                rem -= min(rem, arc.block_size)
        records.append((entry, begin, count))
    width = max(1, ((arc.block_size - 1).bit_length() + 7) // 8)
    toc = 32 + 30 * len(records) + width * len(lengths)
    assert toc == arc.toc_size, f"TOC-Größen-Konflikt: {toc} vs {arc.toc_size}"
    header = b'PSAR' + struct.pack('>I4sIIIII', arc.version, b'zlib', toc, 30, len(records), arc.block_size, arc.flags)
    table, offset = [], toc
    for entry, begin, count in records:
        table.append(entry.digest + struct.pack('>I', begin) + entry.length.to_bytes(5, 'big') + offset.to_bytes(5, 'big'))
        offset += sum(len(x) for x in blocks[begin:begin+count])
    return header + b''.join(table) + b''.join(x.to_bytes(width, 'big') for x in lengths) + b''.join(blocks)

def patch_archive(clean_path: Path, target_path: Path, delta: dict, expected_hash: str) -> str:
    print(f"\n[PSARC] Patching: {clean_path} -> {target_path}")
    arc = Psarc(clean_path)
    idx = arc.names().index('times.ngp')
    b = bytearray(arc.read_entry(arc.entries[idx]))
    
    for pos_str, hx in delta.items():
        pos = int(pos_str)
        b[pos:pos+4] = bytes.fromhex(hx)
        
    repacked = repack(arc, idx, bytes(b))
    target_path.parent.mkdir(parents=True, exist_ok=True)
    target_path.write_bytes(repacked)
    
    actual_hash = hashlib.sha256(repacked).hexdigest().upper()
    print(f"  SHA256: {actual_hash}")
    assert actual_hash == expected_hash, f"Hash-Mismatch! Erwartet {expected_hash}, erhalten {actual_hash}"
    print(f"  Verifikation: PASS (100% Bit-Exakt!)")
    return actual_hash

def patch_ltn(clean_path: Path, target_path: Path) -> None:
    print(f"\n[LTN] Patching: {clean_path} -> {target_path}")
    old = clean_path.read_bytes()
    new, count = re.subn(rb'(?m)^1([ \t]+// Distance Blur Enable)', rb'0\1', old)
    assert count == 1, f"Distance Blur Enable in {clean_path.name} nicht gefunden!"
    target_path.parent.mkdir(parents=True, exist_ok=True)
    target_path.write_bytes(new + b'\n')
    print(f"  Distance Blur von 1 auf 0 umgestellt (PASS)")

def main():
    parser = argparse.ArgumentParser(description="Wendet 5000m Sichtweite- und Distance-Blur-Fixes auf Twisted Metal PSARCs und LTNs an.")
    parser.add_argument("--disc-dir", type=Path, default=None, help="Pfad zum USRDIR-Verzeichnis der sauberen Disc/ISO")
    parser.add_argument("--target-dir", type=Path, default=None, help="Pfad zum Ziel-USRDIR der RPCS3-Installation (dev_hdd0/game/BCUS98106/USRDIR)")
    args = parser.parse_args()

    script_dir = Path(__file__).resolve().parent
    delta_file = script_dir / 'times_ngp_deltas.json'
    if not delta_file.exists():
        raise FileNotFoundError(f"Delta-Definitionsdatei nicht gefunden: {delta_file}")
    deltas = json.loads(delta_file.read_text(encoding='utf-8'))

    # Automatische Pfadermittlung falls keine CLI-Argumente übergeben wurden
    root_ws = Path(r'C:\Users\box3\Documents\ChatGPT\ps3')
    disc_base = args.disc_dir or (root_ws / 'ps3recomp-experiment/twistedmetal/disc/PS3_GAME/USRDIR')
    target_base = args.target_dir or (root_ws / 'rpcs3-live-4k/build-msvc2026/bin-twisted-metal-4k-anleitungs-nachbau/dev_hdd0/game/BCUS98106/USRDIR')

    print("=" * 65)
    print("TWISTED METAL (2012) - AUTARKER 4K ASSET-PATCHER")
    print(f"Disc-Quelle: {disc_base}")
    print(f"Ziel-Ordner: {target_base}")
    print("=" * 65)

    # 1. SP times.psarc (5000m Bäume & Laternen)
    sp_clean = disc_base / 'levels/times.psarc'
    sp_target = target_base / 'levels/times.psarc'
    sp_hash = "9129889C700E23CB61569EA90722E0E8F0A64B00CB2D4F4750BAA51DD407A22C"
    patch_archive(sp_clean, sp_target, deltas['sp_delta'], sp_hash)

    # 2. MP times.psarc (5000m Bäume & Laternen)
    mp_clean = disc_base / 'mplevels/times.psarc'
    mp_target = target_base / 'mplevels/times.psarc'
    mp_hash = "757C01A688C74918AB9A1CBBBE59BF1D8787D0C5BA44391059E798E96159E90A"
    patch_archive(mp_clean, mp_target, deltas['mp_delta'], mp_hash)

    # 3. times.ltn (Distance Blur deaktivieren)
    patch_ltn(disc_base / 'levels/times.ltn', target_base / 'levels/times.ltn')

    # 4. suburbs.ltn (Distance Blur deaktivieren)
    patch_ltn(disc_base / 'levels/suburbs.ltn', target_base / 'levels/suburbs.ltn')

    print("\n" + "=" * 65)
    print("ERFOLG: Alle 4 Level-Assets (2x PSARC, 2x LTN) vollständig bereitgestellt!")
    print("=" * 65)

if __name__ == '__main__':
    main()
