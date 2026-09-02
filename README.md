# PythonUltra for fx-CG50

A working fork of PythonExtra, now built as **PythonUltra**, focused on modern Casio fx-CG50 hardware and lightweight 2D/3D game development.

## Project goals

1. Import and preserve the upstream PythonExtra source.
2. Restore display compatibility on newer fx-CG50 hardware/OS revisions.
3. Keep the interactive Python shell.
4. Add game-oriented functionality with minimal footprint.
5. Add a C-backed `dtriangle()` rasterizer to the gint-facing module.
6. Add a compact `ctypes`-style interface appropriate for the embedded MicroPython environment.
7. Add a NumPy-compatible numerical layer sized for the fx-CG50 rather than attempting to ship desktop NumPy unchanged.
8. Produce reproducible `.g3a` build artifacts with GitHub Actions.
9. Provide a compact, calculator-native Pygame compatibility layer for learning and game projects.

## Compact Pygame layer

`import pygame` is built into `PythonUltra.g3a`. The first compatibility release includes:

- display setup/update, RGB565 `Surface`, `Rect`, colors and pixel access;
- rectangle, line, polygon, circle, ellipse and arc drawing;
- calculator key events, pressed-key state and an `EXIT`-to-`QUIT` mapping;
- `pygame.time.Clock`, delay and tick timing;
- text rendering through the calculator font;
- uncompressed 24/32-bit BMP and binary PPM image loading;
- image scaling, flipping and rotation;
- `Sprite`, `Group`, `GroupSingle` and rectangle collision helpers.

Audio, networking and SDL desktop-window features are deliberately omitted. The display is always the physical fx-CG50 resolution, 396×224, even if a desktop-oriented program requests another mode.

## Development plan

- `main` — imported upstream baseline and project infrastructure.
- `cg50-modern-display` — new fx-CG50 display compatibility.
- `game-runtime` — `dtriangle()` and game-focused runtime work.
- `numeric-runtime` — compact NumPy-style and ctypes-compatible APIs.

## Upstream

Original project: **Lephenixnoir/PythonExtra** on Planet Casio.

The bootstrap workflow imports the upstream source into this repository so the rest of the work can happen on GitHub.
