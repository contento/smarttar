# SmartTar — Handoff

Status snapshot for resuming on another machine.

- **Branch:** historical notes from `feat/vendor-separation`. The repo state has since moved on.
- **`main`:** current development line is v2.98.0.
- **Vendor history cleanup:** complete. `vendor/` is no longer present in any reachable ref; the short SHAs recorded below are pre-rewrite and may no longer resolve locally.

---

## Jun 13 — Vendor separation + historical docs cleanup (this session)

### Vendor separation

Moved proprietary toolchain binaries (Borland C++ 3.1, Pharlap 286, Zinc 3.5,
DOS utilities) to a separate **private** GitHub repository to avoid copyright /
redistribution issues in the main smarttar repo.

**Private repo:** [`contento/smarttar-vendor`](https://github.com/contento/smarttar-vendor)
(PRIVATE, 1798 files + README)

What was done:
- Created `smarttar-vendor` private repo on GitHub via `gh` CLI
- Pushed all `vendor/` contents (bc/, pharlap/, zinc/, util/) to the private repo
- Added `README.md` to private repo explaining why it's private and listing components
- Created [`setup-vendor.sh`](setup-vendor.sh) / [`setup-vendor.ps1`](setup-vendor.ps1) — clone from private repo; on failure suggest Zinc 3.5, BC 3.1, Pharlap 286
- Created [`VENDOR_SETUP.md`](VENDOR_SETUP.md) — manual setup instructions, directory structure, SSH troubleshooting
- Updated [`build.sh`](build.sh) / [`build.ps1`](build.ps1) / [`run.sh`](run.sh) / [`run.ps1`](run.ps1) — check for `vendor/`, exit with clear guidance if missing
- Updated [`.gitignore`](.gitignore) — now excludes `vendor/`
- Updated [`README.md`](README.md) / [`README.es.md`](README.es.md) — vendor setup in Quick Start section
- Updated [`CLAUDE.md`](CLAUDE.md) — repository layout reflects vendor is external
- Updated [`RELEASING.md`](RELEASING.md) — vendor clone step (step 2) in release process
- Updated [`.github/workflows/release.yml`](.github/workflows/release.yml) — CI clones vendor via `VENDOR_REPO_TOKEN` secret before build
- Verified all key files present via SSH clone from private repo
- Removed nested `.git` from `vendor/` so it stays a plain directory

### Historical docs cleanup

Moved 2 planning/audit documents from repo root to [`wiki/dev/`](wiki/dev/) as historical references:
- `MIGRATION_PLAN_OWZ.md` → `wiki/dev/MIGRATION_PLAN_OWZ.md`
- `STABILITY_AUDIT.md` → `wiki/dev/STABILITY_AUDIT.md`

Updated all cross-references in [`TODO.md`](TODO.md) and [`wiki/dev/ISR_VOLATILE_NOTES.md`](wiki/dev/ISR_VOLATILE_NOTES.md).

### Commits on `feat/vendor-separation`

```
4608a41 chore: mark CI workflow and vendor clone verification as done in TODO.md
d069115 chore: mark vendor repo creation as done in TODO.md
998f552 chore: move historical docs to wiki/dev/
d5d74b6 feat: move vendor/ to private repo (smarttar-vendor) to avoid copyright issues
```

### Remaining vendor separation TODO items

- `[x]` Remove `vendor/` from this repo's git history. Completed after the
  cloned-vendor workflow was verified; current reachable history no longer
  contains `vendor/`.
- `[ ]` Build with open-source toolchains — long-term milestone (Open Watcom,
  OpenZinc), not part of vendor separation.

---

## Jun 13 — PDF receipts + config fixes (earlier session)

The PDF approach that succeeded is the one the Jun-12 lessons below pointed to:
**no `Config` struct change, no `FORM_TAG` shift.** `P_PORT="pdf"` (a config
string value, not a binary-struct field) makes `SPOOLER::Print` intercept the
printer byte stream, strip ESC/P codes, and write plain text through a small
PDF 1.4 writer (`src/pdf_wr.c` + `include/pdf_wr.h`). Output:
`bin\PDF\RXYYMMDD.pdf`, one file per day, receipts appended as pages.

### Bugs found and fixed (all verified on host with `pdfinfo`)

- **No output at all** — `fopen("PDF\\...","wb")` can't create the missing
  `PDF\` dir on DOS. Lazy `mkdir("PDF")` before open. (`c4cb8cf`)
- **Blank pages (`() Tj`)** — the ESC-strip heuristic ("skip until 0xFF")
  ate all text, because templates are ESC/P (`ESC ! n`) with no `0xFF` in the
  `Printf`/NUL-terminated path. Replaced with a command-length table matching
  `src/pr/pr_*.c`; also strip stray control bytes. (`e40973f`)
- **Malformed PDF** — `pdf_wr_close` seek clobbered `/Type`->`/Typ`; `/Length`
  stayed 0; fixed-size `/Kids` placeholder overflowed past page 1. Rewrote
  `pdf_wr.c`: Pages catalog deferred to close, `/Length` back-patched into a
  10-digit field, multi-page safe, bounds-checked. (`e40973f`)
- **Garbled filename `RX-13062.PDF`** — assumed `_GetSysDate` returns
  `YYYY-MM-DD` (it returns `DD/MM/YYYY`), and `RX-DDMMYYYY` broke 8.3. Now
  `RXYYMMDD.pdf` via the numeric date overload. (`e40973f`)
- **Missing xref/trailer** — `pdf_wr_close` only ran in `~SPOOLER`; hard/abnormal
  DOS exit left PDF unterminated. Added `pdf_wr_close` to `SPOOLER::Terminate()`.
- **Xref entry size** — entries were 21 bytes (extra space before `\r\n`);
  PDF spec requires exactly 20. Fixed in `pdf_wr.c`.
- **Garbage serial `<CAnn>`** — `g_appInfo.ShortSerial` is only decrypted when
  the EXE is serialized; a demo build isn't, so it printed raw bytes. Set
  Serial/ShortSerial to `"DEMO"` in demo mode. (`97cd8ac`)
- **MAKEFILE** — `pdf_wr.obj` had no build rule; added it. (committed)

### Multi-receipt per page

Form feed (`0x0C`) between receipts now inserts a blank line separator instead
of forcing `pdf_wr_page_break()`. Receipts stack naturally; `pdf_wr_line`
auto-wraps to a new page when full. At ~10 lines/receipt (LINEAL_80 format),
~7 receipts fit per US Letter page.

### Config changes

- `P_PORT=pdf`, `P_FORM=80 col. lineal` (LINEAL_80 — single-column, 1 line/receipt,
  cleanest for PDF). Default FORM changed from DR_80 to LINEAL_80.
- `RECNO_LABEL=Recibo`, `SHORT_SERIAL=AA52048` added to all .ini files.
- Demo mode: always uses INI `SHORT_SERIAL` (EEPROM holds raw encrypted bytes).
- Production: EEPROM serial takes priority; INI is fallback when empty.
- Documented `RECNO_LABEL`, `RECNO_DIGITS`, `RECNO_LEADING_ZEROS`, `SHORT_SERIAL`
  in both English and Spanish wiki config references.

> `util/ini2cfg/st.ini` carries the **skip-worktree** bit (local dev config).
> The committed defaults above were applied by un-setting it, committing a
> minimal diff, then re-setting it — so local runtime drift stays hidden.
- Demo mode now also unhides the **Configuración** + **Extensiones** menus
  (extended the existing `s_bDevelopment` enable to `IsDemoMode()`). Note: the
  Extensiones menu is *visible* but param editing is still gated by
  `g_extAreChangeable` (supervisor password) — make it demo-editable as a
  follow-up if wanted. (`73dfcae`)

### Still open / known limitations

- `pdf_wr_close` (writes xref + `%%EOF`) only runs in `~SPOOLER`. A hard/abnormal
  DOS exit leaves the PDF unterminated and unreadable. Verify the destructor
  runs on normal quit.
- PDF renders uniform Courier 8pt: ESC/P **font-size** codes (double-width/
  height) are stripped, so SR_80's big company-name line is not emphasized.
  Optional enhancement: teach `pdf_wr.c` + the parser to honor the `ESC ! n`
  size bits.

---

## What happened this session (Jun 12)

Started implementing a PDF printer driver (`PR_PDF.DLL`) with `WriteString` export, a `UserWriteString` callback in `SPOOLER::PrintChar`, and config wiring. Cascading failures:

1. **Config struct layout breakage** — Added `P_PDF_DIR[64]` field to the `Config` struct. The pre-built `ini2cfg.exe` binary (checked-in) writes `ST.CFG` at the old struct size. The EXE reads expecting the new struct → all fields after the insertion point (including `ENGINE_KIND`, `CLUSTERS`, `ACTIVE_CLUSTERS`, `FORM`) read from wrong offsets → garbage config → no demo, no simulation.

2. **Rebuilt `ini2cfg.exe`** — Forced rebuild from source to match struct, but then the auth step (`util_authenticate()`) failed because the auth CFG was also written by the old struct layout.

3. **Fixed by removing `P_PDF_DIR` from struct** — Reverted field, rebuilt `ini2cfg.exe` via `mk_cfg.bat`. Excluded `P_PDF_DIR` from binary config; the output directory `"PDF"` was meant to be hardcoded with directory-creation logic, but that creation code was lost in the revert and never made it into the current branch (see Jun 13 note below).

4. **`FORM_TAG` enum shift** — Adding `PDF` to the enum shifts all subsequent values. Dangerous without auditing every switch and array indexed by `FORM_TAG`.

5. **Data chain fixes mixed in** — `phones.csv`, `.inf`, `ST.CFG` copy issues are pre-existing on `main` but got interleaved with the PDF changes, making debugging harder.

**Resolution:** reverted `main` to `fc00e16` (before any PDF work). Created `feat/pdf-printer` at the same point.

---

## Lessons for the PDF branch

1. **No `Config` struct changes** — `P_PDF_DIR` doesn't go in binary `ST.CFG`. Hardcode or read from `st.ini` at runtime.
2. **Don't shift `FORM_TAG`** — Add PDF as a separate config trigger, not a new enum value.
3. **Don't touch spooler's `PrintChar` hot path** — Intercept at a higher level (`ctrl_pr.cpp` before bytes reach the queue).
4. **Ship as a complete branch** — Test the full chain before merging. No incremental patching on `main`.
5. **Keep data chain fixes separate** — `phones.csv` / `.inf` / `ST.CFG` copies are pre-existing issues on `main`, not PDF-driver problems.

---

## Current state of `main`

- v2.80.0 (merge feat/pdf-printer)
- Stability audit closed (Tier 3 ISR deferred)
- DEMO_ENGINE merged
- Zinc grid fix on `fix/zinc-grid-geometry` branch, unmerged
- **Data files not copied to `st/bin/**`: `phones.csv`, `.inf`, `PH_INFO.DAT`, `ST.CFG`, `st.ini` are missing from the runtime directory. The build doesn't deploy them. Pre-existing — not related to PDF.

Build: `./build.sh --force && ./run.sh`

---

## Other open items (from TODO.md)

- **PDF print driver** — DONE on `feat/pdf-printer`. Merged to `main` (v2.80.0).
- **Vendor separation** — DONE on `feat/vendor-separation` (see Jun 13 section).
  Private repo created, setup scripts working. Remaining: remove from git history.
- **ISR volatile audit (Tier 3)** — deferred. Parked in `wiki/dev/ISR_VOLATILE_NOTES.md`. Guard-rail: no compiler optimization without the audit.
- **Build with open-source toolchains** — long-term. See `wiki/dev/MIGRATION_PLAN_OWZ.md`.
- **Idea backlog** — PDF print driver bulk-export, text/markdown file drivers.
