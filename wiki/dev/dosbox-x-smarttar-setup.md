# DOSBox-X Configuration for SmartTar

Reference document for configuring the DOSBox-X environment used to build and test SmartTar.

## Goals

- Force **MS-DOS 6.22** as the reported DOS version
- Default the video adapter to **SVGA (S3 Trio64)**
- Provide a set of **Unix-style command aliases** via `DOSKEY` for ergonomic shell use

---

## 1. DOSBox-X Configuration File

Locate the active `dosbox-x.conf`. From inside DOSBox-X, run:

```
config -wcd
```

This writes the current config to the working directory and prints its path.

On macOS the user config typically lives at:

```
~/Library/Preferences/DOSBox-X Preferences
```

Apply the following sections (merge with existing values; do not duplicate section headers).

### `[dosbox]`

```ini
[dosbox]
machine = svga_s3
memsize = 32
```

- `machine = svga_s3` — emulates an S3 Trio64 SVGA card. Supports VGA, VESA, and SVGA modes; broadly compatible with period software including Pharlap-extended applications.
- `memsize = 32` — 32 MB of RAM. Generous but harmless; helps Pharlap 286 and Borland C++ have headroom.

### `[dos]`

```ini
[dos]
ver = 6.22
ems = true
xms = true
umb = true
```

- `ver = 6.22` — `VER` reports MS-DOS 6.22; installers and version-gated tools behave accordingly.
- `ems`, `xms`, `umb` — enabled so Pharlap and TSRs can use expanded/extended memory and load high.

### `[cpu]`

```ini
[cpu]
cputype = pentium_mmx
cycles = max
```

- `cputype = pentium_mmx` — modern enough to satisfy any CPU check, still compatible with 286/386-era code.
- `cycles = max` — let the host run as fast as possible. Override per-game if timing-sensitive software misbehaves.

### `[render]`

```ini
[render]
aspect = true
```

Preserves 4:3 aspect ratio when scaling to a modern display.

### `[autoexec]`

Append (do not replace) the existing autoexec block:

```ini
[autoexec]
@echo off
mount C ~/dos/c
PATH=C:\DOS;C:\BIN;C:\PHARLAP;C:\BC;%PATH%
C:
DOSKEY /BUFSIZE=4096 /LISTSIZE=256
CALL C:\BIN\ALIASES.BAT
CLS
echo SmartTar DOS 6.22 environment ready.
echo Type ALIASES to see Unix-style command shortcuts.
```

Adjust the `mount C` path to match the actual host directory holding the SmartTar DOS tree. Adjust the `PATH` entries (`C:\PHARLAP`, `C:\BC`) to wherever Pharlap 286 and Borland C++ are installed inside the DOS image.

---

## 2. Alias Batch File

Create `C:\BIN\ALIASES.BAT` inside the DOS environment with the following contents:

```batch
@echo off
REM ============================================================
REM  SmartTar DOS 6.22 - Unix-style aliases (DOSKEY macros)
REM ------------------------------------------------------------
REM  Loaded from AUTOEXEC.BAT via: CALL C:\BIN\ALIASES.BAT
REM  Macros only persist for the current COMMAND.COM session.
REM ============================================================

REM --- Listing ---
doskey ls=dir /w /o:n $*
doskey ll=dir /o:n $*
doskey la=dir /a /o:n $*
doskey l=dir /w /o:n $*

REM --- Navigation ---
doskey ..=cd ..
doskey ...=cd ..\..
doskey ....=cd ..\..\..
doskey pwd=cd
doskey ~=cd \

REM --- File operations ---
doskey cat=type $*
doskey rm=del $*
doskey rmdir=rd $*
doskey cp=copy $*
doskey mv=move $*
doskey mkdir=md $*
doskey touch=copy nul $*
doskey clear=cls

REM --- Search (DOS built-ins; not full grep/find semantics) ---
doskey grep=find $*
doskey head=type $* $b more
doskey more=more $*

REM --- System info ---
doskey df=mem
doskey free=mem
doskey ps=mem /c /p
doskey uname=ver
doskey whoami=echo %USERNAME%

REM --- Shell helpers ---
doskey history=doskey /history
doskey h=doskey /history
doskey reload=call c:\bin\aliases.bat
doskey aliases=doskey /macros
doskey exit=exit

REM --- SmartTar shortcuts (adjust paths as needed) ---
doskey st=cd c:\smarttar
doskey stb=cd c:\smarttar\build
doskey sts=cd c:\smarttar\src
```

### Notes on the aliases

- **`$*`** forwards all arguments. **`$b`** is a pipe (`|`) inside `DOSKEY` macros.
- **`grep` -> `find`**: DOS `FIND` is not GNU grep. Syntax differs: `find "pattern" file.txt` (quotes required, no regex). This alias is a convenience, not a true replacement.
- **`ps` -> `mem /c /p`**: shows loaded modules, the closest DOS equivalent to a process list.
- **`touch`**: `copy nul filename` creates an empty file. Does not update timestamps on existing files (DOS has no native `utime`).
- Aliases only live in the current `COMMAND.COM` session. Spawning a sub-shell (e.g. from a build tool) will not inherit them unless `ALIASES.BAT` is re-invoked.
- Run `aliases` at any time to list every active macro.

---

## 3. Verification Checklist

After restarting DOSBox-X, confirm the environment with:

```
VER                    REM should report MS-DOS 6.22
MEM                    REM check conventional/UMB/XMS layout
DOSKEY /MACROS         REM should list every alias from ALIASES.BAT
ls                     REM should behave like DIR /W
```

For SVGA verification, any VESA-aware app (or `MODE CON COLS=80`) confirms the adapter is recognized. Pharlap 286 applications should detect VESA modes through INT 10h without changes.

---

## 4. Files to Create or Update

| Path | Action |
|---|---|
| `dosbox-x.conf` (host) | Merge sections from section 1 |
| `C:\BIN\ALIASES.BAT` (DOS) | Create with contents from section 2 |
| `C:\AUTOEXEC.BAT` (DOS) | Ensure it calls `C:\BIN\ALIASES.BAT` (or rely on the `[autoexec]` block in section 1) |

If `C:\BIN` does not exist on the DOS image, create it first:

```
MD C:\BIN
```

Then transfer `ALIASES.BAT` into it via the mounted host directory.
