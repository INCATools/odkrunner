/*
 * ODK Runner
 * Copyright (C) 2025 Damien Goutte-Gattat
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


/*
 * Definitions of per-user directories.
 *
 * Each ODK_USERDIR macro below take the following 4 parameters:
 * - an identifier for the type of per-user directory:
 *     - CONFIG: directory for configuration file,
 *     - DATA: directory for application data,
 *     - ENVS: directory for ODK environments (see below);
 * - the primary environment variable to use to get the directory;
 * - the fallback environment variable;
 * - the path to append to the directory pointed to by the fallback
 *   variable.
 */
#if defined(ODK_RUNNER_LINUX)
/* On GNU/Linux we always check the Freedesktop.org variables first. */
ODK_USERDIR(CONFIG, "XDG_CONFIG_HOME",  "HOME",     "/.config")
ODK_USERDIR(DATA,   "XDG_DATA_HOME",    "HOME",     "/.local/share")
ODK_USERDIR(ENVS,   "XDG_DATA_HOME",    "HOME",     "/.local/share")

#elif defined(ODK_RUNNER_MACOS)
/*
 * We cannot use ~/Library/Preferences for configuration files because
 * Apple docs are clear that this directory must never be accessed
 * directly -- only through the Preferences API.
 *
 * We put the environments under ~/.local/share (since we cannot have
 * them in "~/Library/Application Support" because of the space
 * character), but since this is a XDG directory we do check the
 * corresponding XDG_DATA_HOME variable first; it is not supposed to be
 * set on macOS, but if the user did explicitly set it themselves to
 * something else than ~/.local/share, we must assume they don't wont
 * anything (or already have something else) at that place.
 */
ODK_USERDIR(CONFIG, NULL,               "HOME",     "/Library/Application Support")
ODK_USERDIR(DATA,   NULL,               "HOME",     "/Library/Application Support")
ODK_USERDIR(ENVS,   "XDG_DATA_HOME",    "HOME",     "/.local/share")

#elif defined(ODK_RUNNER_WINDOWS)
/* For Window everything goes under LOCALAPPDATA. */
ODK_USERDIR(CONFIG, "LOCALAPPDATA",     NULL,       "")
ODK_USERDIR(DATA,   "LOCALAPPDATA",     NULL,       "")
ODK_USERDIR(ENVS,   "LOCALAPPDATA",     NULL,       "")

#endif
