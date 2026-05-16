# Releasing SmartTar

## Versioning policy

SmartTar follows [Semantic Versioning](https://semver.org/):

- **PATCH** (`X.Y.Z+1`): bug fixes, internal stability work, doc edits, build / tooling tweaks. **Most releases.**
- **MINOR** (`X.Y+1.0`): new visible features, new fields in saved data, new printer drivers. Rare per the stability-only mandate in [CLAUDE.md](CLAUDE.md).
- **MAJOR** (`X+1.0.0`): breaking on-disk format changes, dropping a printer driver, dropping a build variant. Very rare.

The historical `Build N` counters under each version in [`st/versions.txt`](st/versions.txt) (e.g. `Build 5`) are out-of-band patch numbers from before semver was adopted; from now on, bump the patch instead.

## Where version lives

- [`st/versions.txt`](st/versions.txt) — changelog, source of truth for *content*
- [`CLAUDE.md`](CLAUDE.md) line 17 — single-line `Current version:` reference
- Git tags `vX.Y.Z` — release markers

A `ST_VERSION` runtime constant doesn't exist yet — see the maintenance milestone in [TODO.md](TODO.md). Until it does, `versions.txt` and `CLAUDE.md` are the only places carrying the number.

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

Both scripts do exactly two things:

- Update `CLAUDE.md` line 17 (`Current version: **X.Y.Z**`)
- Prepend a new `[ X.Y.Z ]` block to `st/versions.txt` with a `TODO add description.` stub

They do **not** commit, tag, or push — that's step 3.

### 3. Write the changelog

Open `st/versions.txt`, find the new `[ X.Y.Z ]` block at the top, and replace `TODO add description.` with the real summary. Follow the existing format (one bullet per change, prefixed by ` - <Mon DD YYYY>: `).

### 4. Commit, tag, push

```sh
git add CLAUDE.md st/versions.txt
git commit -m "Release v2.34.2: <one-line summary>"
git tag -a v2.34.2 -m "$(git log -1 --format=%s)"
git push origin main v2.34.2
```

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

- `st/web/versions.txt` is a duplicate of `st/versions.txt`. The bump scripts touch only the canonical one — decide whether to delete the duplicate, keep it as a separate web-only changelog, or extend the scripts to sync both.
- No runtime `ST_VERSION` constant exists yet (see TODO.md). Adding `st/include/version.h` and an `About` dialog hook would let the app self-identify.
