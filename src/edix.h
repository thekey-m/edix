#ifndef EDIX_H
#define EDIX_H

#include <stddef.h>

#include "def.h"

void help_all(void);

bool console_running(void);
bool console_init(const char p_path[256]);
bool console_exit(void);

#endif
