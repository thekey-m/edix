#include <stdio.h>
#include <conio.h>

#include "edix.h"

    void
help_all(void) {
    const char message[1024] =
        "-c : copy a file and edit the copied file\n";

    puts(message);
}
