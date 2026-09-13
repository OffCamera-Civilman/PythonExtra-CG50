'''PythonUltra editor navigation and fast-repeat patch.

Adds the SHIFT+VARS editor command menu (style/top/bottom/line) and gives the
main editor a faster software key repeat without changing popup/menu repeat.
The patch is intentionally idempotent so CI can prepare the source tree more
than once safely.
'''

from pathlib import Path

ROOT = Path(__file__).resolve().parent
EDITOR = ROOT / "modules" / "pyeditor" / "__init__.py"
MARKER = "EDITOR_NAV_VERSION = 1"


def replace_once(text, old, new, label):
    if new in text:
        return text, False
    if old not in text:
        raise SystemExit("Unable to locate editor navigation block: " + label)
    return text.replace(old, new, 1), True


def main():
    text = EDITOR.read_text(encoding="utf-8")
    if MARKER in text:
        print("PythonUltra editor navigation already applied")
        return

    changed = False

    old = '''EDITOR_FIXES_VERSION = 1\n'''
    new = '''EDITOR_FIXES_VERSION = 1\nEDITOR_NAV_VERSION = 1\n'''
    text, did = replace_once(text, old, new, "version marker")
    changed = changed or did

    old = '''class _KeyReader:\n    \"\"\"Drain releases before acting on repeats; preserve real presses in order.\"\"\"\n    def __init__(self, g):\n        self.g = g\n        self.pending = []\n        self.repeat_keys = (g.KEY_UP, g.KEY_DOWN, g.KEY_LEFT, g.KEY_RIGHT, g.KEY_DEL)\n\n    def read(self):\n        g = self.g\n        while True:\n            repeat = None\n            while True:\n                ev = g.pollevent()\n                if ev.type == g.KEYEV_NONE:\n                    break\n                if ev.type == g.KEYEV_DOWN:\n                    self.pending.append(ev.key)\n                    repeat = None\n                elif ev.type == g.KEYEV_HOLD and ev.key in self.repeat_keys:\n                    repeat = ev.key\n                elif ev.type == g.KEYEV_UP and ev.key == repeat:\n                    repeat = None\n            if self.pending:\n                return self.pending.pop(0)\n            # keydown() reflects processed events, so it is only current after\n            # draining the queue. Coalesce a backlog to at most one movement.\n            if repeat is not None and g.keydown(repeat):\n                return repeat\n            time.sleep(0.01)\n'''
    new = '''class _KeyReader:\n    \"\"\"Drain the event queue and optionally synthesize a fast editor repeat.\n\n    Popup/menu navigation keeps gint's conservative HOLD cadence. The editor\n    view asks for fast_repeat=True, which begins repeating after 180 ms and\n    then advances every 30 ms while the physical navigation key remains down.\n    \"\"\"\n    FAST_REPEAT_DELAY = 0.18\n    FAST_REPEAT_INTERVAL = 0.03\n\n    def __init__(self, g):\n        self.g = g\n        self.pending = []\n        self.repeat_keys = (g.KEY_UP, g.KEY_DOWN, g.KEY_LEFT, g.KEY_RIGHT, g.KEY_DEL)\n        self.held_key = None\n        self.repeat_at = 0.0\n\n    def read(self, fast_repeat=False):\n        g = self.g\n        while True:\n            hold_event = None\n            now = time.time()\n            while True:\n                ev = g.pollevent()\n                if ev.type == g.KEYEV_NONE:\n                    break\n                if ev.type == g.KEYEV_DOWN:\n                    self.pending.append(ev.key)\n                    if ev.key in self.repeat_keys:\n                        self.held_key = ev.key\n                        self.repeat_at = now + self.FAST_REPEAT_DELAY\n                elif ev.type == g.KEYEV_HOLD and ev.key in self.repeat_keys:\n                    self.held_key = ev.key\n                    hold_event = ev.key\n                    if self.repeat_at <= 0.0:\n                        self.repeat_at = now + self.FAST_REPEAT_DELAY\n                elif ev.type == g.KEYEV_UP and ev.key == self.held_key:\n                    self.held_key = None\n                    self.repeat_at = 0.0\n            if self.pending:\n                return self.pending.pop(0)\n\n            held = self.held_key\n            if held is not None and not g.keydown(held):\n                self.held_key = None\n                self.repeat_at = 0.0\n                held = None\n\n            if fast_repeat and held is not None:\n                now = time.time()\n                if now >= self.repeat_at:\n                    self.repeat_at = now + self.FAST_REPEAT_INTERVAL\n                    return held\n            elif hold_event is not None and g.keydown(hold_event):\n                return hold_event\n\n            time.sleep(0.005 if fast_repeat else 0.01)\n'''
    text, did = replace_once(text, old, new, "key reader")
    changed = changed or did

    old = '''    def style_menu(self):\n        action = self.popup(\"Editor Style\", (\"Syntax Theme\", \"Font / Size\", \"Cancel\"))\n        if action == \"Syntax Theme\":\n            choice = self.popup(\"Syntax Theme\", THEME_NAMES)\n            if choice:\n                self.set_theme(choice)\n        elif action == \"Font / Size\":\n            choice = self.popup(\"Editor Font\", FONT_OPTIONS)\n            if choice:\n                self.apply_font(choice)\n\n    def catalog_menu(self):\n'''
    new = '''    def style_menu(self):\n        action = self.popup(\"Editor Style\", (\"Syntax Theme\", \"Font / Size\", \"Cancel\"))\n        if action == \"Syntax Theme\":\n            choice = self.popup(\"Syntax Theme\", THEME_NAMES)\n            if choice:\n                self.set_theme(choice)\n        elif action == \"Font / Size\":\n            choice = self.popup(\"Editor Font\", FONT_OPTIONS)\n            if choice:\n                self.apply_font(choice)\n\n    def jump_top(self):\n        self.cy = 0\n        self.cx = 0\n        self.goal_x = 0\n        self.scroll_x = 0\n        self.scroll_y = 0\n        self.msg = \"Top\"\n        self._ensure_visible()\n\n    def jump_bottom(self):\n        self.cy = max(0, len(self.lines) - 1)\n        self.cx = len(self.lines[self.cy])\n        self.goal_x = self.cx\n        self.scroll_x = 0\n        self.scroll_y = max(0, self.cy - MAX_ROWS + 1)\n        self.msg = \"Bottom\"\n        self._ensure_visible()\n\n    def jump_line(self):\n        value = self.input_bar(\"Jump to line\", str(self.cy + 1))\n        if value is None:\n            return\n        try:\n            line_number = int(value)\n        except (TypeError, ValueError):\n            self.msg = \"Line number?\"\n            return\n        line_number = max(1, min(len(self.lines), line_number))\n        self.cy = line_number - 1\n        self.cx = 0\n        self.goal_x = 0\n        self.scroll_x = 0\n        self.msg = \"Line \" + str(line_number)\n        self._ensure_visible()\n\n    def vars_menu(self):\n        action = self.popup(\"SHIFT VARS\", (\n            \"Editor Style\", \"Jump to Top\", \"Jump to Bottom\",\n            \"Jump to Line #\", \"Cancel\"))\n        if action == \"Editor Style\":\n            self.style_menu()\n        elif action == \"Jump to Top\":\n            self.jump_top()\n        elif action == \"Jump to Bottom\":\n            self.jump_bottom()\n        elif action == \"Jump to Line #\":\n            self.jump_line()\n\n    def catalog_menu(self):\n'''
    text, did = replace_once(text, old, new, "SHIFT VARS commands")
    changed = changed or did

    # Both the input bar and editor loop had SHIFT+VARS wired directly to the
    # style dialog. Route both through the new command menu.
    old = '''                    self.style_menu()\n                    continue\n'''
    new = '''                    self.vars_menu()\n                    continue\n'''
    count = text.count(old)
    if count != 2:
        raise SystemExit("Expected two SHIFT+VARS style-menu routes, found %d" % count)
    text = text.replace(old, new, 2)
    changed = True

    old = '''                key = self._keys.read()\n                if key in (g.KEY_F1, g.KEY_F2, g.KEY_F3, g.KEY_F4, g.KEY_F5):\n'''
    new = '''                key = self._keys.read(fast_repeat=True)\n                if key in (g.KEY_F1, g.KEY_F2, g.KEY_F3, g.KEY_F4, g.KEY_F5):\n'''
    text, did = replace_once(text, old, new, "editor fast repeat")
    changed = changed or did

    if changed:
        EDITOR.write_text(text, encoding="utf-8")
    print("Applied PythonUltra editor navigation and fast repeat")


if __name__ == "__main__":
    main()
