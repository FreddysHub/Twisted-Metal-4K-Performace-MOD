"""Small, game-data-free regression tests. Run: python -m unittest discover -s tests."""
import importlib.util
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
SCRIPT = ROOT / "03_QUELLCODE" / "apply_times_ngp_delta.py"
spec = importlib.util.spec_from_file_location("tm4k_asset_patcher", SCRIPT)
patcher = importlib.util.module_from_spec(spec)
spec.loader.exec_module(patcher)


class AssetPatcherTests(unittest.TestCase):
    def test_explicit_paths_required(self):
        result = subprocess.run([sys.executable, str(SCRIPT)], capture_output=True)
        self.assertEqual(result.returncode, 2)
        self.assertIn(b"--disc-dir", result.stderr)
        self.assertIn(b"--target-dir", result.stderr)

    def test_ltn_preserves_all_other_bytes(self):
        with tempfile.TemporaryDirectory(prefix="tm4k-test-") as temp:
            source, target = Path(temp) / "clean.ltn", Path(temp) / "new.ltn"
            original = b"header\r\n1\t// Distance Blur Enable\r\n0 // other\r\n\x00"
            source.write_bytes(original)
            patcher.patch_ltn(source, target)
            expected = original.replace(b"1\t// Distance Blur Enable", b"0\t// Distance Blur Enable")
            self.assertEqual(target.read_bytes(), expected)
            self.assertEqual(source.read_bytes(), original)

    def test_ltn_refuses_overwrite(self):
        with tempfile.TemporaryDirectory(prefix="tm4k-test-") as temp:
            source, target = Path(temp) / "clean.ltn", Path(temp) / "new.ltn"
            source.write_bytes(b"1 // Distance Blur Enable\n")
            target.write_bytes(b"keep")
            with self.assertRaises(FileExistsError):
                patcher.patch_ltn(source, target)
            self.assertEqual(target.read_bytes(), b"keep")

    def test_ambiguous_ltn_rejected(self):
        with tempfile.TemporaryDirectory(prefix="tm4k-test-") as temp:
            source, target = Path(temp) / "clean.ltn", Path(temp) / "new.ltn"
            source.write_bytes(b"1 // Distance Blur Enable\n" * 2)
            with self.assertRaises(ValueError):
                patcher.patch_ltn(source, target)
            self.assertFalse(target.exists())

    def test_bad_clean_archive_writes_nothing(self):
        with tempfile.TemporaryDirectory(prefix="tm4k-test-") as temp:
            base, target = Path(temp) / "disc", Path(temp) / "output"
            (base / "levels").mkdir(parents=True)
            (base / "levels/times.psarc").write_bytes(b"not the matching clean game")
            result = subprocess.run(
                [sys.executable, str(SCRIPT), "--disc-dir", str(base), "--target-dir", str(target)],
                capture_output=True,
            )
            self.assertNotEqual(result.returncode, 0)
            self.assertIn(b"Clean input SHA256 mismatch", result.stderr)
            self.assertFalse(target.exists())

    def test_source_cannot_be_target(self):
        with tempfile.TemporaryDirectory(prefix="tm4k-test-") as temp:
            result = subprocess.run(
                [sys.executable, str(SCRIPT), "--disc-dir", temp, "--target-dir", temp],
                capture_output=True,
            )
            self.assertNotEqual(result.returncode, 0)
            self.assertIn(b"Target must not be inside", result.stderr)


if __name__ == "__main__":
    unittest.main()
