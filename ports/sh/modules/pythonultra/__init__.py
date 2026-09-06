"""PythonUltra on-calculator reference and UI helpers."""

__version__ = "0.3.1-cg50"

try:
    from ._build import BUILD_ID
except ImportError:
    BUILD_ID = "development"

_CATALOG = (
    ("builtins", ("abs", "all", "any", "bool", "bytearray", "bytes", "chr", "compile", "dict", "dir", "enumerate", "eval", "exec", "float", "format", "getattr", "hasattr", "help", "hex", "id", "input", "int", "isinstance", "iter", "len", "list", "map", "max", "min", "next", "object", "open", "ord", "pow", "print", "range", "repr", "reversed", "round", "set", "slice", "sorted", "str", "sum", "tuple", "type", "vars", "zip")),
    ("gint", ("dclear", "dupdate", "dtext", "dline", "drect", "drect_border", "dcircle", "dellipse", "dtriangle", "getkey", "pollevent", "keydown", "image_rgb565")),
    ("numpy", ("array", "ndarray", "matrix", "mat", "asarray", "asmatrix", "zeros", "ones", "full", "identity", "eye", "arange", "linspace", "reshape", "transpose", "concatenate", "dot", "cross", "norm", "normalize", "lerp", "matmul", "sqrt", "sin", "cos", "tan", "abs", "sum", "mean", "amin", "amax")),
    ("pygame", ("Color", "Rect", "Surface", "display", "draw", "event", "key", "time", "font", "image", "transform", "sprite", "init", "quit")),
    ("py3d", ("Renderer", "cube", "vec3", "add", "sub", "scale", "dot", "cross", "length", "normalize", "identity", "matmul4", "compose", "translation", "scaling", "rotation_x", "rotation_y", "rotation_z", "transform", "focal_length", "project", "rgb565", "shade_rgb565", "prepare_triangles")),
    ("ctypes", ("c_int", "c_uint", "c_float", "c_double", "c_bool", "buffer", "read_u8", "write_u8", "fill", "copy", "available", "call")),
    ("checksum", ("sha256_file", "sha1_file")),
    ("os", ("getcwd", "chdir", "listdir", "mkdir", "remove", "unlink", "rename", "rmdir", "stat", "sep")),
    ("json", ("dumps", "loads", "dump", "load")),
    ("time", ("time", "sleep", "sleep_ms", "sleep_us", "ticks_ms", "ticks_us", "ticks_diff")),
    ("math", ("sqrt", "sin", "cos", "tan", "asin", "acos", "atan", "atan2", "floor", "ceil", "exp", "log", "pow", "pi", "e")),
    ("random", ("random", "randint", "randrange", "choice", "getrandbits", "seed")),
    ("sys", ("path", "argv", "modules", "version", "implementation", "platform")),
    ("io", ("StringIO", "BytesIO", "FileIO")),
    ("struct", ("pack", "unpack", "calcsize")),
    ("array", ("array",)),
    ("collections", ("deque", "namedtuple", "OrderedDict")),
    ("binascii", ("crc32",)),
    ("deflate", ("DeflateIO", "RAW", "ZLIB", "GZIP", "AUTO")),
    ("zipfile", ("compress", "extract", "namelist")),
    ("pyterm", ("dispatch", "commands", "man")),
    ("pyeditor", ("Editor", "open_file", "new_file", "themes")),
    ("pyfiles", ("Browser", "browse")),
    ("casioplot", ("set_pixel", "get_pixel", "draw_string", "clear_screen", "show_screen")),
    ("kandinsky", ("color", "set_pixel", "get_pixel", "draw_string", "fill_rect")),
    ("ion", ("keydown",)),
)

UI_LIGHT = {"bg":0xFFFF, "fg":0x0000, "bar":0xD69A, "sel_bg":0x07E0, "sel_fg":0xFFFF, "title":0x001F}
UI_DARK = {"bg":0x1082, "fg":0xD69A, "bar":0x3186, "sel_bg":0x2148, "sel_fg":0xFFFF, "title":0x7D7C}


def modules():
    return tuple(item[0] for item in _CATALOG)


def catalog_data(name):
    for module, methods in _CATALOG:
        if module == name:
            return methods
    return ()


def catalog(name=None):
    if name is None:
        for module, methods in _CATALOG:
            print(module + ": " + " ".join(methods))
        return None
    methods = catalog_data(str(name))
    if methods:
        print(str(name) + ": " + " ".join(methods))
        return methods
    print("Unknown catalog module: " + str(name))
    return None


def popup(title, items, dark=False):
    """Display a calculator-sized modal list and return the chosen item."""
    import gint
    if not items:
        return None
    colors = UI_DARK if dark else UI_LIGHT
    selected = 0
    scroll = 0
    max_visible = 11
    while True:
        visible = min(len(items), max_visible)
        bottom = min(220, 66 + visible * 12)
        gint.drect(50, 28, 345, bottom, colors["bg"])
        gint.drect_border(50, 28, 345, bottom, colors["bar"], 2, colors["bg"])
        gint.dtext(58, 36, colors["title"], title[:34])
        if selected < scroll:
            scroll = selected
        if selected >= scroll + max_visible:
            scroll = selected - max_visible + 1
        for row in range(max_visible):
            idx = scroll + row
            if idx >= len(items):
                break
            bg = colors["sel_bg"] if idx == selected else colors["bg"]
            fg = colors["sel_fg"] if idx == selected else colors["fg"]
            gint.drect(55, 52 + row * 12, 340, 63 + row * 12, bg)
            prefix = str(row + 1) if row < 9 else ("A" if row == 9 else "B")
            gint.dtext(59, 52 + row * 12, fg, prefix + ". " + str(items[idx])[:31])
        gint.dupdate()
        key = gint.getkey().key
        if key in (gint.KEY_EXIT, gint.KEY_LEFT):
            return None
        if key in (gint.KEY_EXE, gint.KEY_RIGHT):
            return items[selected]
        if key == gint.KEY_UP:
            selected = max(0, selected - 1)
        elif key == gint.KEY_DOWN:
            selected = min(len(items) - 1, selected + 1)
        elif key == gint.KEY_ADD:
            selected = min(len(items) - 1, selected + max_visible)
        elif key == gint.KEY_SUB:
            selected = max(0, selected - max_visible)


def catalog_ui(dark=False):
    """Open the F3 module -> public methods catalog."""
    while True:
        module = popup("PythonUltra Catalog", modules(), dark)
        if module is None:
            return None
        member = popup(module, catalog_data(module), dark)
        if member is not None:
            return module, member


def info_lines():
    return (
        "PythonUltra for Casio fx-CG50",
        "Build: " + BUILD_ID,
        "MicroPython + gint + JustUI",
        "Based on PythonExtra",
        "Co-collaboration build:",
        "OffCamera-Civilman + Brandon",
        "GitHub: @brandonendall",
        "Upstream: Lephenixnoir, SlyVTT",
        "Planete Casio contributors",
        "New-display compatibility included",
        "NumPy + Pygame + py3d + Editor",
        "Files + ZIP + SHA256/SHA1",
        "Terminal + Catalog + Themes",
    )


def info_ui(dark=False):
    return popup("PythonUltra Info", info_lines(), dark)
