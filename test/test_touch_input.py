"""Host checks for nonblocking contact handling used by the radio loop."""
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest


@unittest.skipUnless(shutil.which("g++"), "g++ is required")
class TouchInputTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.directory = tempfile.TemporaryDirectory()
        cls.executable = str(Path(cls.directory.name) / "touch_input")
        source = r'''
#include "TouchInput.h"
#include <cassert>
int main(int argc, char** argv) {
  assert(argc == 2);
  TouchInput touch;
  using Event = TouchInput::Event;
  switch (argv[1][0]) {
  case 'h':
    assert(touch.update(true, 30, 40) == Event::press);
    for (int i = 0; i < 1000; ++i)
      assert(touch.update(true, 30, 40) == Event::none);
    assert(touch.update(true, 31, 40) == Event::drag);
    assert(touch.update(true, 31, 40) == Event::none);
    assert(touch.update(false, 0, 0) == Event::none);
    assert(touch.update(true, 31, 40) == Event::press);
    break;
  case 's': {
    int samples = 0;
    for (uint32_t now = 0; now <= 100; ++now) {
      if (touch.sampleReady(now)) ++samples;
    }
    assert(samples == 5);
    break;
  }
  case 'w':
    assert(touch.sampleReady(0xfffffff0u));
    assert(!touch.sampleReady(0xfffffffau));
    assert(touch.sampleReady(4u));
    assert(!touch.sampleReady(5u));
    assert(touch.sampleReady(24u));
    break;
  default: return 1;
  }
}
'''
        result = subprocess.run(
            ["g++", "-std=c++17", "-x", "c++", "-I" + str(
                Path(__file__).resolve().parents[1] / "include"),
             "-o", cls.executable, "-"],
            input=source, capture_output=True, text=True, check=False,
        )
        if result.returncode:
            cls.directory.cleanup()
            raise RuntimeError(result.stderr)

    @classmethod
    def tearDownClass(cls):
        cls.directory.cleanup()

    def run_case(self, case):
        result = subprocess.run([self.executable, case], capture_output=True,
                                text=True, cwd=self.directory.name, check=False)
        self.assertEqual(result.returncode, 0, result.stderr)

    def test_held_touch_drag_and_release(self):
        self.run_case("held")

    def test_touch_sample_interval(self):
        self.run_case("sampling")

    def test_millis_rollover(self):
        self.run_case("wrap")


if __name__ == "__main__":
    unittest.main()
