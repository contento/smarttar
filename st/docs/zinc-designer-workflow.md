# Zinc Designer / RES.DAT workflow

> **DRAFT — not yet verified by Gonzalo.** This was drafted from repo
> inspection (MAKEFILE rules, `st/bin/mvres.bat`, `zinc/BIN/` contents,
> file layout). Steps marked **`FIXME`** are best-guess; correct them on
> the next pass through the workflow.

## What RES.DAT is

`RES.DAT` is the Zinc 3.5 binary resource file. It holds the application's
dialog layouts, widget definitions, string IDs, and other UI metadata.
Two artifacts are derived from it:

- `st/src/res.cpp` — generated C++ wiring (tracked source)
- `st/include/res.hpp` — generated symbol IDs (tracked header)

The canonical copy lives at [st/res/RES.DAT](../res/RES.DAT). The
MAKEFILE rule at [st/MAKEFILE:210-211](../MAKEFILE) copies it into
`st/bin/res.dat` on every build:

```make
$(BIN_DIR)\$(RES_FILE): $(RES_DIR)\$(RES_FILE)
        @copy $(RES_DIR)\$(RES_FILE) $(BIN_DIR)\$(RES_FILE) > NUL
```

So `st/res/RES.DAT` is the source of truth; `st/bin/res.dat` is a build
artifact. Edits must end up in `st/res/RES.DAT` to persist.

## Tooling

In [zinc/BIN/](../../zinc/BIN/):

| Binary | Purpose |
| ------ | ------- |
| `DESIGN.EXE` | Zinc Designer for DOS / DOS-extender hosts — **use this one under DOSBox-X** |
| `WDESIGN.EXE` | Windows-mode designer variant — not needed for our workflow |
| `BMP2DAT.EXE` | Converts `.BMP` images into the Zinc `.DAT` binary form for embedding |
| `ICO2DAT.EXE` | Same, for `.ICO` |
| `GENHELP.EXE` | Compiles `docs/help.txt` to `bin/help.dat` (unrelated to RES.DAT; called via `HELP=1`) |
| `P_DESIGN.ZNC` | Zinc Designer config / palette file. Lives alongside the EXE — leave it where it is. **`FIXME`** confirm what this actually is |

`zinc/BIN` is already on the PATH inside DOSBox-X (see
[dosbox-x.conf](../../dosbox-x.conf)), so `DESIGN` can be invoked from
anywhere.

## Workflow (best-guess — verify and correct)

1. **Build at least once** so `st/bin/res.dat` exists and matches
   `st/res/RES.DAT`. The Designer operates on `bin/res.dat`, not on the
   tracked source copy directly. **`FIXME`** or does it operate directly
   on `st/res/RES.DAT`?
2. **Open the Designer from `st/bin/`** inside DOSBox-X:

   ```cmd
   cd \st\bin
   DESIGN res.dat
   ```

   **`FIXME`** confirm: is the filename argument needed, or does it pick
   up `res.dat` from CWD automatically? Does it need to be launched from
   `st/bin/` specifically, or any directory?
3. **Edit** dialogs, widgets, IDs, strings inside the Designer GUI.
   Mouse comes from DOSBox-X's INT 33h emulation (see
   [dosbox-x.conf](../../dosbox-x.conf) `mouse_emulation = integration`)
   — do **not** load `util/MOUSE/MOUSE.EXE` on top.
4. **Save.** The Designer writes:
   - `res.dat` (updated binary)
   - `res.cpp` (regenerated C++ wiring)
   - `res.hpp` (regenerated ID symbols)

   **`FIXME`** confirm output filenames + that all three are written
   together, and into the same directory the Designer was launched from.
5. **Move the generated sources back into the tree.** From `st/bin/`:

   ```cmd
   mvres
   ```

   [st/bin/mvres.bat](../bin/mvres.bat) moves `res.hpp` to
   `..\include` and `res.cpp` to `..\src` (overwriting the tracked
   copies). It does **not** touch `res.dat`.
6. **Copy `res.dat` back to the canonical location:**

   ```cmd
   copy res.dat ..\res\RES.DAT
   ```

   This is the easy step to forget. Without it, the next build will
   copy the **old** `st/res/RES.DAT` over your edited `st/bin/res.dat`
   and your work is lost.

   **`FIXME`** is there an automated step for this? Should `mvres.bat`
   be extended to do it?
7. **Rebuild.** `make` picks up the new `res.dat`, `res.cpp`, and
   `res.hpp`. The `res.obj` rule at
   [st/MAKEFILE:312](../MAKEFILE) recompiles `src/res.cpp` and
   relinks `st.exe`.
8. **Commit** all four files together so the binary and generated
   sources stay in sync:

   ```bash
   git add st/res/RES.DAT st/src/res.cpp st/include/res.hpp
   git commit -m "RES.DAT: <describe the UI change>"
   ```

## Gotchas

- **Encoding inside dialogs.** Strings rendered by Zinc are **Latin-1**,
  not CP850 — see the encoding section of [CLAUDE.md](../../CLAUDE.md).
  If you type `ñ` / `ó` / `á` etc. in the Designer, they end up in
  `RES.DAT` as Latin-1 bytes and Zinc draws them correctly via its
  bitmap font. (Console/printer output uses CP850 and is bridged via
  `_ISO2ASCII()` in `st_util.cpp`.) **`FIXME`** confirm — does the
  Designer's text-input field use the DOS code page or Latin-1?
- **Don't run the Designer while `st.exe` is open** in the same DOSBox-X
  instance. **`FIXME`** verify whether this is actually a constraint.
- **`P_DESIGN.ZNC`** sits in `zinc/BIN/` — leave it there. Moving or
  deleting it may break the Designer's startup config. **`FIXME`**
  describe what it actually does.

## Related references

- [st/MAKEFILE](../MAKEFILE) — RES.DAT copy rule (`$(BIN_DIR)\$(RES_FILE)`)
- [st/bin/mvres.bat](../bin/mvres.bat) — moves generated `res.cpp` /
  `res.hpp` back into the tree
- [CLAUDE.md](../../CLAUDE.md) — Latin-1 vs CP850 encoding policy
- [dosbox-x.conf](../../dosbox-x.conf) — mouse/PATH setup that the
  Designer relies on
