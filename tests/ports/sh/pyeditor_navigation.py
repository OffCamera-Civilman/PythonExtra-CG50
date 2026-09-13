"""Host-side regression checks for PythonUltra editor navigation preparation."""

from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
EDITOR = ROOT / "ports" / "sh" / "modules" / "pyeditor" / "__init__.py"
text = EDITOR.read_text(encoding="utf-8")

required = (
    "EDITOR_NAV_VERSION = 1",
    'self.popup("SHIFT VARS"',
    '"Editor Style", "Jump to Top", "Jump to Bottom"',
    '"Jump to Line #", "Cancel"',
    "def jump_top(self):",
    "def jump_bottom(self):",
    "def jump_line(self):",
    "def vars_menu(self):",
    "FAST_REPEAT_DELAY = 0.18",
    "FAST_REPEAT_INTERVAL = 0.03",
    "def read(self, fast_repeat=False):",
    "self._keys.read(fast_repeat=True)",
)

missing = [item for item in required if item not in text]
if missing:
    raise SystemExit("pyeditor navigation regression: missing " + repr(missing))

# Popup/input helpers should keep the default conservative repeat path.
if text.count("self._keys.read()") < 2:
    raise SystemExit("pyeditor navigation regression: popup/input repeat changed")

print("pyeditor navigation regression: ok")
