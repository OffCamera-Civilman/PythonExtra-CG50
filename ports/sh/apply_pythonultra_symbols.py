'''Add PythonUltra's SHIFT+DEL programming-symbol picker to the native shell.'''

from pathlib import Path

ROOT = Path(__file__).resolve().parent
MAIN = ROOT / "main.c"
WIDGET_C = ROOT / "widget_shell.c"
WIDGET_H = ROOT / "widget_shell.h"
PYULTRA = ROOT / "modules" / "pythonultra" / "__init__.py"


def replace_once(text, old, new, label):
    if new in text:
        return text, False
    if old not in text:
        raise SystemExit("Unable to locate PythonUltra symbol block: " + label)
    return text.replace(old, new, 1), True


def patch(path, replacements):
    text = path.read_text(encoding="utf-8")
    changed = False
    for old, new, label in replacements:
        text, did = replace_once(text, old, new, label)
        changed = changed or did
    if changed:
        path.write_text(text, encoding="utf-8")
    return changed


def patch_python_ui():
    return patch(PYULTRA, [
        (
            '''UI_DARK = {"bg":0x1082, "fg":0xD69A, "bar":0x3186, "sel_bg":0x2148, "sel_fg":0xFFFF, "title":0x7D7C}\n\n\ndef modules():\n''',
            '''UI_DARK = {"bg":0x1082, "fg":0xD69A, "bar":0x3186, "sel_bg":0x2148, "sel_fg":0xFFFF, "title":0x7D7C}\n\n# Common ASCII characters used in Python, shell commands, paths, expressions,\n# data formats and general programming. SHIFT+DEL opens this list in Terminal.\n_PROGRAMMING_SYMBOLS = (\n    "@", "#", "$", "%", "^", "&", "*", "!", "?", "~", "`",\n    "<", ">", "=", "+", "-", "/", "\\\\", "|", "_",\n    "(", ")", "[", "]", "{", "}", "'", '\"', ":", ";", ",", ".",\n)\n\n\ndef modules():\n''',
            "programming symbol list",
        ),
        (
            '''def catalog_ui(dark=False):\n''',
            '''def symbol_ui(dark=False):\n    """Open the SHIFT+DEL programming-symbol picker and return one character."""\n    return popup("Programming Symbols", _PROGRAMMING_SYMBOLS, dark)\n\n\ndef catalog_ui(dark=False):\n''',
            "symbol UI helper",
        ),
    ])


def patch_widget():
    changed = patch(WIDGET_H, [
        (
            '''extern uint16_t WIDGET_SHELL_MOD_CHANGED;\nextern uint16_t WIDGET_SHELL_INPUT;\n''',
            '''extern uint16_t WIDGET_SHELL_MOD_CHANGED;\nextern uint16_t WIDGET_SHELL_INPUT;\nextern uint16_t WIDGET_SHELL_SYMBOLS;\n''',
            "symbol event declaration",
        ),
    ])
    changed = patch(WIDGET_C, [
        (
            '''J_DEFINE_EVENTS(WIDGET_SHELL_MOD_CHANGED, WIDGET_SHELL_INPUT)\n''',
            '''J_DEFINE_EVENTS(WIDGET_SHELL_MOD_CHANGED, WIDGET_SHELL_INPUT, WIDGET_SHELL_SYMBOLS)\n''',
            "symbol event definition",
        ),
        (
            '''    if(ev.key == KEY_DEL) {\n        console_delete_at_cursor(s->console, 1);\n        return true;\n    }\n''',
            '''    if(ev.key == KEY_DEL) {\n        if(ev.shift)\n            jwidget_emit(s, (jevent){ .type = WIDGET_SHELL_SYMBOLS });\n        else\n            console_delete_at_cursor(s->console, 1);\n        return true;\n    }\n''',
            "SHIFT+DEL symbol event",
        ),
    ]) or changed
    return changed


def patch_main():
    return patch(MAIN, [
        (
            '''    if(e.type == WIDGET_SHELL_MOD_CHANGED)\n        PE.scene->widget.update = true;\n\n    if(e.type == WIDGET_SHELL_INPUT) {\n''',
            '''    if(e.type == WIDGET_SHELL_MOD_CHANGED)\n        PE.scene->widget.update = true;\n\n    if(e.type == WIDGET_SHELL_SYMBOLS) {\n        nlr_buf_t nlr;\n        if(nlr_push(&nlr) == 0) {\n            mp_obj_t module = mp_import_name(MP_QSTR_pythonultra,\n                mp_const_none, MP_OBJ_NEW_SMALL_INT(0));\n            mp_obj_t function = mp_load_attr(module, MP_QSTR_symbol_ui);\n            mp_obj_t args[1] = { mp_obj_new_bool(pe_dark_mode) };\n            mp_obj_t result = mp_call_function_n_kw(function, 1, 0, args);\n            if(result != mp_const_none) {\n                char const *symbol = mp_obj_str_get_str(result);\n                console_write_raw(PE.console, symbol, strlen(symbol));\n                PE.scene->widget.update = true;\n            }\n            nlr_pop();\n        }\n        else {\n            mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(nlr.ret_val));\n        }\n    }\n\n    if(e.type == WIDGET_SHELL_INPUT) {\n''',
            "terminal symbol picker handler",
        ),
    ])


def main():
    changed = []
    if patch_python_ui(): changed.append("catalog")
    if patch_widget(): changed.append("widget")
    if patch_main(): changed.append("native")
    print("PythonUltra symbols: " + (", ".join(changed) if changed else "already applied"))


if __name__ == "__main__":
    main()
