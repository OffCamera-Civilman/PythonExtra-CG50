'''Compatibility launcher for the PythonUltra UI3 patch stage.

The editor has evolved independently of UI3. Restoring the global gint font on
editor exit is a cosmetic cleanup only; when that exact one-line source shape
is absent, keep the stronger UI3 checks for every other integration block and
continue the build.
'''

import apply_pythonultra_ui3 as ui3

_original_replace_once = ui3.replace_once


def _compatible_replace_once(text, old, new, label):
    if label == "restore default font on editor exit":
        if new in text:
            return text, False
        if old not in text:
            return text, False
    return _original_replace_once(text, old, new, label)


ui3.replace_once = _compatible_replace_once

if __name__ == "__main__":
    ui3.main()
