#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "def.h"
#include "edix.h"
#include "err.h"

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
    HANDLE                              hstdout;
    HANDLE                              hstdin;
    CONSOLE_SCREEN_BUFFER_INFO          csbi;
    CHAR_INFO                           *restore_buffer;
    COORD                               original_cursor_pos;
    COORD                               buffer_size;
    unsigned long                       mode;
    char                                path[256];
    bool                                isrun;
    
    static char*
errcode_to_str(DWORD p_code) {
    char *ret = NULL;
    int len = snprintf(NULL, 0, "%lu", p_code);

    if (len >= 0) {
        ret = (char*) malloc(len + 1);
        if (ret != NULL)
            snprintf(ret, len + 1, "%lu", p_code);
    }

    return ret;
}

    static bool
console_clear_buffer(WORD attributes) {
    COORD       coord;
    DWORD       numcells, written;

    system(SYS_CL);

    if (!GetConsoleScreenBufferInfo(hstdout, &csbi))
        goto winerr;

    numcells = csbi.dwSize.X * csbi.dwSize.Y;
    coord.X = 0;
    coord.Y = 0;
    if (!FillConsoleOutputCharacter(hstdout, ' ', numcells, coord, &written))
        goto winerr;
    if (!FillConsoleOutputAttribute(hstdout, attributes, numcells,
            coord, &written))
        goto winerr;
    if (!SetConsoleCursorPosition(hstdout, coord))
        goto winerr;

    return true;

    winerr:
        err_new(errcode_to_str(GetLastError()), false);
        return false;
}

    static bool
console_save(void) {
    if (!GetConsoleScreenBufferInfo(hstdout, &csbi))
        goto winerr;
    if (!GetConsoleMode(hstdout, &mode))
        goto winerr;

    restore_buffer = (CHAR_INFO*)
        malloc(csbi.dwSize.X * csbi.dwSize.Y * sizeof(CHAR_INFO));
    if (!restore_buffer) {
        err_new("os_win console_save() malloc(): restore_buffer", true);
        return false;
    }
    original_cursor_pos = csbi.dwCursorPosition;
    buffer_size = csbi.dwSize;
    
    SMALL_RECT console_rect = (SMALL_RECT) {
        .Left = 0,
        .Top = 0,
        .Right = csbi.dwSize.X - 1,
        .Bottom = csbi.dwSize.Y - 1
    };
    if (!ReadConsoleOutput(hstdout, restore_buffer, csbi.dwSize,
            (COORD) {0,0}, &console_rect))
        goto winerr;

    return true;

    winerr:
        err_new(errcode_to_str(GetLastError()), false);
        return false;
}

    static bool
console_restore(void) {
    if (!GetConsoleScreenBufferInfo(hstdout, &csbi))
        goto winerr;
    if (!(SetConsoleMode(hstdout, mode)))
        goto winerr;

    SMALL_RECT console_rect = (SMALL_RECT) {
        .Left = 0,
        .Top = 0,
        .Right = csbi.dwSize.X - 1,
        .Bottom = csbi.dwSize.Y - 1
    };
    SetConsoleWindowInfo(hstdout, TRUE, &console_rect);

    if (!WriteConsoleOutput(hstdout, restore_buffer, buffer_size,
            (COORD) {0,0}, &console_rect))
        goto winerr;
    if (!SetConsoleCursorPosition(hstdout, original_cursor_pos))
        goto winerr;

    free(restore_buffer);
    return true;

    winerr:
        err_new(errcode_to_str(GetLastError()), false);
        return false;
}

    static bool
console_cursor_set(short x, short y) {
    if (x < 0 || y < 0)
        return true;
    return (SetConsoleCursorPosition(hstdout, (COORD) {x,y})) != 0;
}

    static bool
console_cursor_move(short x, short y) {
    if (!GetConsoleScreenBufferInfo(hstdout, &csbi))
        return false;

    return console_cursor_set
        (csbi.dwCursorPosition.X + x, csbi.dwCursorPosition.Y + y);
}

    static void
console_handle_input() {
    INPUT_RECORD ir;
    DWORD numevents;

    ReadConsoleInput(hstdin, &ir, 1, &numevents);

    if (numevents == 0)
        return;

    if (ir.EventType == KEY_EVENT && ir.Event.KeyEvent.bKeyDown) {
        switch(ir.Event.KeyEvent.wVirtualKeyCode) {
        case VK_RETURN: // Enter
            putchar('\n');
            break;
        case VK_BACK: // Backspace
            putchar('\b');
            putchar(' ');
            putchar('\b');
            break;
        case VK_UP: // Up arrow
            if (!console_cursor_move(0, -1))
                goto winerr;
            break;
        case VK_DOWN: // Down arrow
            if (!console_cursor_move(0, 1))
                goto winerr;
            break;
        case VK_LEFT: // Left arrow
            if (!console_cursor_move(-1, 0))
                goto winerr;
            break;
        case VK_RIGHT: // Right arrow
            if (!console_cursor_move(1, 0))
                goto winerr;
            break;
        case VK_ESCAPE:
            isrun = false;
            break;
        default:
            putchar(ir.Event.KeyEvent.uChar.AsciiChar);
            break;
        }
    }

    return;

    winerr:
        err_new(errcode_to_str(GetLastError()), false);
        isrun = false;
}

    bool
console_running(void) {
    isrun = true;

    while (isrun) {
        console_handle_input();
    }

    return true;
}

    bool
console_init(const char p_path[256]) {
    if (!(hstdout = GetStdHandle(STD_OUTPUT_HANDLE)))
        goto winerr;
    if (!(hstdin = GetStdHandle(STD_INPUT_HANDLE)))
        goto winerr;

    if (!console_save())
        return false;
    if (!console_clear_buffer
            (FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED))
        return false;

    memset(path, '\0', sizeof(path));
    strcpy_s(path, sizeof path, p_path);

    puts(path);

    return true;

    winerr:
        err_new(errcode_to_str(GetLastError()), false);
        return false;
}

    bool
console_exit(void) {
    console_restore();

    return true;
}

#endif
