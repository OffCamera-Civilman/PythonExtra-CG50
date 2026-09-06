'''Compatibility launcher for the PythonUltra UI3 patch stage.

The editor has evolved independently of UI3. Restoring the global gint font on
editor exit is a cosmetic cleanup only; when that exact one-line source shape
is absent, keep the stronger UI3 checks for every other integration block and
continue the build.

UI3 also adds string-based font selectors to modgint.c. Ensure the generated C
source declares strcmp() explicitly so the SH cross-compiler can build it with
implicit-function declarations treated as errors.

Hardware follow-up: UI2 historically forced Terminal as the startup view even
though the base application starts in Files. The current PythonUltra preference
is enhanced Files first. This compatibility stage runs after UI2, so it restores
that behavior without allowing the older UI2 replacement to undo it. It also
avoids frozen-QSTR punctuation names in the terminal manual separator.
'''

from pathlib import Path

import apply_pythonultra_ui3 as ui3

_original_replace_once = ui3.replace_once


def _compatible_replace_once(text, old, new, label):
    if label == "restore default font on editor exit":
        if new in text:
            return text, False
        if old not in text:
            return text, False
    return _original_replace_once(text, old, new, label)


def _ensure_string_header():
    path = Path(__file__).with_name("modgint.c")
    text = path.read_text(encoding="utf-8")
    if "#include <string.h>" in text:
        return
    marker = "#include <stdlib.h>\n"
    if marker not in text:
        raise SystemExit("Unable to locate modgint standard include block")
    text = text.replace(marker, marker + "#include <string.h>\n", 1)
    path.write_text(text, encoding="utf-8")


def _prefer_files_startup():
    path = Path(__file__).with_name("main.c")
    text = path.read_text(encoding="utf-8")
    old = '''    /* Initial state: terminal first; F1 opens enhanced Files. */
    jfileselect_browse(PE.fileselect, "/");
    jscene_show_and_focus(PE.scene, PE.shell);
    jwidget_set_visible(PE.title, PE.show_title_in_shell);
'''
    new = '''    /* Initial state: enhanced Files first; F2 opens Terminal. */
    jfileselect_browse(PE.fileselect, "/");
    jscene_show_and_focus(PE.scene, PE.shell);
    jwidget_set_visible(PE.title, PE.show_title_in_shell);
    if(pe_dark_mode)
        pe_run_python_action("import pyfiles as _pf; _pf.browse('/', 'GitHub Dark')");
    else
        pe_run_python_action("import pyfiles as _pf; _pf.browse('/', 'GitHub Light')");
'''
    if new in text:
        return
    if old not in text:
        raise SystemExit("Unable to locate PythonUltra terminal-first startup block")
    path.write_text(text.replace(old, new, 1), encoding="utf-8")


def _runtime_help_separator():
    path = Path(__file__).with_name("modules") / "pyterm" / "__init__.py"
    text = path.read_text(encoding="utf-8")
    old = '    print("-" * 30)\n'
    new = '    print(chr(45) * 30)\n'
    if new in text:
        return
    if old not in text:
        raise SystemExit("Unable to locate PythonUltra manual separator")
    path.write_text(text.replace(old, new, 1), encoding="utf-8")


ui3.replace_once = _compatible_replace_once

if __name__ == "__main__":
    ui3.main()
    _ensure_string_header()
    _prefer_files_startup()
    _runtime_help_separator()
