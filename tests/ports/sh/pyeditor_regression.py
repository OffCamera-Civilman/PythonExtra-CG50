"""Host regressions for PythonUltra's calculator editor state and layout."""

import builtins
import importlib.util
from pathlib import Path
import sys
import tempfile
import types

ROOT = Path(__file__).resolve().parents[3]
EDITOR_PATH = ROOT / "ports" / "sh" / "modules" / "pyeditor" / "__init__.py"
PYFILES_PATH = ROOT / "ports" / "sh" / "modules" / "pyfiles" / "__init__.py"


class FakePermissions(types.ModuleType):
    def __init__(self):
        super().__init__("pyperm")
        self.allow_read = True
        self.allow_write = True

    def require_read(self, path):
        if not self.allow_read:
            raise OSError("Permission denied (read): " + str(path))
        return True

    def require_write(self, path):
        if not self.allow_write:
            raise OSError("Permission denied (write): " + str(path))
        return True

    def writable(self, path):
        return self.allow_write


class Event:
    def __init__(self, event_type, key):
        self.type = event_type
        self.key = key


class FakeGint(types.ModuleType):
    def __init__(self):
        super().__init__("gint")
        names = (
            "KEY_0", "KEY_1", "KEY_2", "KEY_3", "KEY_4", "KEY_5",
            "KEY_6", "KEY_7", "KEY_8", "KEY_9", "KEY_DOT", "KEY_ADD",
            "KEY_SUB", "KEY_MUL", "KEY_DIV", "KEY_LEFTPAR",
            "KEY_RIGHTPAR", "KEY_COMMA", "KEY_NEG", "KEY_EQUALS",
            "KEY_XOT", "KEY_LOG", "KEY_LN", "KEY_SIN", "KEY_COS",
            "KEY_TAN", "KEY_FRAC", "KEY_FD", "KEY_ARROW", "KEY_EXP",
            "KEY_VARS", "KEY_EXIT", "KEY_EXE", "KEY_SHIFT", "KEY_ALPHA",
            "KEY_DEL", "KEY_UP", "KEY_DOWN", "KEY_LEFT", "KEY_RIGHT",
            "KEY_F1", "KEY_F2", "KEY_F3", "KEY_F4", "KEY_F5", "KEY_F6",
            "KEY_OPTN",
        )
        for value, name in enumerate(names, 10):
            setattr(self, name, value)
        self.KEYEV_DOWN = 1
        self.KEYEV_HOLD = 2
        self.events = []
        self.current_font = "small"
        self.drawn_text = []
        self.font_calls = []
        self.default_font_restores = 0
        self.clear_count = 0

    def dfont_builtin(self, name):
        self.current_font = name
        self.font_calls.append(name)
        return {
            "small": (9, 10),
            "normal": (12, 17),
            "large": (18, 23),
        }[name]

    def dfont(self, font):
        if font is None:
            self.default_font_restores += 1

    def dsize(self, text):
        scale, height = {
            "small": (1.0, 10),
            "normal": (1.6, 17),
            "large": (2.2, 23),
        }[self.current_font]
        narrow = " ilI.,:;'!|"
        wide = "mwMW@%"
        width = 0
        for char in str(text):
            base = 3 if char in narrow else (8 if char in wide else 6)
            width += max(1, int(base * scale))
        # Like gint, include spacing between glyphs but not after the last.
        return width + max(0, len(text) - 1), height

    def dtext(self, x, y, color, text):
        self.drawn_text.append((self.current_font, x, y, str(text)))

    def dclear(self, color):
        pass

    def drect(self, *args):
        pass

    def drect_border(self, *args):
        pass

    def dupdate(self):
        pass

    def clearevents(self):
        self.clear_count += 1

    def pollevent(self):
        if not self.events:
            raise AssertionError("test exhausted fake key events")
        return self.events.pop(0)


fake_permissions = FakePermissions()
fake_gint = FakeGint()
sys.modules["pyperm"] = fake_permissions
sys.modules["gint"] = fake_gint
ui_spec = importlib.util.spec_from_file_location(
    "pythonultra", ROOT / "ports/sh/modules/pythonultra/__init__.py")
ui = importlib.util.module_from_spec(ui_spec)
ui_spec.loader.exec_module(ui)
ui.menu_border = lambda: 0x07FF
sys.modules["pythonultra"] = ui

spec = importlib.util.spec_from_file_location("pythonultra_pyeditor_test", EDITOR_PATH)
editor_module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(editor_module)


with tempfile.TemporaryDirectory() as temp_root:
    root = Path(temp_root)
    missing = root / "new.py"
    editor = editor_module.Editor(str(missing))
    assert editor.msg == "New"
    assert editor.font_name == "System Small"
    assert "JetBrains" not in " ".join(editor_module.FONT_OPTIONS)

    # Catalog inserts qualified text at the cursor, not an executable command.
    choices = iter(("numpy", "array"))
    real_popup = editor.popup
    editor.popup = lambda title, items: next(choices)
    editor.lines = ["x = ()"]
    editor.cx = 5
    editor.catalog_menu()
    assert editor.lines == ["x = (numpy.array)"]
    editor.popup = real_popup
    editor.cx = 0

    # Proportional text placement must use measured widths, not max cell width.
    editor.lines = ["import ctypes"]
    editor.apply_font("System Normal")
    fake_gint.drawn_text.clear()
    editor.draw_line(editor.lines[0], 0, 0)
    ctypes_draw = next(item for item in fake_gint.drawn_text if item[3] == "ctypes")
    expected_x = editor_module.TEXT_X + editor._text_advance("import ")
    assert ctypes_draw[1] == expected_x
    assert ctypes_draw[1] != editor_module.TEXT_X + 7 * editor_module.FONT_W

    # Normal and large body fonts keep all status/softkey chrome on small font.
    for choice, builtin in (("System Normal", "normal"), ("System Large", "large")):
        editor.apply_font(choice)
        fake_gint.drawn_text.clear()
        editor.draw()
        body = [item for item in fake_gint.drawn_text if item[2] < editor_module.BODY_H]
        chrome = [item for item in fake_gint.drawn_text if item[2] >= editor_module.BODY_H]
        assert any(item[0] == builtin for item in body)
        assert chrome and all(item[0] == "small" for item in chrome)
        for font, x, y, text in chrome:
            if text.startswith("F") and len(text) > 1 and text[1].isdigit():
                slot = int(text[1]) - 1
                assert slot * 66 <= x
                fake_gint.current_font = font
                assert x + fake_gint.dsize(text)[0] <= min(395, slot * 66 + 65)

    # Changing font must not jump a valid viewport back to the beginning.
    editor.lines = ["abcdefghij"] * 12
    editor.cy, editor.cx = 4, 6
    editor.goal_x = 6
    editor.scroll_y, editor.scroll_x = 2, 2
    editor.apply_font("System Normal")
    assert (editor.scroll_y, editor.scroll_x) == (2, 2)

    # Multiline paste must preserve the line-array invariant and trailing text.
    editor.lines = ["leftRIGHT"]
    editor.cy, editor.cx = 0, 4
    editor.insert("A\nB\nC")
    assert editor.lines == ["leftA", "B", "CRIGHT"]
    assert (editor.cy, editor.cx) == (2, 1)

    # A failed/oversized open must preserve the current path and buffer.
    editor.filename = str(missing)
    editor.lines = ["keep", "this"]
    oversized = root / "huge.py"
    oversized.write_bytes(b"x" * (editor_module.MAX_EDIT_BYTES + 1))
    assert editor.load_file(str(oversized)) is False
    assert editor.filename == str(missing)
    assert editor.lines == ["keep", "this"]
    protected = root / "protected.py"
    protected.write_text("print(1)\n", encoding="utf-8")
    fake_permissions.allow_read = False
    assert editor.load_file(str(protected)) is False
    assert editor.lines == ["keep", "this"]
    fake_permissions.allow_read = True

    absent = root / "does-not-exist.py"
    assert editor.load_file(str(absent)) is False
    assert editor.lines == ["keep", "this"]
    assert editor.filename == str(missing)
    failed_editor = editor_module.Editor(str(oversized))
    assert failed_editor.load_failed
    assert failed_editor.save_file() is False
    assert oversized.stat().st_size == editor_module.MAX_EDIT_BYTES + 1

    # Save stages output; failures never truncate the original source.
    save_path = root / "safe.py"
    save_path.write_text("original", encoding="utf-8")
    save_editor = editor_module.Editor(str(save_path))
    save_editor.lines = ["changed", "line two"]
    save_editor.dirty = True
    real_rename = editor_module.os.rename
    def fail_publish(src, dst):
        if str(src).endswith(".pu-tmp"):
            raise OSError("simulated storage error")
        return real_rename(src, dst)
    editor_module.os.rename = fail_publish
    try:
        assert save_editor.save_file() is False
    finally:
        editor_module.os.rename = real_rename
    assert save_path.read_text(encoding="utf-8") == "original"
    assert save_editor.dirty
    assert save_editor.save_file()
    assert save_path.read_text(encoding="utf-8") == "changed\nline two"
    assert not list(root.glob("*.pu-tmp"))
    assert not list(root.glob("*.pu-backup"))

    # Popups and text prompts use raw events and preserve the viewport.
    editor.cy, editor.cx = 1, 2
    editor.scroll_y = 1
    fake_gint.events[:] = [Event(fake_gint.KEYEV_DOWN, fake_gint.KEY_EXE)]
    assert editor.popup("Test", ("OK",)) == "OK"
    assert editor.scroll_y == 1
    fake_gint.events[:] = [Event(fake_gint.KEYEV_DOWN, key) for key in (
        fake_gint.KEY_ALPHA, fake_gint.KEY_XOT, fake_gint.KEY_SHIFT,
        fake_gint.KEY_ALPHA, fake_gint.KEY_LOG, fake_gint.KEY_ALPHA,
        fake_gint.KEY_SHIFT, fake_gint.KEY_ADD, fake_gint.KEY_EXE)]
    assert editor.input_bar("Test") == "aB["

    # New-file naming must never select an existing file for truncation.
    existing = root / "new.py"
    existing.write_text("do not replace", encoding="utf-8")
    unused = editor_module._unused_filename(str(existing))
    assert unused.endswith("new_1.py")
    assert Path(unused) != existing
    assert editor.start_new(str(existing)) is False
    assert existing.read_text(encoding="utf-8") == "do not replace"

    # Editor Run executes once and reports exceptions as failure.
    runnable = root / "once.py"
    runnable.write_text(
        "import builtins\nbuiltins._pythonultra_editor_runs += 1\n",
        encoding="utf-8",
    )
    builtins._pythonultra_editor_runs = 0
    run_editor = editor_module.Editor(str(runnable))
    assert run_editor.run_code() is True
    assert builtins._pythonultra_editor_runs == 1
    failing = root / "fail.py"
    failing.write_text("raise ValueError('expected')\n", encoding="utf-8")
    assert editor_module.Editor(str(failing)).run_code() is False
    del builtins._pythonultra_editor_runs

    # Every exit path clears the handoff queue and restores gint's default font.
    exit_editor = editor_module.Editor(str(missing))
    fake_gint.events[:] = [Event(fake_gint.KEYEV_DOWN, fake_gint.KEY_EXIT)]
    before_restores = fake_gint.default_font_restores
    assert exit_editor.run() == "exit"
    assert fake_gint.clear_count > 0
    assert fake_gint.default_font_restores == before_restores + 1

# Files must not execute an editor-run script for a second time.
pyfiles_source = PYFILES_PATH.read_text(encoding="utf-8")
assert 'result == "run"' not in pyfiles_source

print("PythonUltra editor regression checks passed")
