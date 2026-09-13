"""Keep the editor regression aligned with the SHIFT+VARS navigation menu."""

from pathlib import Path

TEST = Path(__file__).with_name("pyeditor_regression.py")
OLD = "    assert seen == ['Style', 'Symbols']\n"
NEW = "    assert seen == ['SHIFT VARS', 'Symbols']\n"

text = TEST.read_text(encoding="utf-8")
if NEW in text:
    print("pyeditor regression already expects SHIFT VARS")
elif OLD in text:
    TEST.write_text(text.replace(OLD, NEW, 1), encoding="utf-8")
    print("Updated pyeditor regression for SHIFT VARS")
else:
    raise SystemExit("Unable to locate stale SHIFT VARS regression expectation")
