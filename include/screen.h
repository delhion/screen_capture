#ifndef SCREEN_H
#define SCREEN_H

// type wchar_t
#include <stddef.h>

struct Screen;

struct Screen *create_screen(void);
void free_screen(struct Screen *);

int capture_screen(struct Screen *);
int save_screen(struct Screen *, const wchar_t *);

#endif
