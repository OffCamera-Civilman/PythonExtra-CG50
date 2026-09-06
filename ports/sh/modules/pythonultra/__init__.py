"""On-calculator PythonUltra module and method catalog."""

__version__ = "0.1.0-cg50"

_CATALOG = (
    ("numpy",
     "array ndarray matrix mat asarray asmatrix zeros ones full identity eye "
     "arange linspace reshape transpose concatenate dot cross norm normalize "
     "lerp matmul sqrt sin cos tan abs sum mean amin amax"),
    ("pygame",
     "Color Rect Surface display draw event key time font image transform "
     "sprite init quit"),
    ("py3d",
     "Renderer cube vec3 add sub scale dot cross length normalize identity "
     "matmul4 compose translation scaling rotation_x rotation_y rotation_z "
     "transform focal_length project rgb565 shade_rgb565 prepare_triangles"),
    ("gint",
     "dclear dupdate dtext dline drect dcircle dellipse dtriangle getkey "
     "keydown image_rgb565"),
    ("ctypes",
     "c_int c_uint c_float c_double c_bool buffer read_u8 write_u8 fill copy "
     "available call"),
    ("os", "listdir mkdir remove unlink rename rmdir stat sep"),
    ("json", "dumps loads dump load"),
    ("math", "sqrt sin cos tan asin acos atan atan2 floor ceil exp log pow pi e"),
    ("random", "random randint randrange choice getrandbits seed"),
    ("time", "time sleep sleep_ms sleep_us ticks_ms ticks_us ticks_diff"),
    ("sys", "path modules version implementation platform"),
    ("collections", "deque namedtuple OrderedDict"),
    ("array", "array"),
    ("io", "StringIO BytesIO FileIO"),
    ("struct", "pack unpack calcsize"),
    ("casioplot", "set_pixel get_pixel draw_string clear_screen show_screen"),
    ("kandinsky", "color set_pixel get_pixel draw_string fill_rect"),
    ("ion", "keydown"),
)


def modules():
    """Return PythonUltra's curated module names."""
    return tuple(item[0] for item in _CATALOG)


def catalog(name=None):
    """Print the built-in PythonUltra catalog or one module entry."""
    if name is None:
        print("PythonUltra module catalog")
        print("Use F3 to reopen. SHIFT+UP/DOWN scrolls.")
        for module, methods in _CATALOG:
            print(module + ": " + methods)
        print("For details: import module; help(module)")
        return None

    wanted = str(name)
    for module, methods in _CATALOG:
        if module == wanted:
            print(module + ": " + methods)
            return methods
    print("Unknown catalog module: " + wanted)
    print("Available: " + " ".join(modules()))
    return None
