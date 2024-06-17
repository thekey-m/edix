#ifndef EDIX_ERR_H
#define EDIX_ERR_H

#include "def.h"

bool err_new(const char *p_errstr, bool quit);
void err_log_file(const char *p_fpath);
void err_log(void);

#endif
