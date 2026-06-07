# Authoring Conventions

Rules for editing this vault so the generated outputs stay correct.

## Encoding & characters

- Save every page as **UTF-8** (Obsidian's default). Never hand-edit the
  generated `_build/help.txt` -- it is Latin-1 and is overwritten on each build.
- Accented characters (`ñ á é í ó ú ¿ ¡`) are fine in UTF-8 here; the generator
  transcodes them to Latin-1 for `help.txt`. Avoid characters outside Latin-1
  in help topics (e.g. `—` em-dash, `→` arrow, curly quotes) -- they cannot be
  represented in `help.dat`. Use ASCII `--`, `->`, `"` in `es/ayuda/` pages.
  (Manual `.docx` pages may use full Unicode.)

## Frontmatter

Every page starts with a YAML block.

Manual chapter page:
```yaml
---
title: "Menú de Configuración"
lang: es            # es | en
manual: guia-usuario-es
order: 6            # chapter number (also encoded in the NN- filename)
---
```

Help topic page (`es/ayuda/`):
```yaml
---
topic: H_CONFIG_MENU   # MUST match the Zinc help anchor used in the source
title: "Menú Configuración del Sistema"   # shown as the help window title
lang: es
order: 8               # position in the generated help.txt
---
```

## Help topic body

- One Markdown line per logical line; a blank line is a paragraph break. This
  maps 1:1 to the Zinc `\` line model on build.
- Keep the `topic:` value identical to the `H_*` id referenced from the C/UI
  code -- the app looks help up by that anchor.
- Do not add Markdown headings inside a help topic; the title comes from
  frontmatter and Zinc renders the body as flowing text.

## Adding a manual chapter

1. Create `NN-slug.md` in the manual folder (the `NN` prefix sets order).
2. Add frontmatter + a single `# Title` H1.
3. Add it to that manual's `index.md` contents list as `[[NN-slug|Title]]`.

## Adding a help topic

1. Create `es/ayuda/H_NEW_TOPIC.md` with the frontmatter above.
2. Pick an `order:` value for its slot in `help.txt`.
3. Rebuild and confirm the topic appears.

## Links

- Use Obsidian wikilinks. For `index.md` targets (non-unique name) use a path:
  `[[es/manual-usuario/index|Guía del Usuario]]`.
- Cross-reference chapters by their unique `NN-slug` filename.

## After editing — rebuild & verify

```sh
./build-docs.sh
```
Then sanity-check `_build/` (open a `.docx`, diff `help.txt`).
