"""Integrated PythonUltra text editor for the fx-CG50.

The editor is based on the project's PyEditorRC prototype, refactored to be a
frozen module with safer file handling, curated PythonUltra integration and a
small-memory theme system.
"""

import gc
import sys

__version__ = "0.2.0-cg50"

SCREEN_W = 396
SCREEN_H = 224
NAV_H = 12
INFO_H = 12
FONT_W = 8
FONT_H = 11
TEXT_X = 2
MAX_ROWS = (SCREEN_H - NAV_H - INFO_H) // FONT_H
MAX_COLS = (SCREEN_W - TEXT_X) // FONT_W

THEMES = {
    "GitHub Dark": (0x1082, 0xD69A, 0x18E3, 0xD69A, 0x2148, 0xFFFF, 0xFF7B, 0x7D7C, 0x79FF, 0xA59D, 0x8C71, 0xD39F, 0xFFA6, 0x3186),
    "GitHub Light": (0xFFFF, 0x18E3, 0xF7BE, 0x18E3, 0xBDF7, 0x0000, 0xA00F, 0x06B9, 0x045F, 0x0863, 0x6B6D, 0x7A6D, 0x9A63, 0xD69A),
    "Linux Terminal": (0x0000, 0xC618, 0x0000, 0x07E0, 0x03E0, 0x0000, 0xFFE0, 0x07FF, 0xF81F, 0x07E0, 0x8410, 0xFBE0, 0x07FF, 0x4208),
    "PythonUltra Dark": (0x0000, 0xFFFF, 0x0000, 0xC618, 0x07FF, 0x0000, 0xE004, 0x041F, 0xFB44, 0x8430, 0xF8F9, 0x7B5F, 0xBF3A, 0x528A),
    "PythonUltra Light": (0xFFFF, 0x0000, 0x0000, 0xFFFF, 0x07E0, 0xFFFF, 0x001F, 0x7800, 0xF800, 0x3549, 0x8410, 0xA81F, 0x7B5F, 0xD69A),
}
THEME_NAMES = tuple(THEMES)
DEFAULT_THEME = "GitHub Dark"

# Palette tuple indexes.
BG, FG, BAR, BAR_FG, SEL_BG, SEL_FG, KEYWORD, BUILTIN, NUMBER, STRING, COMMENT, DECORATOR, TYPE, BORDER = range(14)

KEYWORDS_FLOW = {"if", "else", "elif", "for", "while", "break", "continue", "return", "yield", "pass", "raise", "try", "except", "finally", "with", "async", "await", "match", "case", "from", "import"}
KEYWORDS_DEF = {"def", "class", "lambda", "global", "nonlocal"}
KEYWORDS_LOGIC = {"and", "or", "not", "in", "is", "del"}
KEYWORDS_CONST = {"True", "False", "None"}
KEYWORDS = KEYWORDS_FLOW | KEYWORDS_DEF | KEYWORDS_LOGIC | KEYWORDS_CONST
BUILTINS = {"abs", "all", "any", "bin", "bool", "bytearray", "bytes", "callable", "chr", "compile", "dict", "dir", "divmod", "enumerate", "eval", "exec", "filter", "float", "format", "getattr", "globals", "hasattr", "hash", "help", "hex", "id", "input", "int", "isinstance", "iter", "len", "list", "locals", "map", "max", "memoryview", "min", "next", "object", "oct", "open", "ord", "pow", "print", "range", "repr", "reversed", "round", "set", "setattr", "slice", "sorted", "str", "sum", "tuple", "type", "vars", "zip"}
TYPES = {"int", "float", "str", "bool", "bytes", "bytearray", "list", "dict", "tuple", "set"}
MODULES = {"builtins", "gint", "numpy", "pygame", "py3d", "pythonultra", "pyeditor", "pyfiles", "ctypes", "os", "json", "time", "math", "random", "sys", "io", "struct", "array", "collections", "casioplot", "kandinsky", "ion"}
OPERATORS = "+-*/%=<>|&^~"
SYMBOLS = ("_", ".", ",", ":", ";", "!", "?", "@", "#", "%", "&", "*", "~", "(", ")", "[", "]", "{", "}", "=", "+", "-", "/", "\\", "|", "^", "<", ">", "'", '"')


def themes():
    return THEME_NAMES


def _gint():
    import gint
    return gint


def _maps(g):
    base = {
        g.KEY_0:"0", g.KEY_1:"1", g.KEY_2:"2", g.KEY_3:"3", g.KEY_4:"4", g.KEY_5:"5", g.KEY_6:"6", g.KEY_7:"7", g.KEY_8:"8", g.KEY_9:"9",
        g.KEY_DOT:".", g.KEY_ADD:"+", g.KEY_SUB:"-", g.KEY_MUL:"*", g.KEY_DIV:"/", g.KEY_LEFTPAR:"(", g.KEY_RIGHTPAR:")", g.KEY_COMMA:",", g.KEY_NEG:" ", g.KEY_EQUALS:"="
    }
    alpha = {
        g.KEY_XOT:"a", g.KEY_LOG:"b", g.KEY_LN:"c", g.KEY_SIN:"d", g.KEY_COS:"e", g.KEY_TAN:"f", g.KEY_FRAC:"g", g.KEY_FD:"h", g.KEY_LEFTPAR:"i", g.KEY_RIGHTPAR:"j", g.KEY_COMMA:"k", g.KEY_ARROW:"l",
        g.KEY_7:"m", g.KEY_8:"n", g.KEY_9:"o", g.KEY_4:"p", g.KEY_5:"q", g.KEY_6:"r", g.KEY_MUL:"s", g.KEY_DIV:"t", g.KEY_1:"u", g.KEY_2:"v", g.KEY_3:"w", g.KEY_ADD:"x", g.KEY_SUB:"y", g.KEY_0:"z", g.KEY_DOT:" ", g.KEY_EXP:'"', g.KEY_VARS:"_", g.KEY_NEG:" "
    }
    shift = {g.KEY_MUL:"{", g.KEY_DIV:"}", g.KEY_ADD:"[", g.KEY_SUB:"]", g.KEY_DOT:"=", g.KEY_0:":", g.KEY_EXP:"3.14159"}
    return base, alpha, shift


class Editor:
    def __init__(self, filename="new.py", theme=DEFAULT_THEME):
        self.g = _gint()
        self.base_map, self.alpha_map, self.shift_map = _maps(self.g)
        self.filename = filename or "new.py"
        self.lines = [""]
        self.cx = 0
        self.cy = 0
        self.scroll_x = 0
        self.scroll_y = 0
        self.mode = "NORMAL"
        self.alpha_mode = 0
        self.shift_active = False
        self.clipboard = ""
        self.sel_start = None
        self.last_search = ""
        self.dirty = False
        self.msg = ""
        self.theme_name = theme if theme in THEMES else DEFAULT_THEME
        self.palette = THEMES[self.theme_name]
        self.load_file(self.filename)

    def set_theme(self, name):
        if name in THEMES:
            self.theme_name = name
            self.palette = THEMES[name]
            self.msg = name

    def resolve_char(self, key):
        if self.shift_active and key in self.shift_map:
            return self.shift_map[key]
        if self.alpha_mode:
            c = self.alpha_map.get(key, self.base_map.get(key))
            if c and self.alpha_mode == 2:
                return c.upper()
            return c
        return self.base_map.get(key)

    def load_file(self, filename=None):
        if filename:
            self.filename = filename
        try:
            with open(self.filename, "r") as f:
                text = f.read()
            self.lines = text.split("\n") if text else [""]
            self.msg = "Loaded"
        except OSError:
            self.lines = [""]
            self.msg = "New"
        self.cx = self.cy = self.scroll_x = self.scroll_y = 0
        self.dirty = False
        gc.collect()

    def save_file(self, filename=None):
        if filename is not None:
            self.filename = filename.strip()
        if not self.filename:
            self.msg = "Name error"
            return False
        try:
            with open(self.filename, "w") as f:
                f.write("\n".join(self.lines))
            self.dirty = False
            self.msg = "Saved"
            return True
        except OSError as exc:
            self.msg = "Save " + str(exc)[:16]
            return False

    def _token_color(self, token, first):
        p = self.palette
        if first == "#": return p[COMMENT]
        if first in "\"'": return p[STRING]
        if first == "@": return p[DECORATOR]
        if token in KEYWORDS: return p[KEYWORD]
        if token in BUILTINS: return p[BUILTIN]
        if token in TYPES: return p[TYPE]
        if token in MODULES: return p[BUILTIN]
        if token and token[0].isdigit(): return p[NUMBER]
        if first in OPERATORS: return p[KEYWORD]
        return p[FG]

    def draw_line(self, text, y, line_index):
        g = self.g
        if self.scroll_x >= len(text):
            return
        i = self.scroll_x
        x = TEXT_X
        limit = min(len(text), self.scroll_x + MAX_COLS)
        while i < limit:
            c = text[i]
            j = i + 1
            if c == "#":
                j = limit
            elif c in "\"'":
                while j < limit:
                    if text[j] == c and text[j-1] != "\\":
                        j += 1
                        break
                    j += 1
            elif c.isalpha() or c == "_":
                while j < limit and (text[j].isalnum() or text[j] == "_"):
                    j += 1
            elif c.isdigit():
                while j < limit and (text[j].isdigit() or text[j] == "."):
                    j += 1
            elif c == "@":
                while j < limit and (text[j].isalnum() or text[j] in "_."):
                    j += 1
            token = text[i:j]
            color = self._token_color(token, c)
            # Visual selection is rendered one character at a time only when active.
            if self.mode == "VISUAL" and self.sel_start:
                a = self.sel_start
                b = (self.cy, self.cx)
                if a > b: a, b = b, a
                for k, ch in enumerate(token):
                    absolute = i + k
                    selected = a <= (line_index, absolute) <= b
                    if selected:
                        g.drect(x, y, x + FONT_W - 1, y + FONT_H - 1, self.palette[SEL_BG])
                    g.dtext(x, y, self.palette[SEL_FG] if selected else color, ch)
                    x += FONT_W
            else:
                g.dtext(x, y, color, token)
                x += FONT_W * len(token)
            i = j

    def draw(self):
        g = self.g
        p = self.palette
        g.dclear(p[BG])
        for row in range(MAX_ROWS):
            idx = self.scroll_y + row
            if idx >= len(self.lines):
                break
            self.draw_line(self.lines[idx], row * FONT_H, idx)
        ry = self.cy - self.scroll_y
        rx = self.cx - self.scroll_x
        if 0 <= ry < MAX_ROWS and 0 <= rx <= MAX_COLS:
            x = TEXT_X + rx * FONT_W
            y = ry * FONT_H
            if self.mode == "INSERT":
                g.drect(x, y, x + 1, y + FONT_H - 1, p[FG])
            else:
                g.drect_border(x, y, x + FONT_W - 1, y + FONT_H - 1, p[FG], 1, p[BG])
        y_info = SCREEN_H - NAV_H - INFO_H
        y_nav = SCREEN_H - NAV_H
        g.drect(0, y_info, SCREEN_W - 1, y_nav - 1, p[BG])
        g.drect(0, y_nav, SCREEN_W - 1, SCREEN_H - 1, p[BAR])
        mod = "A" if self.alpha_mode == 2 else ("a" if self.alpha_mode else "1")
        mark = "*" if self.dirty else ""
        info = "%s [%s] %d:%d %s%s %s" % (self.mode, mod, self.cy + 1, self.cx + 1, self.filename[-17:], mark, self.msg)
        g.dtext(2, y_info + 1, p[FG], info[:47])
        labels = ("Run", "Save", "New", "Open", "Find", "Theme")
        for n, label in enumerate(labels):
            g.dtext(3 + n * 66, y_nav + 1, p[BAR_FG], "F%d:%s" % (n + 1, label))
        g.dupdate()

    def popup(self, title, items):
        import pythonultra
        dark = self.theme_name != "GitHub Light" and self.theme_name != "PythonUltra Light"
        return pythonultra.popup(title, tuple(items), dark)

    def input_bar(self, prompt, initial=""):
        g = self.g
        text = initial
        while True:
            self.draw()
            p = self.palette
            g.drect(20, 80, 376, 116, p[BG])
            g.drect_border(20, 80, 376, 116, p[BORDER], 2, p[BG])
            g.dtext(28, 89, p[FG], (prompt + ": " + text + "_")[-43:])
            g.dupdate()
            k = g.getkey().key
            if k == g.KEY_EXIT:
                return None
            if k == g.KEY_EXE:
                return text
            if k == g.KEY_SHIFT:
                self.shift_active = not self.shift_active
                continue
            if k == g.KEY_ALPHA:
                if self.shift_active:
                    self.alpha_mode = 2
                else:
                    self.alpha_mode = 0 if self.alpha_mode else 1
                self.shift_active = False
                continue
            if k == g.KEY_DEL:
                if self.shift_active:
                    sym = self.popup("Symbols", SYMBOLS)
                    if sym: text += sym
                    self.shift_active = False
                else:
                    text = text[:-1]
                continue
            c = self.resolve_char(k)
            if c:
                text += c
            if self.shift_active:
                self.shift_active = False

    def move(self, dy, dx):
        self.cy = max(0, min(len(self.lines) - 1, self.cy + dy))
        self.cx = max(0, min(len(self.lines[self.cy]), self.cx + dx))
        if self.cy < self.scroll_y: self.scroll_y = self.cy
        if self.cy >= self.scroll_y + MAX_ROWS: self.scroll_y = self.cy - MAX_ROWS + 1
        if self.cx < self.scroll_x: self.scroll_x = self.cx
        if self.cx >= self.scroll_x + MAX_COLS - 2: self.scroll_x = self.cx - MAX_COLS + 3
        self.scroll_x = max(0, self.scroll_x)

    def insert(self, text):
        if not text: return
        line = self.lines[self.cy]
        self.lines[self.cy] = line[:self.cx] + text + line[self.cx:]
        self.cx += len(text)
        self.dirty = True
        self.move(0, 0)

    def backspace(self):
        if self.cx > 0:
            line = self.lines[self.cy]
            self.lines[self.cy] = line[:self.cx-1] + line[self.cx:]
            self.cx -= 1
            self.dirty = True
        elif self.cy > 0:
            prev = self.lines[self.cy-1]
            cur = self.lines.pop(self.cy)
            self.cy -= 1
            self.cx = len(prev)
            self.lines[self.cy] = prev + cur
            self.dirty = True
        self.move(0, 0)

    def newline(self):
        line = self.lines[self.cy]
        before = line[:self.cx]
        after = line[self.cx:]
        indent = ""
        for char in before:
            if char == " ": indent += " "
            else: break
        if before.rstrip().endswith(":"):
            indent += "    "
        self.lines[self.cy] = before
        self.lines.insert(self.cy + 1, indent + after)
        self.cy += 1
        self.cx = len(indent)
        self.dirty = True
        self.move(0, 0)

    def copy_selection(self, cut=False):
        if not self.sel_start:
            return
        a = self.sel_start
        b = (self.cy, self.cx)
        if a > b: a, b = b, a
        chunks = []
        for row in range(a[0], b[0] + 1):
            line = self.lines[row]
            left = a[1] if row == a[0] else 0
            right = b[1] + 1 if row == b[0] else len(line)
            chunks.append(line[left:right])
        self.clipboard = "\n".join(chunks)
        if cut:
            head = self.lines[a[0]][:a[1]]
            tail = self.lines[b[0]][b[1] + 1:]
            self.lines[a[0]] = head + tail
            for _ in range(b[0] - a[0]):
                self.lines.pop(a[0] + 1)
            self.cy, self.cx = a
            self.dirty = True
        self.mode = "NORMAL"
        self.sel_start = None
        self.msg = "Cut" if cut else "Copied"

    def find(self):
        query = self.input_bar("Find", self.last_search)
        if not query:
            return
        self.last_search = query
        for offset in range(len(self.lines)):
            row = (self.cy + offset) % len(self.lines)
            start = self.cx + 1 if row == self.cy else 0
            pos = self.lines[row].find(query, start)
            if pos >= 0:
                self.cy = row
                self.cx = pos
                self.msg = "Found"
                self.move(0, 0)
                return
        self.msg = "Not found"

    def theme_menu(self):
        choice = self.popup("Syntax Theme", THEME_NAMES)
        if choice:
            self.set_theme(choice)

    def catalog_menu(self):
        import pythonultra
        module = self.popup("Catalog", pythonultra.modules())
        if not module: return
        member = self.popup(module, pythonultra.catalog_data(module))
        if member:
            self.insert(member)

    def confirm_discard(self):
        if not self.dirty:
            return True
        return self.popup("Unsaved changes", ("Cancel", "Discard")) == "Discard"

    def run_code(self):
        if self.dirty and not self.save_file():
            return False
        try:
            with open(self.filename, "r") as f:
                code = f.read()
            scope = {"__name__":"__main__", "__file__":self.filename}
            exec(code, scope, scope)
            self.msg = "Run OK"
        except Exception as exc:
            print("Editor run error:", repr(exc))
            self.msg = "Run error"
        gc.collect()
        return True

    def run(self):
        g = self.g
        while True:
            self.draw()
            k = g.getkey().key
            if k == g.KEY_F1:
                if self.run_code():
                    return "run"
                continue
            if k == g.KEY_F2:
                self.save_file(); continue
            if k == g.KEY_F3:
                if self.confirm_discard():
                    self.filename = "new.py"; self.lines = [""]; self.cx = self.cy = 0; self.dirty = False; self.msg = "New"
                continue
            if k == g.KEY_F4:
                if self.confirm_discard():
                    name = self.input_bar("Open", self.filename)
                    if name: self.load_file(name)
                continue
            if k == g.KEY_F5:
                self.find(); continue
            if k == g.KEY_F6:
                self.theme_menu(); continue
            if k == g.KEY_SHIFT:
                self.shift_active = not self.shift_active; continue
            if k == g.KEY_ALPHA:
                if self.shift_active: self.alpha_mode = 2
                else: self.alpha_mode = 0 if self.alpha_mode else 1
                self.shift_active = False; continue
            if self.shift_active:
                if k == g.KEY_8:
                    self.mode = "VISUAL"; self.sel_start = (self.cy, self.cx); self.shift_active = False; continue
                if k == g.KEY_9:
                    self.insert(self.clipboard); self.shift_active = False; continue
                if k == g.KEY_4:
                    self.catalog_menu(); self.shift_active = False; continue
                if k == g.KEY_DEL:
                    sym = self.popup("Symbols", SYMBOLS)
                    if sym: self.insert(sym)
                    self.shift_active = False; continue
            if self.mode == "VISUAL":
                if k == g.KEY_OPTN:
                    self.copy_selection(False); continue
                if k == g.KEY_DEL:
                    self.copy_selection(True); continue
            if k == g.KEY_UP: self.move(-1, 0)
            elif k == g.KEY_DOWN: self.move(1, 0)
            elif k == g.KEY_LEFT: self.move(0, -1)
            elif k == g.KEY_RIGHT: self.move(0, 1)
            elif k == g.KEY_OPTN:
                self.mode = "NORMAL" if self.mode == "INSERT" else "INSERT"
                self.sel_start = None
            elif k == g.KEY_DEL:
                if self.mode == "INSERT": self.backspace()
            elif k == g.KEY_EXE:
                if self.mode != "VISUAL":
                    self.mode = "INSERT"
                    self.newline()
            elif k == g.KEY_EXIT:
                if self.confirm_discard(): return "exit"
            else:
                c = self.resolve_char(k)
                if c:
                    if self.mode == "NORMAL": self.mode = "INSERT"
                    if self.mode == "INSERT": self.insert(c)
            if self.shift_active and k not in (g.KEY_SHIFT, g.KEY_ALPHA):
                self.shift_active = False


def open_file(filename, theme=DEFAULT_THEME):
    """Edit an existing file. Returns 'run' if F1 requested execution."""
    return Editor(filename, theme).run()


def new_file(filename="new.py", theme=DEFAULT_THEME):
    """Create/edit a new Python file."""
    editor = Editor(filename, theme)
    if editor.msg != "New":
        editor.lines = [""]
        editor.dirty = True
    return editor.run()
