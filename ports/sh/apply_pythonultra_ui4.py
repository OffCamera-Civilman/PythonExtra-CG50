"""PythonUltra UI4: wrapped file viewer, terminal handoff, and turbo editor navigation."""

from pathlib import Path

ROOT = Path(__file__).resolve().parent
EDITOR = ROOT / "modules" / "pyeditor" / "__init__.py"
FILES = ROOT / "modules" / "pyfiles" / "__init__.py"
MARKER = "PYTHONULTRA_UI4_VERSION = 1"


def replace_once(text, old, new, label):
    if new in text:
        return text
    if old not in text:
        raise SystemExit("UI4 patch could not locate " + label)
    return text.replace(old, new, 1)


def patch_editor():
    text = EDITOR.read_text(encoding="utf-8")
    if MARKER in text:
        print("PythonUltra UI4 editor already applied")
        return

    start = text.index("class _KeyReader:")
    end = text.index("\ndef _maps(g):", start)
    reader = '''class _KeyReader:
    """Event reader with RC-style automatic turbo acceleration for navigation."""
    TURBO_DELAY_MS = 120
    TURBO_INTERVAL_MS = 18
    TURBO_STAGE2_MS = 420
    TURBO_STAGE3_MS = 900

    def __init__(self, g):
        self.g = g
        self.pending = []
        self.repeat_keys = (g.KEY_UP, g.KEY_DOWN, g.KEY_LEFT, g.KEY_RIGHT, g.KEY_DEL)
        self.held_key = None
        self.held_since = 0
        self.last_repeat = 0

    def _ticks(self):
        return time.ticks_ms()

    def _diff(self, newer, older):
        return time.ticks_diff(newer, older)

    def repeat_step(self):
        if self.held_key is None:
            return 1
        age = self._diff(self._ticks(), self.held_since)
        if age >= self.TURBO_STAGE3_MS:
            return 6
        if age >= self.TURBO_STAGE2_MS:
            return 3
        return 1

    def read(self, fast_repeat=False):
        g = self.g
        while True:
            hold_event = None
            now = self._ticks()
            while True:
                ev = g.pollevent()
                if ev.type == g.KEYEV_NONE:
                    break
                if ev.type == g.KEYEV_DOWN:
                    self.pending.append(ev.key)
                    if ev.key in self.repeat_keys:
                        self.held_key = ev.key
                        self.held_since = now
                        self.last_repeat = now
                elif ev.type == g.KEYEV_HOLD and ev.key in self.repeat_keys:
                    hold_event = ev.key
                    if self.held_key != ev.key:
                        self.held_key = ev.key
                        self.held_since = now
                        self.last_repeat = now
                elif ev.type == g.KEYEV_UP and ev.key == self.held_key:
                    self.held_key = None
                    self.held_since = 0
                    self.last_repeat = 0
            if self.pending:
                return self.pending.pop(0)

            held = self.held_key
            if held is not None and not g.keydown(held):
                self.held_key = None
                held = None

            if fast_repeat and held is not None:
                now = self._ticks()
                if (self._diff(now, self.held_since) >= self.TURBO_DELAY_MS
                        and self._diff(now, self.last_repeat) >= self.TURBO_INTERVAL_MS):
                    self.last_repeat = now
                    return held
            elif hold_event is not None and g.keydown(hold_event):
                return hold_event

            time.sleep_ms(2 if fast_repeat else 10)
'''
    text = text[:start] + reader + text[end:]

    text = replace_once(text,
        "EDITOR_NAV_VERSION = 1\n",
        "EDITOR_NAV_VERSION = 1\n" + MARKER + "\n",
        "UI4 marker")

    text = replace_once(text,
        "        self.shift_active = False\n        self.clipboard = \"\"\n",
        "        self.shift_active = False\n        self.turbo_mode = True\n        self.clipboard = \"\"\n",
        "turbo state")

    old_vars = '''    def vars_menu(self):
        action = self.popup("SHIFT VARS", (
            "Editor Style", "Jump to Top", "Jump to Bottom",
            "Jump to Line #", "Cancel"))
        if action == "Editor Style":
            self.style_menu()
        elif action == "Jump to Top":
            self.jump_top()
        elif action == "Jump to Bottom":
            self.jump_bottom()
        elif action == "Jump to Line #":
            self.jump_line()
'''
    new_vars = '''    def vars_menu(self):
        turbo_label = "Turbo: ON" if self.turbo_mode else "Turbo: OFF"
        action = self.popup("SHIFT VARS", (
            "Editor Style", turbo_label, "Jump to Top", "Jump to Bottom",
            "Jump to Line #", "Cancel"))
        if action == "Editor Style":
            self.style_menu()
        elif action == turbo_label:
            self.turbo_mode = not self.turbo_mode
            self.msg = "Turbo ON" if self.turbo_mode else "Turbo OFF"
        elif action == "Jump to Top":
            self.jump_top()
        elif action == "Jump to Bottom":
            self.jump_bottom()
        elif action == "Jump to Line #":
            self.jump_line()
'''
    text = replace_once(text, old_vars, new_vars, "SHIFT VARS turbo")

    old_move = '''                if key == g.KEY_UP:
                    self.move(-1, 0)
                elif key == g.KEY_DOWN:
                    self.move(1, 0)
                elif key == g.KEY_LEFT:
                    self.move(0, -1)
                elif key == g.KEY_RIGHT:
                    self.move(0, 1)
'''
    new_move = '''                nav_step = self._keys.repeat_step() if self.turbo_mode else 1
                if key == g.KEY_UP:
                    self.move(-nav_step, 0)
                elif key == g.KEY_DOWN:
                    self.move(nav_step, 0)
                elif key == g.KEY_LEFT:
                    self.move(0, -nav_step)
                elif key == g.KEY_RIGHT:
                    self.move(0, nav_step)
'''
    text = replace_once(text, old_move, new_move, "turbo movement")

    EDITOR.write_text(text, encoding="utf-8")
    print("PythonUltra UI4 editor: turbo navigation applied")


def patch_files():
    text = FILES.read_text(encoding="utf-8")
    if MARKER in text:
        print("PythonUltra UI4 files already applied")
        return

    text = replace_once(text,
        '__version__ = "0.7.0-cg50"\n',
        '__version__ = "0.7.1-cg50"\n' + MARKER + '\n',
        "Files UI4 marker")

    helper_anchor = '''def _modified_text(value):
'''
    helper = '''def _wrap_view_lines(text, width=46):
    """Wrap logical text lines for the calculator viewer without losing text."""
    wrapped = []
    for logical in str(text).replace("\\r\\n", "\\n").replace("\\r", "\\n").split("\\n"):
        if logical == "":
            wrapped.append("")
            continue
        remaining = logical
        while len(remaining) > width:
            cut = remaining.rfind(" ", 0, width + 1)
            if cut <= 0:
                cut = width
                wrapped.append(remaining[:cut])
                remaining = remaining[cut:]
            else:
                wrapped.append(remaining[:cut])
                remaining = remaining[cut + 1:]
        wrapped.append(remaining)
    return wrapped or [""]


'''
    if helper not in text:
        text = text.replace(helper_anchor, helper + helper_anchor, 1)

    text = replace_once(text,
        '        lines = text.split("\\n") if text else [""]\n',
        '        lines = _wrap_view_lines(text, 46)\n',
        "viewer wrapping")
    text = replace_once(text,
        '                g.dtext(4, HEADER_H + row * 11, p[1], lines[idx][:48])\n',
        '                g.dtext(4, HEADER_H + row * 11, p[1], lines[idx])\n',
        "viewer clipping")

    text = replace_once(text,
        '''            elif key == g.KEY_F2:
                self.edit_file(path); return
''',
        '''            elif key == g.KEY_F2:
                return self.edit_file(path)
''',
        "file-info edit return")
    text = replace_once(text,
        '''            elif key == g.KEY_F4:
                if lower.endswith(".py"): self.run_file(path); return
''',
        '''            elif key == g.KEY_F4:
                if lower.endswith(".py"): return self.run_file(path)
''',
        "file-info run return")
    text = replace_once(text,
        '''        self.file_info(path)

    def run_file(self, path=None):
''',
        '''        return self.file_info(path)

    def run_file(self, path=None):
''',
        "enter selected return")
    text = replace_once(text,
        '''            exec(code, scope, scope); self.msg = "Run OK"
''',
        '''            exec(code, scope, scope); self.msg = "Run OK"
            return "terminal"
''',
        "run file terminal return")
    text = replace_once(text,
        '''            self.pyeditor.open_file(path, self.theme_name)
            self.refresh()
''',
        '''            result = self.pyeditor.open_file(path, self.theme_name)
            self.refresh()
            if result == "run":
                return "terminal"
''',
        "editor run terminal return")
    text = replace_once(text,
        '''        elif choice == "Run file": self.run_file()
        elif choice == "Edit file": self.edit_file()
''',
        '''        elif choice == "Run file":
            if self.run_file() == "terminal": return "terminal"
        elif choice == "Edit file":
            if self.edit_file() == "terminal": return "terminal"
''',
        "more menu terminal return")
    text = replace_once(text,
        '''            elif key in (g.KEY_EXE, g.KEY_RIGHT): self.enter_selected()
''',
        '''            elif key in (g.KEY_EXE, g.KEY_RIGHT):
                if self.enter_selected() == "terminal": return "terminal"
''',
        "browser enter terminal return")
    text = replace_once(text,
        '''            elif key == g.KEY_F6:
                if self.more_menu() == "exit": return
            elif key == g.KEY_OPTN:
                if self.more_menu() == "exit": return
''',
        '''            elif key == g.KEY_F6:
                action = self.more_menu()
                if action in ("exit", "terminal"): return action
            elif key == g.KEY_OPTN:
                action = self.more_menu()
                if action in ("exit", "terminal"): return action
''',
        "browser menu terminal return")

    FILES.write_text(text, encoding="utf-8")
    print("PythonUltra UI4 files: wrapping and terminal handoff applied")


def main():
    patch_editor()
    patch_files()


if __name__ == "__main__":
    main()
