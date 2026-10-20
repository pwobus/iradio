"""Host-side checks for the factory FNK0104B wiring and conflicting overrides."""
from pathlib import Path
import shutil
import subprocess
import unittest


@unittest.skipUnless(shutil.which("g++"), "g++ is required for pin configuration checks")
class FreenovePinsTest(unittest.TestCase):
    def compile(self, source, *defines):
        return subprocess.run(
            ["g++", "-std=c++17", "-x", "c++", "-fsyntax-only",
             "-I" + str(Path(__file__).resolve().parents[1] / "include"),
             *defines, "-"], input=source, text=True, capture_output=True,
            check=False,
        )

    def test_factory_wiring(self):
        result = self.compile('''
#include "FreenovePins.h"
static_assert(FN_TFT_MOSI == 11 && FN_TFT_MISO == 13);
static_assert(FN_TFT_SCLK == 12 && FN_TFT_CS == 10);
static_assert(FN_TFT_DC == 46 && FN_TFT_BL == 45);
static_assert(FN_TOUCH_SDA == 16 && FN_TOUCH_SCL == 15);
static_assert(FN_TOUCH_INT == 17 && FN_TOUCH_RST == 18);
static_assert(FN_I2S_MCLK == 4 && FN_I2S_BCLK == 5);
static_assert(FN_I2S_WS == 7 && FN_I2S_DOUT == 8);
static_assert(FN_AUDIO_ENABLE == 1);
''')
        self.assertEqual(result.returncode, 0, result.stderr)

    def test_supplied_spi_wiring_conflicts_with_factory_touch(self):
        result = self.compile('#include "FreenovePins.h"',
                              "-DFN_TFT_MOSI=13", "-DFN_TFT_MISO=12",
                              "-DFN_TFT_SCLK=14", "-DFN_TFT_CS=15")
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("FNK0104B pin conflict", result.stderr)

    def test_display_cannot_reuse_audio_output_pin(self):
        result = self.compile('#include "FreenovePins.h"', "-DFN_TFT_CS=8")
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("FNK0104B pin conflict", result.stderr)


if __name__ == "__main__":
    unittest.main()
