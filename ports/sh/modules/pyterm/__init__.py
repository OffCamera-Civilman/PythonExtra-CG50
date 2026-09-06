"""PythonUltra terminal command layer.

This module is called by the native shell when PythonUltra is in terminal mode.
It intentionally implements a small, predictable Linux-like command set for the
calculator filesystem rather than pretending to be a full GNU userland.
"""

import gc
import os
import sys

__version__ = "0.1.0-cg50"

ENTER_PYTHON = 1
CLEAR_SCREEN = 2
EXIT_APP = 3

_COMMANDS = (
    "help", "man", "ls", "cd", "pwd", "mkdir", "touch", "rm", "rmdir",
    "mv", "cp", "cat", "echo", "clear", "python", "zip", "unzip", "edit",
    "files", "modules", "info", "version",
)

_MAN = {
    "help": (
        "NAME: help - list commands or show command help",
        "SYNOPSIS: help [command]",
        "EXAMPLE: help python",
        "SEE ALSO: man",
    ),
    "man": (
        "NAME: man - PythonUltra command manual",
        "SYNOPSIS: man command",
        "EXAMPLE: man ls",
        "TIP: command -h and command --help also work",
    ),
    "ls": (
        "NAME: ls - list files and folders",
        "SYNOPSIS: ls [-l] [path]",
        "-l  show type and byte size",
        "EXAMPLE: ls -l games",
    ),
    "cd": (
        "NAME: cd - change working directory",
        "SYNOPSIS: cd [path]",
        "cd .. goes to the parent folder",
        "cd / goes to storage root",
        "EXAMPLE: cd games",
    ),
    "pwd": (
        "NAME: pwd - print working directory",
        "SYNOPSIS: pwd",
    ),
    "mkdir": (
        "NAME: mkdir - create folders",
        "SYNOPSIS: mkdir [-p] folder [folder ...]",
        "-p  create missing parent folders",
        "EXAMPLE: mkdir -p games/demo",
    ),
    "touch": (
        "NAME: touch - create a file if it does not exist",
        "SYNOPSIS: touch file [file ...]",
        "EXAMPLE: touch notes.txt",
    ),
    "rm": (
        "NAME: rm - remove files",
        "SYNOPSIS: rm file [file ...]",
        "Folders are not removed by rm; use rmdir",
        "EXAMPLE: rm old.txt",
    ),
    "rmdir": (
        "NAME: rmdir - remove empty folders",
        "SYNOPSIS: rmdir folder [folder ...]",
        "EXAMPLE: rmdir old_folder",
    ),
    "mv": (
        "NAME: mv - rename or move a file/folder",
        "SYNOPSIS: mv source destination",
        "EXAMPLE: mv old.py new.py",
    ),
    "cp": (
        "NAME: cp - copy a file",
        "SYNOPSIS: cp source destination",
        "EXAMPLE: cp game.py game_backup.py",
    ),
    "cat": (
        "NAME: cat - display text file contents",
        "SYNOPSIS: cat file [file ...]",
        "EXAMPLE: cat README.txt",
    ),
    "echo": (
        "NAME: echo - print text",
        "SYNOPSIS: echo [text ...]",
        "EXAMPLE: echo Hello PythonUltra",
    ),
    "clear": (
        "NAME: clear - clear terminal scrollback",
        "SYNOPSIS: clear",
    ),
    "python": (
        "NAME: python - enter REPL or run Python code",
        "SYNOPSIS: python",
        "          python file.py [args ...]",
        "          python -c code [args ...]",
        "          python -h | python --help",
        "python alone enters the interactive Python REPL",
        "exit or exit() returns from REPL to terminal",
        "Script arguments are available through sys.argv",
        "EXAMPLE: python hello.py Brandon 42",
        "EXAMPLE: python hello.py -h  (passes -h to script)",
    ),
    "zip": (
        "NAME: zip - compress a file or folder",
        "SYNOPSIS: zip archive.zip source",
        "Uses DEFLATE compression and standard ZIP format",
        "EXAMPLE: zip project.zip project",
    ),
    "unzip": (
        "NAME: unzip - extract a ZIP archive",
        "SYNOPSIS: unzip archive.zip [destination]",
        "EXAMPLE: unzip project.zip project_copy",
    ),
    "edit": (
        "NAME: edit - open a text file in PythonUltra Editor",
        "SYNOPSIS: edit file",
        "Works with .py, .txt, .csv, .json, .md and other text",
        "EXAMPLE: edit notes.txt",
    ),
    "files": (
        "NAME: files - open graphical PythonUltra Files",
        "SYNOPSIS: files [folder]",
        "EXAMPLE: files /",
    ),
    "modules": (
        "NAME: modules - open the PythonUltra module catalog",
        "SYNOPSIS: modules",
    ),
    "info": (
        "NAME: info - show build and collaboration information",
        "SYNOPSIS: info",
    ),
    "version": (
        "NAME: version - show PythonUltra and MicroPython version",
        "SYNOPSIS: version",
    ),
}


def commands():
    return _COMMANDS


def man(command):
    command = str(command).strip().lower()
    pages = _MAN.get(command)
    if not pages:
        print("No manual entry for " + command)
        return False
    print("PYTHONULTRA MANUAL - " + command.upper())
    print("-" * 30)
    for line in pages:
        print(line)
    return True


def _split(line):
    """Small shell-like tokenizer supporting quotes and backslash escapes."""
    result = []
    current = []
    quote = None
    escape = False
    for char in line:
        if escape:
            current.append(char)
            escape = False
        elif char == "\\":
            escape = True
        elif quote:
            if char == quote:
                quote = None
            else:
                current.append(char)
        elif char in ("'", '"'):
            quote = char
        elif char in (" ", "\t"):
            if current:
                result.append("".join(current))
                current = []
        else:
            current.append(char)
    if escape:
        current.append("\\")
    if quote:
        raise ValueError("unterminated quote")
    if current:
        result.append("".join(current))
    return result


def _is_dir(path):
    try:
        return bool(os.stat(path)[0] & 0x4000)
    except OSError:
        return False


def _mkdir_p(path):
    absolute = path.startswith("/")
    current = "/" if absolute else ""
    for part in path.split("/"):
        if not part or part == ".":
            continue
        if part == "..":
            current = current.rstrip("/")
            current = current.rsplit("/", 1)[0] if "/" in current else ("/" if absolute else "")
            continue
        if current in ("", "/"):
            current = ("/" if absolute else "") + part
        else:
            current += "/" + part
        try:
            os.mkdir(current)
        except OSError:
            try:
                if not _is_dir(current):
                    raise
            except Exception:
                raise


def _copy(src, dst):
    with open(src, "rb") as source:
        with open(dst, "wb") as target:
            while True:
                block = source.read(1024)
                if not block:
                    break
                target.write(block)


def _run_python_file(filename, args):
    old_argv = list(sys.argv)
    old_path = list(sys.path)
    try:
        sys.argv[:] = [filename] + list(args)
        folder = filename.rsplit("/", 1)[0] if "/" in filename else os.getcwd()
        if not folder:
            folder = "/"
        if folder not in sys.path:
            sys.path.insert(0, folder)
        with open(filename, "r") as source:
            code = source.read()
        scope = {"__name__": "__main__", "__file__": filename}
        exec(code, scope, scope)
    finally:
        sys.argv[:] = old_argv
        sys.path[:] = old_path
        gc.collect()


def _run_python_code(code, args):
    old_argv = list(sys.argv)
    try:
        sys.argv[:] = ["-c"] + list(args)
        scope = {"__name__": "__main__", "__file__": "<terminal>"}
        exec(code, scope, scope)
    finally:
        sys.argv[:] = old_argv
        gc.collect()


def dispatch(line):
    """Execute one terminal command and return a native-shell action code."""
    line = str(line).strip()
    if not line:
        return 0

    # Friendly no-space help shorthand requested for calculator typing.
    if line.lower().endswith("--h") and " " not in line:
        line = line[:-3] + " --help"

    try:
        argv = _split(line)
    except ValueError as exc:
        print("terminal:", exc)
        return 0
    if not argv:
        return 0

    cmd = argv[0].lower()
    args = argv[1:]

    if cmd not in _COMMANDS:
        print(cmd + ": command not found")
        print("Type 'help' for PythonUltra commands")
        return 0

    if cmd not in ("help", "man", "echo", "python") and args and args[0] in ("-h", "--help"):
        man(cmd)
        return 0

    if cmd == "help":
        if args:
            man(args[0])
        else:
            print("PythonUltra Terminal commands:")
            print(" ".join(_COMMANDS))
            print("Use: man COMMAND  or  COMMAND --help")
        return 0

    if cmd == "man":
        if not args:
            print("Usage: man command")
        else:
            man(args[0])
        return 0

    if cmd == "pwd":
        print(os.getcwd())
        return 0

    if cmd == "cd":
        os.chdir(args[0] if args else "/")
        return 0

    if cmd == "ls":
        long_form = False
        path = "."
        for arg in args:
            if arg == "-l": long_form = True
            elif arg in ("-h", "--help"):
                man("ls"); return 0
            else: path = arg
        names = list(os.listdir(path))
        names.sort()
        for name in names:
            full = name if path == "." else path.rstrip("/") + "/" + name
            if long_form:
                try:
                    st = os.stat(full)
                    kind = "d" if st[0] & 0x4000 else "-"
                    print("%s %8d %s" % (kind, st[6], name))
                except OSError:
                    print("?        ? " + name)
            else:
                print(name + ("/" if _is_dir(full) else ""))
        return 0

    if cmd == "mkdir":
        parents = False
        paths = []
        for arg in args:
            if arg == "-p": parents = True
            else: paths.append(arg)
        if not paths:
            print("mkdir: missing operand")
        for path in paths:
            if parents: _mkdir_p(path)
            else: os.mkdir(path)
        return 0

    if cmd == "touch":
        if not args:
            print("touch: missing file operand")
        for path in args:
            try:
                with open(path, "ab"):
                    pass
            except OSError as exc:
                print("touch:", path, exc)
        return 0

    if cmd == "rm":
        if not args:
            print("rm: missing file operand")
        for path in args:
            if _is_dir(path):
                print("rm: " + path + ": is a directory; use rmdir")
            else:
                os.remove(path)
        return 0

    if cmd == "rmdir":
        if not args:
            print("rmdir: missing directory operand")
        for path in args:
            os.rmdir(path)
        return 0

    if cmd == "mv":
        if len(args) != 2:
            print("Usage: mv source destination")
        else:
            os.rename(args[0], args[1])
        return 0

    if cmd == "cp":
        if len(args) != 2:
            print("Usage: cp source destination")
        else:
            _copy(args[0], args[1])
        return 0

    if cmd == "cat":
        if not args:
            print("cat: missing file operand")
        for path in args:
            with open(path, "r") as source:
                while True:
                    block = source.read(512)
                    if not block: break
                    print(block, end="")
            print()
        return 0

    if cmd == "echo":
        if args and args[0] in ("-h", "--help"):
            man("echo")
        else:
            print(" ".join(args))
        return 0

    if cmd == "clear":
        return CLEAR_SCREEN

    if cmd == "python":
        if not args:
            return ENTER_PYTHON
        if args[0] in ("-h", "--help"):
            man("python")
            return 0
        if args[0] in ("-V", "--version"):
            print("PythonUltra MicroPython " + sys.version)
            return 0
        if args[0] == "-c":
            if len(args) < 2:
                print("python -c: missing code")
            else:
                _run_python_code(args[1], args[2:])
            return 0
        _run_python_file(args[0], args[1:])
        return 0

    if cmd == "zip":
        if args and args[0] in ("-h", "--help"):
            man("zip"); return 0
        if len(args) != 2:
            print("Usage: zip archive.zip source")
        else:
            import zipfile
            archive = args[0]
            if not archive.lower().endswith(".zip"):
                archive += ".zip"
            zipfile.compress(args[1], archive)
            print("created " + archive)
        return 0

    if cmd == "unzip":
        if args and args[0] in ("-h", "--help"):
            man("unzip"); return 0
        if not args:
            print("Usage: unzip archive.zip [destination]")
        else:
            import zipfile
            destination = args[1] if len(args) > 1 else None
            print("extracted to " + zipfile.extract(args[0], destination))
        return 0

    if cmd == "edit":
        if not args:
            print("Usage: edit file")
        else:
            import pyeditor
            pyeditor.open_file(args[0])
        return 0

    if cmd == "files":
        import pyfiles
        pyfiles.browse(args[0] if args else os.getcwd())
        return 0

    if cmd == "modules":
        import pythonultra
        pythonultra.catalog_ui(True)
        return 0

    if cmd == "info":
        import pythonultra
        pythonultra.info_ui(True)
        return 0

    if cmd == "version":
        try:
            import pythonultra
            print("PythonUltra " + pythonultra.__version__ + " build " + pythonultra.BUILD_ID)
        except Exception:
            print("PythonUltra")
        print("MicroPython " + sys.version)
        return 0

    return 0
