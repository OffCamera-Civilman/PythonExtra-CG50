"""Fetch the pinned PicoC source used by PythonUltra builds.

The vendor tree is generated during the build instead of committing a large
third-party source copy.  PicoC is BSD-3-Clause licensed; the upstream LICENSE
is retained inside the generated vendor directory.
"""

from pathlib import Path
import io
import shutil
import tarfile
import urllib.request

COMMIT = "a97d94fa3d4d35c6b78b7de69faac7643e34de22"
URL = "https://codeload.github.com/jpoirier/picoc/tar.gz/" + COMMIT
ROOT = Path(__file__).resolve().parent
DEST = ROOT / "vendor" / "picoc"
MARKER = DEST / ".pythonultra_picoc_commit"

REQUIRED = (
    "LICENSE", "picoc.h", "interpreter.h", "platform.h",
    "table.c", "lex.c", "parse.c", "expression.c", "heap.c", "type.c",
    "variable.c", "clibrary.c", "platform.c", "include.c", "debug.c",
    "cstdlib/stdio.c", "cstdlib/math.c", "cstdlib/string.c",
    "cstdlib/stdlib.c", "cstdlib/time.c", "cstdlib/errno.c",
    "cstdlib/ctype.c", "cstdlib/stdbool.c", "cstdlib/unistd.c",
)


def ready():
    if not MARKER.exists() or MARKER.read_text().strip() != COMMIT:
        return False
    return all((DEST / name).exists() for name in REQUIRED)


def main():
    if ready():
        print("PicoC source already pinned at", COMMIT[:12])
        return

    if DEST.exists():
        shutil.rmtree(DEST)
    DEST.mkdir(parents=True)

    print("Fetching PicoC", COMMIT[:12])
    with urllib.request.urlopen(URL, timeout=60) as response:
        payload = response.read()

    prefix = "picoc-" + COMMIT + "/"
    wanted = set(REQUIRED)
    with tarfile.open(fileobj=io.BytesIO(payload), mode="r:gz") as archive:
        for member in archive.getmembers():
            if not member.isfile() or not member.name.startswith(prefix):
                continue
            rel = member.name[len(prefix):]
            if rel not in wanted:
                continue
            target = DEST / rel
            target.parent.mkdir(parents=True, exist_ok=True)
            source = archive.extractfile(member)
            if source is None:
                raise RuntimeError("missing PicoC archive member " + rel)
            target.write_bytes(source.read())

    missing = [name for name in REQUIRED if not (DEST / name).exists()]
    if missing:
        raise RuntimeError("PicoC archive missing: " + ", ".join(missing))

    # The original UNIX build enables GNU readline unconditionally.  The fx-CG50
    # integration provides its own terminal/editor path, so keep PicoC's compact
    # fallback input path and avoid a readline dependency.
    platform_h = DEST / "platform.h"
    text = platform_h.read_text(encoding="utf-8")
    text = text.replace("#define USE_READLINE\n", "/* PythonUltra: readline disabled */\n")
    platform_h.write_text(text, encoding="utf-8")

    MARKER.write_text(COMMIT + "\n", encoding="utf-8")
    print("Prepared PicoC source in", DEST)


if __name__ == "__main__":
    main()
