"""Regression checks for PythonUltra UI4 calculator usability fixes."""

from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
EDITOR = ROOT / "ports" / "sh" / "modules" / "pyeditor" / "__init__.py"
FILES = ROOT / "ports" / "sh" / "modules" / "pyfiles" / "__init__.py"

editor = EDITOR.read_text(encoding="utf-8")
files = FILES.read_text(encoding="utf-8")

editor_required = (
    "PYTHONULTRA_UI4_VERSION = 1",
    "TURBO_DELAY_MS = 120",
    "TURBO_INTERVAL_MS = 18",
    "TURBO_STAGE2_MS = 420",
    "TURBO_STAGE3_MS = 900",
    "time.ticks_ms()",
    "time.ticks_diff(newer, older)",
    "def repeat_step(self):",
    "return 6",
    "return 3",
    "self.turbo_mode = True",
    '"Turbo: ON" if self.turbo_mode else "Turbo: OFF"',
    "nav_step = self._keys.repeat_step() if self.turbo_mode else 1",
)
files_required = (
    "PYTHONULTRA_UI4_VERSION = 1",
    "def _wrap_view_lines(text, width=46):",
    "lines = _wrap_view_lines(text, 46)",
    'return "terminal"',
    'if result == "run":',
    'if self.enter_selected() == "terminal": return "terminal"',
)

missing = [item for item in editor_required if item not in editor]
if missing:
    raise SystemExit("UI4 editor regression missing: " + repr(missing))
missing = [item for item in files_required if item not in files]
if missing:
    raise SystemExit("UI4 files regression missing: " + repr(missing))

# Viewer must no longer hard-clip each logical line at 48 characters.
if "lines[idx][:48]" in files:
    raise SystemExit("UI4 viewer regression: hard clipping remains")

print("UI4 regression: ok")
