# PythonUltra Roadmap

This roadmap tracks the current fx-CG50 hardware-tested goals for PythonUltra.

## P0 - Hardware safety and correctness

- Fix filesystem operations that can hard-reset the calculator.
  - `os.listdir()` / terminal `ls`
  - folder rename/move
  - mkdir/remove/rmdir/stat
  - ZIP extraction
  - all affected calls must respect the fx-CG50/gint filesystem world-switch boundary.
- Fix Editor keyboard input.
  - ALPHA must enter/toggle letters.
  - SHIFT must reach the editor as a modifier.
  - filename/search input must use the same reliable input path.
- Fix editor syntax parser compatibility with this MicroPython build (avoid unavailable CPython-only string helpers).
- Fix QSTR punctuation rendering so symbols/help separators display as real punctuation rather than names such as `_hyphen_`.
- Keep every downloadable G3A named with its GitHub Actions run number: `PythonUltra-CG50-runNNN.g3a`.

## P1 - Files: Utilities-inspired feature set

Use the proven behavior and workflow of gbl08ma's Utilities v2.1 as a UX reference, but reimplement functionality in PythonUltra code. Do not copy GPLv2 source directly into the current PythonUltra codebase without an explicit project licensing decision.

### Core file manager

- Files is the default startup screen.
- Create files of arbitrary extensions, not only `.py`.
- Create folders.
- Rename files/folders safely.
- Delete files and empty folders safely.
- Multi-select.
- Cut / copy / paste files and folders.
- Search by filename; later add content/recursive search if memory allows.
- Sort by name/type/size.
- File information screen.
- Storage-free-space display.
- Permissions UI for PythonUltra's rwx model.

### File information screen

For the selected item show:

- filename
- full path
- file type
- size
- PythonUltra rwx permissions
- modification time/date when available
- actions: Open / Edit / Run (for Python) / Copy / Move / Delete / Compress / Extract
- checksum tools:
  - SHA-256
  - SHA-1
  - MD5 if footprint is acceptable

### Compression

- Keep standard ZIP as the PythonUltra user-facing format.
- Make ZIP creation and extraction hardware-safe and recoverable.
- Never leave partial extraction state after an error when avoidable.
- Provide direct Extract action when a `.zip` is selected.
- Keep ZIP/Unzip available from OPTN/More as well.

### Image preview

- Preview supported images directly from Files.
- BMP/PPM first through existing lightweight image support.
- Add baseline JPEG preview using a memory-conscious decoder path if feasible.
- Avoid full-screen temporary buffers that can exhaust the MicroPython heap.

## P1 - Editor

- Stable ALPHA/SHIFT input.
- Programming-symbol picker for common symbols: `@ # $ % ^ & * ! > < [ ] ( ) { } | ' " : ; , - . _ + = / \\`.
- Syntax themes:
  - GitHub Dark
  - GitHub Light
  - Linux Terminal
  - PythonUltra Dark
  - PythonUltra Light
- Adjustable editor font size.
- Fix/remove JetBrains option until its generated glyph data is verified on hardware.
- New/Open/Save/Find/Run workflows.
- Edit arbitrary plain-text files.
- Undo/redo, select all, deselect, delete/clear workflows as memory permits.

## P1 - Terminal

- Linux-like commands: `ls`, `cd`, `pwd`, `mkdir`, `touch`, `rm`, `rmdir`, `mv`, `cp`, `cat`, `echo`, `clear`, `chmod`, `python`, `zip`, `unzip`, `edit`, `files`, `modules`, `info`, `version`, `alias`, `unalias`, `source`, `history`, `font`, `theme`, `rc`.
- Python help aliases:
  - `python -h`
  - `python --help`
  - `python --h`
  - `python -help`
- Script arguments through `sys.argv`.
- Persistent history file.
- `.pythonultrarc`-style configuration and aliases.
- UP/DOWN command history.
- Adjustable font size.
- Built-in man/help pages.

## P1 - UI polish

Adopt a Casio-native visual language inspired by the Geometry add-in/OS menus:

- F1-F6 tabs aligned exactly with the six physical keys.
- Each soft-key gets a fixed-width slot so labels never overlap.
- Use framed/tabbed buttons rather than free-floating text where practical.
- Match PythonUltra dark/light palettes to a clean Casio-style hierarchy.
- OPTN should visibly indicate secondary actions (for example `OPTN:More`).
- Redesign the main-menu icon for the real fx-CG50 icon-safe area: black background, centered compact mark, no artwork overlapping the OS label.

## P2 - Pygame / sprites

- Fix `Surface.set_colorkey((0, 0, 0))` behavior for BMP sprites so the selected color is treated as transparent during blits.
- Do not require source BMP files to contain an alpha channel for color-key transparency.
- Keep memory-safe image guidance for calculator-sized sprites.

## P2 - Catalog and help

- F3 Catalog shows modules.
- Selecting a module shows supported methods/functions.
- Selecting a method opens a usage page with syntax, short description, and an example.
- Ensure all punctuation displayed by help/catalog is generated safely on hardware.

## P2 - Existing Python/game-development goals

- Compact NumPy including matrix support.
- Pygame compatibility layer.
- `gint.dtriangle` filled-triangle rasterizer.
- `py3d` software 3D pipeline.
- Next py3d milestones: near-plane clipping, movable camera/view transforms, mesh loading, frustum culling, C-side performance optimization.

## Reference policy

- gbl08ma Utilities v2.1 is a feature/UX reference for the file manager, text editor and JPEG viewing behavior.
- Geometry/Casio OS menus are visual/interaction references for soft-key layout.
- PythonUltra remains its own implementation unless the project explicitly chooses a compatible licensing strategy for third-party source.
