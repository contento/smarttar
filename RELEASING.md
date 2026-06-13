# Releasing SmartTar

## Versioning policy

SmartTar follows [Semantic Versioning](https://semver.org/):

- **PATCH** (`X.Y.Z+1`): bug fixes, internal stability work, doc edits, build / tooling tweaks. **Most releases.**
- **MINOR** (`X.Y+1.0`): new visible features, new fields in saved data, new printer drivers. Rare per the stability-only mandate in [CLAUDE.md](CLAUDE.md).
- **MAJOR** (`X+1.0.0`): breaking on-disk format changes, dropping a printer driver, dropping a build variant. Very rare.

The historical `Build N` counters under each version in [`st/versions.txt`](st/versions.txt) (e.g. `Build 5`) are out-of-band patch numbers from before semver was adopted; from now on, bump the patch instead.

## Where version lives

- [`st/include/version.h`](st/include/version.h) — **canonical source of truth.** Defines `ST_VERSION_MAJOR`, `ST_VERSION_MINOR`, `ST_VERSION_PATCH`, and the string `ST_VERSION`. Borland MAKE's `.autodepend` rebuilds any `.cpp` that `#include`s this header, so bumping the version and rebuilding produces an EXE that carries the new number.
- [`st/versions.txt`](st/versions.txt) — changelog (humans). Bump scripts prepend a new `[ X.Y.Z ]` block.
- [`CLAUDE.md`](CLAUDE.md) line 17 — `Current version:` reference for project docs.
- Git tags `vX.Y.Z` — release markers.

The bump scripts keep `version.h`, `CLAUDE.md`, and `versions.txt` in lockstep. Don't hand-edit any of them — run the script.

## Release procedure

### 1. Pick the bump

```sh
# What changed since the last release tag?
git log --oneline $(git describe --tags --abbrev=0 --match 'v[0-9]*')..HEAD
```

- Only bug fixes / doc / tooling → **patch**
- New visible behavior → **minor**
- Breaks something a user relied on → **major**

### 2. Run the bump script

```sh
# bash (macOS / Linux)
./bump-version.sh patch                # 2.34.1 → 2.34.2
./bump-version.sh minor                # 2.34.1 → 2.35.0
./bump-version.sh major                # 2.34.1 → 3.0.0
./bump-version.sh set 2.34.5           # explicit version
./bump-version.sh --dry-run patch      # preview, no writes
```

```powershell
# PowerShell (Windows 11 / pwsh)
.\bump-version.ps1 patch
.\bump-version.ps1 minor
.\bump-version.ps1 major
.\bump-version.ps1 set 2.34.5
.\bump-version.ps1 patch -DryRun
```

Both scripts update three files in lockstep:

- `st/include/version.h` — numeric defines + `ST_VERSION` string
- `CLAUDE.md` line 17 (`Current version: **X.Y.Z**`)
- `st/versions.txt` — prepend a new `[ X.Y.Z ]` block with a `TODO add description.` stub

They do **not** commit, tag, or push — that's step 3.

### 3. Write the changelog

Open `st/versions.txt`, find the new `[ X.Y.Z ]` block at the top, and replace `TODO add description.` with the real summary. Follow the existing format (one bullet per change, prefixed by ` - <Mon DD YYYY>: `).

### 4. Commit, tag, push

```sh
git add st/include/version.h CLAUDE.md st/versions.txt
git commit -m "Release v2.34.2: <one-line summary>"
git tag -a v2.34.2 -m "$(git log -1 --format=%s)"
git push origin main v2.34.2
```

When the `vX.Y.Z` tag is pushed, [`.github/workflows/release.yml`](.github/workflows/release.yml) fires automatically (see [Automated release pipeline](#automated-release-pipeline) below).

## Automated release pipeline

The workflow at [`.github/workflows/release.yml`](.github/workflows/release.yml) runs on each push of a release tag matching `v[0-9]+.[0-9]+.[0-9]+` (plain semver, no suffix — checkpoint tags like `v2.34.1-runnable-by-claude` are intentionally skipped). It:

1. Checks out the repo on `windows-latest`. (DOSBox-X publishes official Windows builds and no Linux AppImage, so a Windows runner avoids building DOSBox-X from source.)
2. Clones the **`smarttar-vendor`** private repo into `vendor/` (proprietary toolchain binaries — Borland C++ 3.1, Pharlap 286, Zinc 3.5). Requires a `VENDOR_REPO_TOKEN` secret with access to the private repo.
3. Downloads the DOSBox-X **portable Windows zip** from GitHub releases at a pinned version (env `DOSBOX_X_VERSION` in the workflow file — bump it when DOSBox-X cuts a new release worth tracking). The extracted `dosbox-x.exe` path is exported as `$env:DOSBOX_X`, which `build.ps1` honors.
4. Rewrites the `mount c …` line in `dosbox-x.conf` to point at `$env:GITHUB_WORKSPACE` (the committed line points at the original author's macOS path).
5. Runs `.\build.ps1 prod`.
6. Confirms `st/bin/st.exe`, `pr_*.dll`, `help.dat`, `RES.DAT` all exist.
7. Verifies the tag matches `ST_VERSION` in `st/include/version.h` — fails the build if they disagree (catches "forgot to run `bump-version.sh`/`.ps1`" mistakes).
8. Packages `st/bin/` artifacts plus the Pharlap runtime DLLs (`vendor/pharlap/BIN/*.DLL`) into `smarttar-X.Y.Z.zip` via `Compress-Archive`.
9. Creates a GitHub Release attached to the tag, using GitHub's auto-generated release notes plus a small body block pointing at `versions.txt`.

You can also trigger a dry-run from the Actions tab via **workflow_dispatch** — that runs the same build but uploads the ZIP as a workflow artifact instead of creating a Release. **Strongly recommended before tagging a real release for the first time**, to shake down environment-specific quirks (mount paths, DOSBox-X launching headlessly under the runner's session, etc.).

## Checkpoint tags (not releases)

For non-release milestones (intermediate snapshots, working states), keep the custom-suffix convention already in the repo:

- `v2.34.1-reorg` — source reorganization snapshot
- `v2.34.1-runnable-by-claude` — first runnable build after resurrection

**Rule of thumb:** plain `vX.Y.Z` = release; `vX.Y.Z-<suffix>` = checkpoint.

## Pre-release suffixes (optional)

Standard semver pre-releases when you want to publish before final:

- `v2.35.0-rc.1`, `v2.35.0-rc.2` — release candidates
- `v2.35.0-beta`, `v2.35.0-alpha` — earlier quality bars

Tag them the same way; they sort below the final `v2.35.0` per semver precedence.

## Open items

- `ST_VERSION` is declared but not yet **consumed** anywhere in the running app. The About dialog / startup log doesn't read it. When a `.cpp` (e.g. `info.cpp`) starts `#include "version.h"`, MAKE's `.autodepend` will automatically rebuild that translation unit on every version bump — closing the sync loop.
