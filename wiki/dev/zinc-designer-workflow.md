# Zinc Designer / RES.DAT workflow

> Verified by Gonzalo 2026-06-09. The one remaining unknown (encoding
> inside Designer dialog text fields) is marked inline.

## What RES.DAT is

`RES.DAT` is the Zinc 3.5 binary resource file. It holds the application's
dialog layouts, widget definitions, string IDs, and other UI metadata.
Two artifacts are derived from it:

- `st/src/res.cpp` — generated C++ wiring (tracked source)
- `st/include/res.hpp` — generated symbol IDs (tracked header)

The canonical copy lives at [st/res/RES.DAT](../../st/res/RES.DAT). The
MAKEFILE rule at [st/MAKEFILE:210-211](../../st/MAKEFILE) copies it into
`st/bin/res.dat` on every build:

```make
$(BIN_DIR)\$(RES_FILE): $(RES_DIR)\$(RES_FILE)
        @copy $(RES_DIR)\$(RES_FILE) $(BIN_DIR)\$(RES_FILE) > NUL
```

So `st/res/RES.DAT` is the source of truth; `st/bin/res.dat` is a build
artifact. Edits must end up in `st/res/RES.DAT` to persist.

## Tooling

In [vendor/zinc/BIN/](../../vendor/zinc/BIN/):

| Binary | Purpose |
| ------ | ------- |
| `DESIGN.EXE` | Zinc Designer for DOS / DOS-extender hosts — **use this one under DOSBox-X** |
| `WDESIGN.EXE` | Windows-mode designer variant — not needed for our workflow |
| `BMP2DAT.EXE` | Converts `.BMP` images into the Zinc `.DAT` binary form for embedding |
| `ICO2DAT.EXE` | Same, for `.ICO` |
| `GENHELP.EXE` | Compiles `st/res/help.txt` to `bin/help.dat` (unrelated to RES.DAT) |
| `P_DESIGN.ZNC` | Zinc Designer palette / config file. Lives alongside the EXE — leave it there. |

`vendor/zinc/BIN` is already on the PATH inside DOSBox-X (see
[dosbox-x.conf](../../dosbox-x.conf)), so `DESIGN` can be invoked from
anywhere.

## Workflow (verified)

1. **Build at least once** so `st/bin/res.dat` exists and matches
   `st/res/RES.DAT`. The Designer operates on `st/bin/res.dat`, not on
   the tracked source copy directly.

2. **Open the Designer from `st/bin/`** inside DOSBox-X:

   ```cmd
   cd \st\bin
   DESIGN res.dat
   ```

   Filename argument is required. Does **not** auto-detect.

3. **Edit** dialogs, widgets, IDs, strings inside the Designer GUI.
   Mouse comes from DOSBox-X's INT 33h emulation (see
   [dosbox-x.conf](../../dosbox-x.conf) `mouse_emulation = integration`)
   — do **not** load `util/MOUSE/MOUSE.EXE` on top.

4. **Save.** The Designer writes **all three files** together:
   - `res.dat` (updated binary)
   - `res.cpp` (regenerated C++ wiring)
   - `res.hpp` (regenerated ID symbols)
   All written to the current working directory (= `st/bin/`).

5. **Move the generated sources back into the tree.** From `st/bin/`:

   ```cmd
   mvres
   ```

   [st/bin/mvres.bat](../../st/bin/mvres.bat) moves `res.hpp` to
   `..\include` and `res.cpp` to `..\src` (overwriting the tracked
   copies). It does **not** touch `res.dat`.

6. **Copy `res.dat` back to the canonical location:**

   ```cmd
   copy res.dat ..\res\RES.DAT
   ```

   This is the easy step to forget. Without it, the next build will
   copy the **old** `st/res/RES.DAT` over your edited `st/bin/res.dat`
   and your work is lost. There is currently no automated step for this;
   `mvres.bat` does not touch `res.dat`.

7. **Rebuild.** `make` picks up the new `res.dat`, `res.cpp`, and
   `res.hpp`. The `res.obj` rule at
   [st/MAKEFILE:312](../../st/MAKEFILE) recompiles `src/res.cpp` and
   relinks `st.exe`.

8. **Commit** all four files together so the binary and generated
   sources stay in sync:

   ```bash
   git add st/res/RES.DAT st/src/res.cpp st/include/res.hpp
   git commit -m "RES.DAT: <describe the UI change>"
   ```

## Gotchas

- **Encoding inside dialogs** (unknown — need to test). Strings rendered
  by Zinc are **Latin-1**, not CP850 — see the encoding section of
  [CLAUDE.md](../../CLAUDE.md). If you type `ñ` / `ó` / `á` etc. in the
  Designer, they end up in `RES.DAT` and Zinc draws them via its bitmap
  font. It is **not confirmed** whether the Designer's text-input field
  uses the DOS code page (CP850) or Latin-1 directly. If it uses CP850,
  the bytes stored in `RES.DAT` will be CP850, which would need a note
  here. Test this on the next Designer session.
- **Don't run the Designer while `st.exe` is open** in the same DOSBox-X
  instance (both hold the `res.dat` file; conflicts are likely).

## Related references

- [st/MAKEFILE](../../st/MAKEFILE) — RES.DAT copy rule (`$(BIN_DIR)\$(RES_FILE)`)
- [st/bin/mvres.bat](../../st/bin/mvres.bat) — moves generated `res.cpp` /
  `res.hpp` back into the tree
- [CLAUDE.md](../../CLAUDE.md) — Latin-1 vs CP850 encoding policy
- [dosbox-x.conf](../../dosbox-x.conf) — mouse/PATH setup that the
  Designer relies on
