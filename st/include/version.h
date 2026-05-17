/*
 * [ VERSION.H ]
 *
 * Single source of truth for the SmartTar version.
 *
 * Edit via bump-version.sh / bump-version.ps1 (see RELEASING.md).
 * Those scripts keep this file, CLAUDE.md, and st/versions.txt in lockstep.
 * Do NOT edit by hand unless you also fix the other two — git diff will
 * show them out of sync.
 *
 * Borland MAKE's .autodepend picks up #include of this header, so any .cpp
 * that consumes ST_VERSION will be recompiled when the version bumps.
 */

#ifndef __VERSION_H
#define __VERSION_H

#define ST_VERSION_MAJOR 2
#define ST_VERSION_MINOR 34
#define ST_VERSION_PATCH 2
#define ST_VERSION       "2.34.2"

#endif /* __VERSION_H */
