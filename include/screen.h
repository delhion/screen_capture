#ifndef SCREEN_H
#define SCREEN_H

struct Screen;

struct Screen *create_screen(struct Screen *);
void free_screen(struct Screen *);

int capture_screen(struct Screen *);
int save_screen(struct Screen *, const wchar_t *);

#endif
