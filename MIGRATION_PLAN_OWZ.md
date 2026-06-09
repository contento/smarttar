# Toolchain Portability — Migration Plan (OWZ)

> **Status:** Investigation phase — risk assessment per layer, not yet actionable.
> Branch: N/A (planning only). Current toolchain: Borland C++ 3.1 + Phar Lap 286 +
> Zinc 3.5. Builds inside DOSBox-X on any host (macOS CI confirmed working).

## Goal

Replace the proprietary toolchain (Borland C++ 3.1, Phar Lap 286 DOS-Extender,
Zinc 3.5) with open-source equivalents so the project can be built without
vendored binaries and without DOSBox-X for the compile step.

---

## Layers (independent — mix and match)

| Layer | Current | Drop-in replacement | Full replacement |
|-------|---------|-------------------|-----------------|
| **Compiler** | Borland C++ 3.1 (BCC286) | Open Watcom v2 (wcc386) | GCC / Clang (DJGPP or LLVM) |
| **DOS Extender** | Phar Lap 286 (`RUN286.EXE`, `BIND286.EXE`) | DOS/4GW (Open Watcom built-in) | CWSDPMI, HX-DOS |
| **UI Library** | Zinc 3.5 (precompiled `d16_zil.lib`, `d16_gfx.lib`) | OpenZinc (source, rebuildable) | Turbo Vision, FOX, custom BGI |

Each layer can be switched independently — a compiler swap doesn't force an
extender swap, and vice versa.

---

## Compiler: Borland C++ 3.1 → Open Watcom v2

**Reference:** https://github.com/open-watcom/open-watcom-v2
**Docs:** https://github.com/open-watcom/open-watcom-v2-wikidocs
**Setup:** https://github.com/open-watcom/setup-watcom

### Risk assessment — HIGH

| Risk | Details |
|------|---------|
| **16-bit → 32-bit model** | BCC286 targets 16-bit protected mode (286). Open Watcom targets 32-bit flat (386+). All `far`/`near`/`huge` pointers become 32-bit flat. This affects every struct, call, and extern in the codebase (~43k lines of C++). |
| **Pre-compiled headers** | `st.cfg` uses `-H=st.sym` (Borland PCH). Open Watcom uses a different mechanism; the PCH boundary (`#pragma hdrstop` in `stdst.h`) would need rework. |
| **Calling conventions** | Borland C++ `__cdecl` vs `__pascal`. Open Watcom uses `__watcall` by default. Zinc's internals use `__cdecl` explicitly in some places. |
| **Inline assembly** | `asm { ... }` Borland syntax. Open Watcom uses a different inline asm format. The ISR (`rt_isr.cpp`) has heavy inline asm for IRQ0/9/1Bh hooks. |
| **Template support** | Borland C++ 3.1 has limited, buggy templates. Open Watcom has better C++98 support but may catch template errors the old compiler missed. |

### Verdict — Not recommended as first step. The compiler swap is the highest-risk,
hardest-to-verify change. Should only be attempted after the extender is swapped
and a working EXE is linkable with the legacy libs under DOS/4GW.

---

## DOS Extender: Phar Lap 286 → DOS/4GW (Causeway / CWSDPMI)

### Risk assessment — LOW

Open Watcom ships with **DOS/4GW** as its native 32-bit extender (via `wlink
system dos4g`). The existing `st.exe` is a Phar Lap 286 bound binary —
`BIND286.EXE` wraps the Pharlap runtime around the `.EXE` and `CFIG286.EXE`
sets switches at the end of the build.

The extender swap is largely a **linker + bind step change**:

| Current | Replacement |
|---------|-------------|
| `TLINK.EXE` (Borland) → `st.exe` | `WLINK.EXE` → `st.exe` |
| `BIND286 st.exe -dll doscalls int33` | DOS/4GW binding (automatic with `system dos4g`) |
| `CFIG286 st.exe -NISTACK 10 -LDTSIZE 4096` | DOS/4GW stack config via linker directive |
| Phar Lap runtime DLLs (`DOSCALLS.DLL`, `INT33.DLL`) | DOS/4GW stub (`DOS4GW.EXE`) at front |

**Reference:** https://dosbox-x.com/wiki/DOS4GW.html (DOSBox-X docs on DOS/4GW)

### What needs changing

- **Linking:** Replace `TLINK` invocation + `BIND286` + `CFIG286` with `WLINK`
  `system dos4g`. The linker produces a DOS/4GW-bound `.EXE` directly.
- **PHAPI calls:** Phar Lap API functions (`DosAllocHuge`, `DosFreeHuge`,
  `DosGetPDB`, `DosGetvect`, `DosSetvect`, etc.) used in `stm2.cpp`,
  `st_util.cpp`, `rt_isr.cpp` need replacements. These are different APIs
  under DOS/4GW — or we use DPMI (INT 31h) calls directly.
- **Interrupt vector hooks:** Phar Lap provides `DosGetvect`/`DosSetvect`.
  DOS/4GW provides equivalent via `_dos_getvect`/`_dos_setvect` or raw INT 21h.
  The ISR code in `rt_isr.cpp` uses these for IRQ0/9/1Bh/23h/24h hooking.

### Reference: Phar Lap API vs DPMI

| Phar Lap call | DOS/4GW / DPMI equivalent |
|---------------|--------------------------|
| `DosAllocHuge(n, 0, &sel, 0, 0)` | `_dos_allocmem()` or DPMI INT 31h AX=0501h |
| `DosFreeHuge(sel)` | `_dos_freemem()` or DPMI INT 31h AX=0502h |
| `DosGetvect(vec, &handler)` | `_dos_getvect()` or DPMI INT 31h AX=0204h |
| `DosSetvect(vec, handler)` | `_dos_setvect()` or DPMI INT 31h AX=0205h |
| `DosGetPDB()` | INT 21h AH=51h |
| `outportb(port, val)` / `inportb(port)` | Standard — same under DOS/4GW |

### Verdict — Feasible. The PHAPI → DPMI mapping is mechanical. The DOS/4GW
extender is well-tested under DOSBox-X. This is the **recommended first swap**.

---

## UI Library: Zinc 3.5 → OpenZinc

**Reference:** http://www.openzinc.com/
**Differences PDF:** http://www.openzinc.com/Documentation/Differences.pdf

### Risk assessment — HIGH

| Risk | Details |
|------|---------|
| **API surface** | Zinc 3.5 `UI_*` API is used throughout the app (~40 files). OpenZinc is the same codebase but diverged after 3.5. The Differences PDF details API signatures that changed — string handling, event types, window manager methods. |
| **16-bit → 32-bit** | Zinc 3.5 has `far`/`near` throughout. OpenZinc targets 32-bit flat. The same `D_GFXDSP.CPP`, `D_BGIDSP.CPP`, etc. need recompilation for the flat model. |
| **Zinc Designer** | `RES.DAT` format may differ. Dialog layouts created with Zinc 3.5 Designer may not be readable by an OpenZinc build. |
| **Fonts** | Zinc's bitmapped fonts are compiled as `.OBJ` files in the library. OpenZinc may use a different font format. |
| **BGI dependency** | The current build uses `d16_gfx` (Zinc's own graphics), not `d16_bgi`. OpenZinc's graphics layer may differ. |

### Migration path (if pursued)

1. Compile OpenZinc from source under Open Watcom targeting DOS/4GW
2. Identify API differences from Differences.pdf
3. Patch SmartTar source for changed signatures
4. Build DEMO mode first (no hardware dependency)
5. Test UI functionality in DOSBox-X
6. Compare RES.DAT reading behavior

### Verdict — High risk, high value. The UI swap is the hardest part because
it touches every screen in the application. Should be attempted **after** the
compiler + extender swap produces a linkable `st.exe`.

---

## Recommended order of operations

### Phase 1: Extender swap (low risk, high confidence)

1. Set up Open Watcom inside DOSBox-X (separate from Borland install)
2. Write a `wl` script that links existing `.OBJ` files with DOS/4GW
3. Replace PHAPI calls with DPMI equivalents (one file at a time)
4. Verify DEMO mode links and runs under DOSBox-X with DOS/4GW

**Exit criteria:** A `st.exe` produced by Open Watcom `wlink` + DOS/4GW
that runs DEMO mode with the current Borland-compiled OBJ files.

### Phase 2: Compiler swap (high risk, incremental)

1. Port one `.cpp` file at a time — start with `cstr.cpp` (simple, no Zinc
   dependency), then `cfg.cpp` (heavy file I/O but no UI), then the RT layer
2. Compile each with `wcc386` and link into the DOS/4GW binary
3. Port Zinc-related files last — `stdst.h` includes must work first
4. Fix `far`/`near`/`huge` pointer issues as they surface

**Exit criteria:** DEMO mode builds entirely with `wcc386` + `wlink` + DOS/4GW
and runs in DOSBox-X.

### Phase 3: OpenZinc UI swap (optional, parallel to Phase 2)

1. Compile OpenZinc from source under Open Watcom
2. Identify and patch API differences
3. Build DEMO mode with OpenZinc

**Exit criteria:** DEMO mode links against OpenZinc libraries and UI renders
correctly in DOSBox-X.

---

## What to do first

The **single most informative test** is:

1. Install Open Watcom v2 inside DOSBox-X
2. Try to compile `src/cstr.cpp` (a standalone file with no Zinc/Phar Lap
   dependencies) with `wcc386 -mf -3r -bt=dos`
3. If it compiles, try linking it into a minimal DOS/4GW test

This tells us:
- Whether the Borland include paths and headers work under Open Watcom
- Whether the source has hidden Borland-isms we need to fix
- Whether the Open Watcom DOS/4GW toolchain works inside our DOSBox-X setup

### URLs

| Resource | URL |
|----------|-----|
| Open Watcom v2 source | https://github.com/open-watcom/open-watcom-v2 |
| Open Watcom v2 wiki docs | https://github.com/open-watcom/open-watcom-v2-wikidocs |
| Open Watcom 1.9 (stable) | https://github.com/open-watcom/open-watcom-1.9 |
| Open Watcom setup | https://github.com/open-watcom/setup-watcom |
| OpenZinc homepage | http://www.openzinc.com/ |
| OpenZinc Differences PDF | http://www.openzinc.com/Documentation/Differences.pdf |
| OpenZinc Documentation | http://www.openzinc.com/Documentation.html |
| Phar Lap SDK (archived) | https://archive.org/details/phar-lap-286-dos-extender |
| DOS/4GW reference | https://dosbox-x.com/wiki/DOS4GW.html |
