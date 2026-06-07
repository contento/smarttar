# SmartTar Documentation Vault

This folder is an **Obsidian vault** and the **single source of truth** for all
SmartTar end-user documentation. The manuals (`.docx`) and the in-app help
(`help.txt` -> `help.dat`) are **generated outputs** built from the Markdown
pages here -- do not edit those outputs directly; edit the Markdown and rebuild.

Open this folder as a vault in Obsidian. Start at [[index]].

## Layout

```
es/                         Spanish content
  manual-usuario/           User Guide  (one page per chapter + index.md)
  manual-referencia/        Reference Manual
  ayuda/                    In-app Zinc help topics (one file per H_* topic)
  stc/                      SmartTar Communicator (STC) companion doc
en/                         English content (mirror of es/)
  users-guide/  reference-manual/  help/
assets/                     Screenshots
tools/                      Conversion scripts (see below)
manifest.json               Maps vault pages -> generated outputs
build-docs.sh               Regenerates every output into _build/
_build/                     Generated outputs (gitignored)
```

## Outputs and how they are generated

| Output | Source pages | Generator |
| ------ | ------------ | --------- |
| `SmartTar_Guia_del_Usuario_ES.docx` | `es/manual-usuario/*.md` | pandoc |
| `SmartTar_Manual_de_Referencia_ES.docx` | `es/manual-referencia/*.md` | pandoc |
| `SmartTar_Users_Guide_EN.docx` | `en/users-guide/*.md` | pandoc |
| `SmartTar_Reference_Manual_EN.docx` | `en/reference-manual/*.md` | pandoc |
| `SmartTar_STC_ES.docx` | `es/stc/index.md` | pandoc |
| `help.txt` (-> `help.dat`) | `es/ayuda/*.md` | `tools/md2help.py` |

Chapter order for the `.docx` manuals comes from the `NN-` filename prefix.
`help.txt` order comes from the `order:` frontmatter on each topic page.

## Building

```sh
./build-docs.sh        # writes everything into _build/
```

Requires `pandoc` and `python3` on the host (this is a host-side build, not the
DOS toolchain). Outputs go to `_build/` so you can diff against the originals in
`st/docs/` before promoting them. To ship a new in-app help:

```sh
cp _build/help.txt ../help.txt        # promote
cd ../.. && ./build.sh                # rebuilds; genhelp compiles help.txt -> help.dat
```

`build.sh` defaults to the `demo` variant, so no argument is needed, and
`help.dat` is rebuilt unconditionally by the MAKEFILE (there is no `HELP=1`
flag to pass).

## Encoding (important)

Vault pages are **UTF-8** -- the fragile Latin-1/CP850 concern from the C
sources does **not** apply while authoring here. Transcoding happens only in the
generators:

- `help.txt` is emitted as **ISO-8859-1 (Latin-1)** -- the encoding Zinc expects
  for `help.dat` (Zinc draws via its own bitmap font; this is *not* CP850).
- `.docx` is UTF-8 internally (pandoc handles it).

See [[CONVENTIONS]] for authoring rules.

## Tools

| Script | Purpose |
| ------ | ------- |
| `tools/helpfmt.py` | Shared parser/renderer for the Zinc help format |
| `tools/help2md.py` | One-time: `help.txt` -> `es/ayuda/*.md` (already run) |
| `tools/md2help.py` | Build: `es/ayuda/*.md` -> `help.txt` (Latin-1) |
| `tools/split_manual.py` | One-time: pandoc-converted manual -> chapter pages |

The `help.txt` <-> Markdown round-trip is loss-free at the logical-line level
(verified for all 48 topics).
