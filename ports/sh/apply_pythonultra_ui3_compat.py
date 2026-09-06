'''Compatibility launcher for the PythonUltra UI3 patch stage.

The editor has evolved independently of UI3. Restoring the global gint font on
editor exit is a cosmetic cleanup only; when that exact one-line source shape
is absent, keep the stronger UI3 checks for every other integration block and
continue the build.

UI3 also adds string-based font selectors to modgint.c. Ensure the generated C
source declares strcmp() explicitly so the SH cross-compiler can build it with
implicit-function declarations treated as errors.
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


ui3.replace_once = _compatible_replace_once

if __name__ == "__main__":
    ui3.main()
    _ensure_string_header()
