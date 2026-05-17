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
#define ST_VERSION_MINOR 50
#define ST_VERSION_PATCH 0
#define ST_VERSION       "2.50.0"

/* Stringify helpers — used by callers that need "X.Y" form built from
 * the numeric MAJOR/MINOR defines (e.g. APP_VER in st_defs.h).
 * The double-indirection is the standard preprocessor trick: the inner
 * macro stringifies the literal argument, the outer one expands first.
 */
#define ST_STRINGIFY_(s) #s
#define ST_STRINGIFY(s)  ST_STRINGIFY_(s)
#define ST_VERSION_SHORT ST_STRINGIFY(ST_VERSION_MAJOR) "." ST_STRINGIFY(ST_VERSION_MINOR)

#endif /* __VERSION_H */
