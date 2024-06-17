#if defined(_WIN32) || defined(_WIN64)
    #define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "err.h"

bool        allocated = false;
char        *err = NULL;

    bool
err_new(const char *p_errstr, bool quit) {
    if (quit)
        exit(1);

    size_t tlen = strlen(p_errstr) + 2;

    if (!allocated) {
        err = (char*) malloc(tlen);
        if (err == NULL)
            return false;
        allocated = true;
        err[0] = '\0';
    } else
        err = (char*) realloc(err, sizeof(err) + tlen);

    strcat(err, p_errstr);
    strcat(err, "\n");

    return true;
}

    void
err_log_file(const char *p_fpath) {
    
}

    void
err_log(void) {
    if (err != NULL)
        printf("%s", err);
}
