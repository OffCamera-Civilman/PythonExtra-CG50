"""Build-time regression checks for PythonUltra calculator features."""

from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[3]
SH = ROOT / "ports" / "sh"
MODULES = SH / "modules"

sys.path.insert(0, str(MODULES))
import numpy as np

# The public frozen package must own the numpy name. The native helper stays
# private so numpy.matrix is visible on actual calculator hardware.
native = (SH / "modnumpy.c").read_text(encoding="utf-8")
assert "MP_REGISTER_MODULE(MP_QSTR__numpy_fast, modnumpy_module);" in native
assert "MP_REGISTER_MODULE(MP_QSTR_numpy, modnumpy_module);" not in native

m = np.matrix([[4.0, 7.0], [2.0, 6.0]])
assert m.T.tolist() == [[4.0, 2.0], [7.0, 6.0]]
ident = m * m.I
assert abs(ident[0, 0] - 1.0) < 1e-5
assert abs(ident[0, 1]) < 1e-5
assert np.cross([1, 0, 0], [0, 1, 0]) == [0, 0, 1]
assert abs(np.norm([3, 4]) - 5.0) < 1e-9
assert np.normalize([0, 5]) == [0.0, 1.0]
assert np.lerp([0, 10], [10, 20], 0.5) == [5.0, 15.0]

main = (SH / "main.c").read_text(encoding="utf-8")
widget = (SH / "widget_shell.c").read_text(encoding="utf-8")
config = (SH / "mpconfigport.h").read_text(encoding="utf-8")
manifest = (SH / "manifest.py").read_text(encoding="utf-8")
modgint = (SH / "modgint.c").read_text(encoding="utf-8")
pyterm = (MODULES / "pyterm" / "__init__.py").read_text(encoding="utf-8")
pyfiles = (MODULES / "pyfiles" / "__init__.py").read_text(encoding="utf-8")
pyeditor = (MODULES / "pyeditor" / "__init__.py").read_text(encoding="utf-8")
pyperm = (MODULES / "pyperm" / "__init__.py").read_text(encoding="utf-8")

assert "pythonultra_help_text" in main
assert "MICROPY_PY_BUILTINS_HELP_TEXT     pythonultra_help_text" in config
assert "key == KEY_F3" in main and "_pu.catalog_ui(" in main
assert "key == KEY_F4" in main and "pe_apply_theme()" in main
assert "key == KEY_F5" in main and "pyeditor" in main
assert "key == KEY_F6" in main and "_pu.info_ui(" in main
assert "pe_terminal_dispatch" in main and "MP_QSTR_pyterm" in main
assert "pe_terminal_startup" in main and "pe_apply_terminal_font" in main
assert "widget_shell_history_recall" in widget
assert "widget_shell_history_push" in widget
assert "modgint_dfont_size" in modgint and "OBJ(dfont_size)" in modgint
assert 'freeze("modules", "pythonultra", opt=3)' in manifest
assert 'freeze("modules", "pyeditor", opt=3)' in manifest
assert 'freeze("modules", "pyfiles", opt=3)' in manifest
assert 'freeze("modules", "pyperm", opt=3)' in manifest
assert 'freeze("modules", "pyterm", opt=3)' in manifest
assert 'freeze("modules", "zipfile", opt=3)' in manifest
assert 'args[0] in ("-h", "--help")' in pyterm and "def dispatch(" in pyterm
assert 'endswith("--h")' not in pyterm
assert "zip" in pyterm and "unzip" in pyterm
assert '"chmod"' in pyterm and "pyperm.chmod" in pyterm
assert 'RC_PATH = "/.pythonultrarc"' in pyterm
assert 'HISTORY_PATH = "/.pythonultra_history"' in pyterm
assert '"alias"' in pyterm and '"history"' in pyterm and '"font"' in pyterm
assert 'cmd.startswith("./")' in pyterm and "require_execute" in pyterm
assert "create_new" in pyfiles and "rename_selected" in pyfiles and "delete_selected" in pyfiles
assert "permission_menu" in pyfiles and "pyperm.require_write" in pyfiles
assert "GitHub Dark" in pyeditor and "GitHub Light" in pyeditor and "Linux" in pyeditor
assert "set_font_size" in pyeditor and "pyperm.require_write" in pyeditor
assert 'DB_PATH = "/.pythonultra_permissions"' in pyperm
assert "def format_mode" in pyperm and "def executable" in pyperm

print("PythonUltra feature regression checks passed")
