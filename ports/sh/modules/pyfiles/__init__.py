"""PythonUltra graphical file manager for the fx-CG50."""

import gc
import os
import sys
import pyperm

__version__ = "0.3.1-cg50"

SCREEN_W = 396
SCREEN_H = 224
HEADER_H = 18
NAV_H = 13
ROW_H = 13
VISIBLE = (SCREEN_H - HEADER_H - NAV_H) // ROW_H

TEXT_EXTENSIONS = (
    ".py", ".txt", ".md", ".csv", ".json", ".ini", ".cfg", ".log",
    ".xml", ".html", ".css", ".js", ".c", ".h", ".cpp", ".hpp", ".sh",
    ".toml", ".yaml", ".yml", ".rst", ".dat",
)


def _gint():
    import gint
    return gint


def _join(folder, name):
    if folder == "/": return "/" + name
    return folder.rstrip("/") + "/" + name


def _parent(folder):
    if not folder or folder == "/": return "/"
    parts = folder.rstrip("/").split("/")
    value = "/".join(parts[:-1])
    return value if value else "/"


def _basename(path):
    return path.rstrip("/").split("/")[-1]


def _is_dir(path):
    return pyperm.is_dir(path)


def _exists(path):
    try:
        os.stat(path); return True
    except OSError:
        return False


def _looks_text(path):
    lower = path.lower()
    if lower.endswith(TEXT_EXTENSIONS): return True
    return "." not in _basename(path)


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
        try: names = os.listdir(self.folder)
        except OSError as exc:
            names = []; self.msg = "List " + str(exc)[:14]
        dirs, files = [], []
        for name in names:
            (dirs if _is_dir(_join(self.folder, name)) else files).append(name)
        dirs.sort(); files.sort()
        self.entries = [(name, True) for name in dirs] + [(name, False) for name in files]
        if self.entries: self.selected = max(0, min(self.selected, len(self.entries) - 1))
        else: self.selected = 0
        self.scroll = max(0, min(self.scroll, max(0, len(self.entries) - VISIBLE)))
        gc.collect()

    def selected_entry(self):
        return self.entries[self.selected] if self.entries else None

    def selected_path(self):
        entry = self.selected_entry()
        return _join(self.folder, entry[0]) if entry else None

    def draw(self):
        g, p = self.g, self.palette
        g.dclear(p[0])
        g.drect(0, 0, SCREEN_W - 1, HEADER_H - 1, p[2])
        title = "FILES " + self.folder
        if self.msg: title += "  " + self.msg
        g.dtext(4, 3, p[3], title[:47])
        if self.selected < self.scroll: self.scroll = self.selected
        if self.selected >= self.scroll + VISIBLE: self.scroll = self.selected - VISIBLE + 1
        for row in range(VISIBLE):
            idx = self.scroll + row
            if idx >= len(self.entries): break
            name, is_dir = self.entries[idx]
            y = HEADER_H + row * ROW_H
            sel = idx == self.selected
            bg = p[4] if sel else p[0]
            fg = p[5] if sel else p[1]
            g.drect(2, y, SCREEN_W - 3, y + ROW_H - 1, bg)
            marker = "[D] " if is_dir else ("[Z] " if name.lower().endswith(".zip") else "    ")
            g.dtext(6, y + 1, fg, (marker + name)[:46])
        y = SCREEN_H - NAV_H
        g.drect(0, y, SCREEN_W - 1, SCREEN_H - 1, p[2])
        # Compact labels are deliberately kept inside six 66-pixel slots.
        for i, label in enumerate(("RUN", "EDIT", "NEW", "REN", "DEL", "EDTR")):
            g.dtext(2 + i * 66, y + 1, p[3], "F%d:%s" % (i + 1, label))
        g.dupdate()

    def popup(self, title, items):
        import pythonultra
        dark = self.theme_name not in ("GitHub Light", "PythonUltra Light")
        return pythonultra.popup(title, tuple(items), dark)

    def input_bar(self, prompt, initial=""):
        g = self.g
        base, alpha, shift = self.pyeditor._maps(g)
        text = initial; alpha_mode = 0; shift_on = False
        while True:
            self.draw(); p = self.palette
            g.drect(18, 78, 378, 116, p[0])
            g.drect_border(18, 78, 378, 116, p[13], 2, p[0])
            g.dtext(26, 88, p[1], (prompt + ": " + text + "_")[-44:])
            g.dupdate(); key = g.getkey().key
            if key == g.KEY_EXIT: return None
            if key == g.KEY_EXE: return text
            if key == g.KEY_SHIFT: shift_on = not shift_on; continue
            if key == g.KEY_ALPHA:
                alpha_mode = 2 if shift_on else (0 if alpha_mode else 1); shift_on = False; continue
            if key == g.KEY_DEL:
                if shift_on:
                    symbol = self.popup("Symbols", self.pyeditor.SYMBOLS)
                    if symbol: text += symbol
                    shift_on = False
                else: text = text[:-1]
                continue
            if shift_on and key in shift: char = shift[key]
            elif alpha_mode:
                char = alpha.get(key, base.get(key))
                if char and alpha_mode == 2: char = char.upper()
            else: char = base.get(key)
            if char: text += char
            if shift_on: shift_on = False

    def enter_selected(self):
        entry = self.selected_entry()
        if not entry: return
        name, is_dir = entry; path = _join(self.folder, name)
        if is_dir:
            self.folder = path; self.selected = self.scroll = 0; self.msg = ""; self.refresh(); return
        if name.lower().endswith(".py"):
            choice = self.popup(name, ("Run", "Edit", "Cancel"))
            if choice == "Run": self.run_file(path)
            elif choice == "Edit": self.edit_file(path)
        elif name.lower().endswith(".zip"):
            choice = self.popup(name, ("Extract ZIP", "Cancel"))
            if choice == "Extract ZIP": self.extract_selected()
        else: self.edit_file(path)

    def run_file(self, path=None):
        path = path or self.selected_path()
        if not path or _is_dir(path) or not path.lower().endswith(".py"):
            self.msg = "Run needs .py"; return
        old_path = list(sys.path)
        try:
            # Like `python file.py` on Linux, interpreter execution needs read,
            # not execute, permission. Direct ./file.py is handled in pyterm.
            pyperm.require_read(path)
            folder = path.rsplit("/", 1)[0] or "/"
            if folder not in sys.path: sys.path.insert(0, folder)
            with open(path, "r") as f: code = f.read()
            scope = {"__name__":"__main__", "__file__":path}
            exec(code, scope, scope); self.msg = "Run OK"
        except Exception as exc:
            print("File run error:", repr(exc)); self.msg = "Run error"
        finally:
            sys.path[:] = old_path; gc.collect()

    def edit_file(self, path=None):
        path = path or self.selected_path()
        if not path or _is_dir(path): self.msg = "Select file"; return
        if not _looks_text(path):
            if self.popup("Unknown/binary type", ("Edit as text", "Cancel")) != "Edit as text": return
        try:
            pyperm.require_read(path)
            result = self.pyeditor.open_file(path, self.theme_name)
            if result == "run" and path.lower().endswith(".py"): self.run_file(path)
            self.refresh()
        except Exception as exc: self.msg = "Edit " + str(exc)[:12]

    def open_editor(self):
        """Open the editor directly from F6, independent of file selection."""
        try:
            result = self.pyeditor.new_file(_join(self.folder, "new.py"), self.theme_name)
            if result == "run":
                self.run_file(_join(self.folder, "new.py"))
            self.refresh()
        except Exception as exc:
            self.msg = "Editor " + str(exc)[:10]

    def create_new(self):
        kind = self.popup("New", ("File", "Folder", "Cancel"))
        if kind == "File":
            name = self.input_bar("New file", "new.txt")
            if not name: return
            path = _join(self.folder, name)
            if _exists(path): self.msg = "Already exists"; return
            try:
                with open(path, "w") as f: f.write("")
                self.msg = "Created"; self.refresh(); self.edit_file(path)
            except OSError as exc: self.msg = "Create " + str(exc)[:12]
        elif kind == "Folder":
            name = self.input_bar("New folder", "folder")
            if not name: return
            try:
                os.mkdir(_join(self.folder, name)); self.msg = "Folder created"; self.refresh()
            except OSError as exc: self.msg = "Mkdir " + str(exc)[:12]

    def rename_selected(self):
        path = self.selected_path()
        if not path: return
        old = _basename(path); new = self.input_bar("Rename", old)
        if not new or new == old: return
        dest = _join(self.folder, new)
        if _exists(dest): self.msg = "Name exists"; return
        try:
            pyperm.require_write(path)
            os.rename(path, dest); pyperm.move(path, dest); self.msg = "Renamed"; self.refresh()
        except OSError as exc: self.msg = "Rename " + str(exc)[:12]

    def delete_selected(self):
        entry = self.selected_entry(); path = self.selected_path()
        if not entry or not path: return
        name, is_dir = entry
        if self.popup("Delete " + name[:18], ("Cancel", "DELETE")) != "DELETE": return
        try:
            pyperm.require_write(path)
            if is_dir: os.rmdir(path)
            else: os.remove(path)
            pyperm.remove(path); self.msg = "Deleted"; self.refresh()
        except OSError as exc: self.msg = "Delete " + str(exc)[:12]

    def permission_menu(self):
        path = self.selected_path()
        if not path:
            self.msg = "Select item"; return
        current = pyperm.get_mode(path)
        choice = self.popup("Permissions %03o" % current, (
            "444 read only", "600 owner rw", "644 rw/r/r", "700 owner rwx",
            "755 rwx/rx/rx", "777 all rwx", "Custom", "Cancel"))
        if not choice or choice == "Cancel": return
        modes = {"444 read only":"444", "600 owner rw":"600", "644 rw/r/r":"644",
                 "700 owner rwx":"700", "755 rwx/rx/rx":"755", "777 all rwx":"777"}
        mode = modes.get(choice)
        if choice == "Custom": mode = self.input_bar("chmod mode", "%03o" % current)
        if not mode: return
        try:
            pyperm.chmod(path, mode); self.msg = "chmod " + mode
        except Exception as exc: self.msg = "chmod " + str(exc)[:12]

    def compress_selected(self):
        path = self.selected_path()
        if not path: self.msg = "Select item"; return
        import zipfile
        default = _basename(path.rstrip("/")) + ".zip"
        name = self.input_bar("ZIP name", default)
        if not name: return
        if not name.lower().endswith(".zip"): name += ".zip"
        archive = _join(self.folder, name)
        try:
            pyperm.require_read(path)
            zipfile.compress(path, archive); self.msg = "ZIP created"; self.refresh()
        except Exception as exc:
            print("ZIP error:", repr(exc)); self.msg = "ZIP error"

    def extract_selected(self):
        path = self.selected_path()
        if not path or not path.lower().endswith(".zip"): self.msg = "Select .zip"; return
        import zipfile
        default = _basename(path)[:-4] or "unzipped"
        name = self.input_bar("Extract folder", default)
        if not name: return
        try:
            pyperm.require_read(path)
            zipfile.extract(path, _join(self.folder, name)); self.msg = "ZIP extracted"; self.refresh()
        except Exception as exc:
            print("Unzip error:", repr(exc)); self.msg = "Unzip error"

    def more_menu(self):
        choice = self.popup("More", (
            "Up one folder", "Permissions", "Compress to ZIP", "Extract ZIP", "Theme",
            "PythonUltra Info", "Refresh", "Exit Files"))
        if choice == "Up one folder":
            self.folder = _parent(self.folder); self.selected = self.scroll = 0; self.refresh()
        elif choice == "Permissions": self.permission_menu()
        elif choice == "Compress to ZIP": self.compress_selected()
        elif choice == "Extract ZIP": self.extract_selected()
        elif choice == "Theme":
            theme = self.popup("Files Theme", self.pyeditor.THEME_NAMES)
            if theme:
                self.theme_name = theme; self.palette = self.pyeditor.THEMES[theme]; self.msg = theme
        elif choice == "PythonUltra Info":
            import pythonultra
            pythonultra.info_ui(self.theme_name not in ("GitHub Light", "PythonUltra Light"))
        elif choice == "Refresh": self.refresh()
        elif choice == "Exit Files": return "exit"
        return None

    def run(self):
        g = self.g
        while True:
            self.draw(); key = g.getkey().key
            if key == g.KEY_UP and self.entries: self.selected = max(0, self.selected - 1)
            elif key == g.KEY_DOWN and self.entries: self.selected = min(len(self.entries) - 1, self.selected + 1)
            elif key in (g.KEY_EXE, g.KEY_RIGHT): self.enter_selected()
            elif key == g.KEY_LEFT:
                self.folder = _parent(self.folder); self.selected = self.scroll = 0; self.refresh()
            elif key == g.KEY_F1: self.run_file()
            elif key == g.KEY_F2: self.edit_file()
            elif key == g.KEY_F3: self.create_new()
            elif key == g.KEY_F4: self.rename_selected()
            elif key == g.KEY_F5: self.delete_selected()
            elif key == g.KEY_F6: self.open_editor()
            elif key == g.KEY_OPTN:
                if self.more_menu() == "exit": return
            elif key == g.KEY_EXIT:
                if self.folder != "/":
                    self.folder = _parent(self.folder); self.selected = self.scroll = 0; self.refresh()
                else: return


def browse(folder="/", theme="GitHub Dark"):
    return Browser(folder, theme).run()
