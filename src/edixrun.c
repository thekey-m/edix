/*
Includes
*/
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "def.h"
#include "edix.h"
#include "err.h"

// Parse the command line arguments
    static char*
_getcc(int argc, char* argv[]) {
    size_t tlen = 0;

    for (int i = 0; i < argc; ++i)
        tlen += strlen(argv[i]) + 1;

    char* args = (char*) malloc(tlen);
    if (!args)
        return NULL;

    size_t curpos = 0;
    for (int i = 0; i < argc; ++i) {
        size_t arglen = strlen(argv[i]);
        memcpy(args + curpos, argv[i], arglen);
        curpos += arglen;
        args[curpos++] = ' ';
    }

    args[tlen - 1] = '\0';

    return args;
}

    int
main(int argc, char* argv[]) {
    const char      *p;
    char            path[256];
    bool            inquote = false;
    bool            copy = false;
    bool            help = false;

    char *args = _getcc(argc, argv);
    p = args;

    while (*p) {
        if (*p == '"') {
            inquote = !inquote;
        } else if (!inquote && *p == L' ') {
            ++p;
            break;
        }
        ++p;
    }

    while (*p == ' ')
        ++p;

    if (p[0] == '-') {
        /*
        "-c" argument: copy a file and edit it
        */
        if (p[1] == 'c') {
            copy = true;
            p += 2;
            while(*p == ' ')
                ++p;
        }
        /*
        "-?" argument: help with edix
        */
        else if (p[1] == '?') {
            help = true;
            help_all();
            free(args);
            return 0;
        }
    }

    /*
    Get path. Can be a file or a directory
    */
    int i = 0;
    while (p[0] != '\0') {
        if (p[0] == '-')
            break;
        path[i] = (char) p[0];
        ++p;
        ++i;
    }

    if (!console_init(path))
        goto error;
    if (!console_running())
        goto error;
    if (!console_exit())
        goto error;

    err_log();

    free(args);
    return 0;

    error:
        free(args);
        return 1;
}
