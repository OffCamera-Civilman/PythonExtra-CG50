"""PythonUltra graphical file manager for the fx-CG50."""

import gc
import os
import sys

__version__ = "0.1.0-cg50"

SCREEN_W = 396
SCREEN_H = 224
HEADER_H = 18
NAV_H = 13
ROW_H = 13
VISIBLE = (SCREEN_H - HEADER_H - NAV_H) // ROW_H


def _gint():
    import gint
    return gint


def _join(folder, name):
    if folder == "/":
        return "/" + name
    return folder.rstrip("/") + "/" + name


def _parent(folder):
    if not folder or folder == "/":
        return "/"
    parts = folder.rstrip("/").split("/")
    parent = "/".join(parts[:-1])
    return parent if parent else "/"


def _basename(path):
    return path.rstrip("/").split("/")[-1]


def _is_dir(path):
    try:
        return bool(os.stat(path)[0] & 0x4000)
    except OSError:
        return False


def _exists(path):
    try:
        os.stat(path)
        return True
    except OSError:
        return False


class Browser:
    def __init__(self, folder="/", theme="GitHub Dark"):
        import pyeditor
        self.g = _gint()
        self.pyeditor = pyeditor
        self.folder = folder or "/"
        self.theme_name = theme if theme in pyeditor.THEMES else pyeditor.DEFAULT_THEME
        self.palette = pyeditor.THEMES[self.theme_name]
        self.entries = []
        self.selected = 0
        self.scroll = 0
        self.msg = ""
        self.refresh()

    def refresh(self):
        try:
            names = os.listdir(self.folder)
        except OSError as exc:
            names = []
            self.msg = "List " + str(exc)[:14]
        dirs = []
        files = []
        for name in names:
            if _is_dir(_join(self.folder, name)):
                dirs.append(name)
            else:
                files.append(name)
        dirs.sort()
        files.sort()
        self.entries = [(name, True) for name in dirs] + [(name, False) for name in files]
        if self.entries:
            self.selected = max(0, min(self.selected, len(self.entries) - 1))
        else:
            self.selected = 0
        self.scroll = max(0, min(self.scroll, max(0, len(self.entries) - VISIBLE)))
        gc.collect()

    def selected_entry(self):
        if not self.entries:
            return None
        return self.entries[self.selected]

    def selected_path(self):
        entry = self.selected_entry()
        return _join(self.folder, entry[0]) if entry else None

    def set_theme(self, name):
        if name in self.pyeditor.THEMES:
            self.theme_name = name
            self.palette = self.pyeditor.THEMES[name]
            self.msg = name

    def draw(self):
        g = self.g
        p = self.palette
        g.dclear(p[0])
        g.drect(0, 0, SCREEN_W - 1, HEADER_H - 1, p[2])
        title = "FILES " + self.folder
        if self.msg:
            title += "  " + self.msg
        g.dtext(4, 3, p[3], title[:47])

        if self.selected < self.scroll:
            self.scroll = self.selected
        if self.selected >= self.scroll + VISIBLE:
            self.scroll = self.selected - VISIBLE + 1

        for row in range(VISIBLE):
            idx = self.scroll + row
            if idx >= len(self.entries):
                break
            name, is_dir = self.entries[idx]
            y = HEADER_H + row * ROW_H
            selected = idx == self.selected
            bg = p[4] if selected else p[0]
            fg = p[5] if selected else p[1]
            g.drect(2, y, SCREEN_W - 3, y + ROW_H - 1, bg)
            prefix = "[D] " if is_dir else "    "
            g.dtext(6, y + 1, fg, (prefix + name)[:46])

        y = SCREEN_H - NAV_H
        g.drect(0, y, SCREEN_W - 1, SCREEN_H - 1, p[2])
        labels = ("Run", "Edit", "New", "Rename", "Delete", "More")
        for i, label in enumerate(labels):
            g.dtext(2 + i * 66, y + 1, p[3], "F%d:%s" % (i + 1, label))
        g.dupdate()

    def popup(self, title, items):
        import pythonultra
        dark = self.theme_name not in ("GitHub Light", "PythonUltra Light")
        return pythonultra.popup(title, tuple(items), dark)

    def input_bar(self, prompt, initial=""):
        g = self.g
        base, alpha, shift = self.pyeditor._maps(g)
        text = initial
        alpha_mode = 0
        shift_on = False
        while True:
            self.draw()
            p = self.palette
            g.drect(18, 78, 378, 116, p[0])
            g.drect_border(18, 78, 378, 116, p[13], 2, p[0])
            g.dtext(26, 88, p[1], (prompt + ": " + text + "_")[-44:])
            g.dupdate()
            key = g.getkey().key
            if key == g.KEY_EXIT:
                return None
            if key == g.KEY_EXE:
                return text
            if key == g.KEY_SHIFT:
                shift_on = not shift_on
                continue
            if key == g.KEY_ALPHA:
                alpha_mode = 2 if shift_on else (0 if alpha_mode else 1)
                shift_on = False
                continue
            if key == g.KEY_DEL:
                if shift_on:
                    symbol = self.popup("Symbols", self.pyeditor.SYMBOLS)
                    if symbol:
                        text += symbol
                    shift_on = False
                else:
                    text = text[:-1]
                continue
            if shift_on and key in shift:
                char = shift[key]
            elif alpha_mode:
                char = alpha.get(key, base.get(key))
                if char and alpha_mode == 2:
                    char = char.upper()
            else:
                char = base.get(key)
            if char:
                text += char
            if shift_on:
                shift_on = False

    def enter_selected(self):
        entry = self.selected_entry()
        if not entry:
            return
        name, is_dir = entry
        if is_dir:
            self.folder = _join(self.folder, name)
            self.selected = self.scroll = 0
            self.msg = ""
            self.refresh()
        elif name.endswith(".py"):
            choice = self.popup(name, ("Run", "Edit", "Cancel"))
            if choice == "Run":
                self.run_file(_join(self.folder, name))
            elif choice == "Edit":
                self.edit_file(_join(self.folder, name))
        else:
            self.msg = "Not Python"

    def run_file(self, path=None):
        path = path or self.selected_path()
        if not path or _is_dir(path):
            self.msg = "Select .py"
            return
        if not path.endswith(".py"):
            self.msg = "Not Python"
            return
        old_path = list(sys.path)
        try:
            folder = path.rsplit("/", 1)[0] or "/"
            if folder not in sys.path:
                sys.path.insert(0, folder)
            with open(path, "r") as f:
                code = f.read()
            scope = {"__name__": "__main__", "__file__": path}
            exec(code, scope, scope)
            self.msg = "Run OK"
        except Exception as exc:
            print("File run error:", repr(exc))
            self.msg = "Run error"
        finally:
            sys.path[:] = old_path
            gc.collect()

    def edit_file(self, path=None):
        path = path or self.selected_path()
        if not path or _is_dir(path):
            self.msg = "Select file"
            return
        result = self.pyeditor.open_file(path, self.theme_name)
        if result == "run":
            self.run_file(path)
        self.refresh()

    def create_new(self):
        kind = self.popup("New", ("Python file", "Folder", "Cancel"))
        if kind == "Python file":
            name = self.input_bar("New file", "new.py")
            if not name:
                return
            if not name.endswith(".py"):
                name += ".py"
            path = _join(self.folder, name)
            if _exists(path):
                self.msg = "Already exists"
                return
            try:
                with open(path, "w") as f:
                    f.write("")
                self.msg = "Created"
                self.refresh()
                self.edit_file(path)
            except OSError as exc:
                self.msg = "Create " + str(exc)[:12]
        elif kind == "Folder":
            name = self.input_bar("New folder", "folder")
            if not name:
                return
            path = _join(self.folder, name)
            try:
                os.mkdir(path)
                self.msg = "Folder created"
                self.refresh()
            except OSError as exc:
                self.msg = "Mkdir " + str(exc)[:12]

    def rename_selected(self):
        path = self.selected_path()
        if not path:
            return
        old = _basename(path)
        new = self.input_bar("Rename", old)
        if not new or new == old:
            return
        dest = _join(self.folder, new)
        if _exists(dest):
            self.msg = "Name exists"
            return
        try:
            os.rename(path, dest)
            self.msg = "Renamed"
            self.refresh()
        except OSError as exc:
            self.msg = "Rename " + str(exc)[:12]

    def delete_selected(self):
        entry = self.selected_entry()
        path = self.selected_path()
        if not entry or not path:
            return
        name, is_dir = entry
        if self.popup("Delete " + name[:18], ("Cancel", "DELETE")) != "DELETE":
            return
        try:
            if is_dir:
                os.rmdir(path)
            else:
                os.remove(path)
            self.msg = "Deleted"
            self.refresh()
        except OSError as exc:
            self.msg = "Delete " + str(exc)[:12]

    def more_menu(self):
        choice = self.popup("More", ("Up one folder", "Theme", "PythonUltra Info", "Refresh", "Exit Files"))
        if choice == "Up one folder":
            self.folder = _parent(self.folder)
            self.selected = self.scroll = 0
            self.refresh()
        elif choice == "Theme":
            theme = self.popup("Files Theme", self.pyeditor.THEME_NAMES)
            if theme:
                self.set_theme(theme)
        elif choice == "PythonUltra Info":
            import pythonultra
            pythonultra.info_ui(self.theme_name not in ("GitHub Light", "PythonUltra Light"))
        elif choice == "Refresh":
            self.refresh()
        elif choice == "Exit Files":
            return "exit"
        return None

    def run(self):
        g = self.g
        while True:
            self.draw()
            key = g.getkey().key
            if key == g.KEY_UP:
                if self.entries: self.selected = max(0, self.selected - 1)
            elif key == g.KEY_DOWN:
                if self.entries: self.selected = min(len(self.entries) - 1, self.selected + 1)
            elif key in (g.KEY_EXE, g.KEY_RIGHT):
                self.enter_selected()
            elif key == g.KEY_LEFT:
                self.folder = _parent(self.folder)
                self.selected = self.scroll = 0
                self.refresh()
            elif key == g.KEY_F1:
                self.run_file()
            elif key == g.KEY_F2:
                self.edit_file()
            elif key == g.KEY_F3:
                self.create_new()
            elif key == g.KEY_F4:
                self.rename_selected()
            elif key == g.KEY_F5:
                self.delete_selected()
            elif key == g.KEY_F6:
                if self.more_menu() == "exit":
                    return
            elif key == g.KEY_EXIT:
                if self.folder != "/":
                    self.folder = _parent(self.folder)
                    self.selected = self.scroll = 0
                    self.refresh()
                else:
                    return


def browse(folder="/", theme="GitHub Dark"):
    """Open PythonUltra's integrated file manager."""
    return Browser(folder, theme).run()
