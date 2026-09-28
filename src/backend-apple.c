/*
 * ODK Runner
 * Copyright (C) 2026 Damien Goutte-Gattat
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "backend-apple.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "util.h"
#include "backend-docker.h"

#if defined(ODK_RUNNER_MACOS)

static int
prepare(odk_backend_t *backend, odk_run_config_t *cfg)
{
    (void) backend;

    if ( (cfg->flags & ODK_FLAG_RUNASROOT) == 0 ) {
        odk_add_env_var(cfg, "ODK_USER_ID", "1000", 0);
        odk_add_env_var(cfg, "ODK_GROUP_ID", "1000", 0);
    }

    return 0;
}

static int
run(odk_backend_t *backend, odk_run_config_t *cfg, char **command)
{
    int rc;
    char **argv;
    mem_registry_t mr = { 0 };

    (void) backend;

    argv = odk_backend_docker_build_command(&mr, cfg, command);
    argv[0] = "container";
    rc = spawn_process(argv);
    mr_free(&mr);

    return rc;
}

static int
close_backend(odk_backend_t *backend)
{
    (void) backend;

    return 0;
}

static int
get_total_memory(odk_backend_info_t *info)
{
    FILE *p;
    int ret = -1;

    if ( (p = popen("container system property ls", "r")) != NULL ) {
        char line[128];
        ssize_t n;
        int memory, container_section = 0;

        /*
         * Contrary to `docker info`, `container system property ls`
         * does not allow to query an individual setting, so we need to
         * parse the entire output.
         */
        while ( ! feof(p) ) {
            if ( (n = get_line(p, line, sizeof(line))) > 0 ) {
                if ( line[0] == '[' ) {
                    container_section = strcmp(line, "[container]") == 0;
                }
                else if ( container_section && sscanf(line, "memory = \"%dgb\"", &memory) == 1 )
                    info->total_memory = (long) memory * 1024 * 1024 * 1024;
            }
        }
        pclose(p);
        ret = 0;
    }

    return ret;
}

#endif /* ODK_RUNNER_MACOS */

int
odk_backend_apple_init(odk_backend_t *backend)
{
#if !defined(ODK_RUNNER_MACOS)
    errno = ENOSYS;
    return -1;

#else
    int ret;

    backend->prepare = prepare;
    backend->run = run;
    backend->close = close_backend;

    ret = get_total_memory(&(backend->info));

    return ret;
#endif
}
