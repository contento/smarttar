# SVGA / Higher-Resolution Display Investigation

Branch: `svga-investigation`

## Goal

Upgrade SmartTar's display from 640×480 × 16 colors (standard VGA) to a
higher resolution (800×600 or 1024×768) with more colors, using Zinc 3.5's
graphics engine capabilities.

## Current display system

The app uses **`UI_GRAPHICS_DISPLAY`** — Zinc's own graphics engine,
**not** Borland BGI (`UI_BGI_DISPLAY`). These are separate subsystems with
different library dependencies and mode-setting paths.

| Component | Library | Source |
|---|---|---|
| `UI_GRAPHICS_DISPLAY` | `d16_gfx.lib` | `vendor/zinc/SOURCE/D_GFXDSP.CPP` |
| `UI_BGI_DISPLAY` | `d16_bgi.lib` | `vendor/zinc/SOURCE/D_BGIDSP.CPP` |

The MAKEFILE links `d16_zil d16_gfx bc_16gfx` (Zinc graphics path), not
`d16_zil d16_bgi graph286` (BGI path). Key file: `st/src/st.cpp` line 87:
```cpp
UI_DISPLAY *display = new UI_GRAPHICS_DISPLAY;   // default mode = 4
```

## How resolution is set

The `UI_GRAPHICS_DISPLAY` constructor (`D_GFXDSP.CPP:36-120`):

1. Calls `AllocGFXWorkspace(16)` — allocates Zinc's graphics workspace
2. Calls `Screen(mode)` — sets the video mode via the GFX library
3. Reads `_gfx.screen_x_res` and `_gfx.screen_y_res` for the actual resolution
4. Computes `cellWidth` and `cellHeight` from font metrics

For the Pharlap build (`DOSX286` path), SVGA detection in the constructor is
**skipped** — the `if (mode >= 0x100 && IdentifySuperVGA())` branch only
compiles for real-mode builds.

## Available API in GFX library

Declared in `vendor/zinc/SOURCE/GFX/SOURCE/gfx_pro.h`. NOT all are in
the linked `d16_gfx.lib`:

| Function | In lib? | Purpose |
|---|---|---|
| `IdentifySuperVGA()` | ✅ Yes | Detects SVGA hardware, returns non-zero if present |
| `GetSuperVGAInfo()` | ✅ Yes | Returns available SVGA mode list (if SVGA detected) |
| `SetSuperVGAMode()` | ✅ Yes | Sets a VESA SVGA mode by number |
| `SetExtendedMode()` | ❌ No | Was intended to set arbitrary x/y resolution |
| `SetVideoResolution()` | ❌ No | Direct resolution setter |
| `Screen()` | ✅ Yes | Sets a standard VGA mode (0-4, 0xD-0x12) |

## What was tried

- Modified `st.cpp` to call `IdentifySuperVGA()` + `SetExtendedMode(800, 600, TRUE, 0)`
  after display creation, then update `display->columns`/`lines` if successful.
- `IdentifySuperVGA` compiled and linked fine.
- `SetExtendedMode` caused linker error: `Undefined symbol _SetExtendedMode` —
  not included in the prebuilt `d16_gfx.lib`.

## Next steps (when revisiting)

Two viable approaches to actually change the resolution:

### A) VESA mode switch via GFX library

Call `IdentifySuperVGA()` → `GetSuperVGAInfo()` → `SetSuperVGAMode()` with a
standard VESA mode number (e.g., 0x107 = 800×600×256). These functions **are**
in the library. After the hardware switch, update Zinc's display metrics:

```cpp
extern "C" {
    extern int IdentifySuperVGA(void);
    extern int GetSuperVGAInfo(int **svga_modes, int vram);
    extern int SetSuperVGAMode(int mode);
    extern struct GFX_STATUS _gfx;
}

if (IdentifySuperVGA()) {
    // VESA mode 0x107 = 800x600x256 colors
    if (SetSuperVGAMode(0x107) >= 0) {
        display->columns = _gfx.screen_x_res;
        display->lines = _gfx.screen_y_res;
        // AllocGFXWorkspace may need more planes for 256-color mode
    }
}
```

### B) SciTech VESABGI driver

Download the SciTech VESABGI driver (VESA BGI driver for Borland C++ 3.1),
link it as a static BGI driver via `BGIOBJ` + `registerbgidriver()`, and
switch Zinc to the `UI_BGI_DISPLAY` path. This would require changing the
MAKEFILE to link `d16_bgi` instead of `d16_gfx`.

### C) Custom Screen() wrapper

Patch `D_GFXDSP.CPP` to call VESA `int 0x10` functions directly instead of
relying on the GFX library's `Screen()`. This is the nuclear option —
vendor code modification with no upgrade path.

## Constraints

- **Zinc fonts are bitmapped.** Zinc ships compiled `.OBJ` font files
  (small/dialog/system) with fixed pixel sizes. At higher resolutions they
  won't scale — the cell grid just gets bigger (more cells), not cleaner
  text. True scalable fonts would need stroked font support.
- **Zinc resources (RES.DAT)** are position/coordinate-specific. Dialog layouts,
  button positions, and the booth grid are defined in Zinc Designer at fixed
  pixel offsets. At higher resolutions they would need re-layout.
- **Pharlap memory.** Higher-resolution framebuffers need more video memory.
  `AllocGFXWorkspace(16)` = 16 color planes. 256-color SVGA would need more.
  DOSBox-X config gives 32 MB extended memory.
