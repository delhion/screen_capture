#include "screen.h"
#include <stdlib.h>

struct Screen {
	BYTE *pixels;
	int width;
	int height;
	DWORD size;
};

struct Screen *create_screen(void) {
	struct Screen *screen = (struct Screen *)malloc(sizeof(struct Screen));
	if(!screen) return NULL;
	screen->pixels = NULL;
	return screen;
}

void free_screen(struct Screen *screen) {
	if(screen) {
		if(screen->pixels)
			free(screen->pixels);
		free(screen);
	}
}

int capture_screen(struct Screen *screen) {
	HDC hdc_screen, hdc_mem;
	HBITMAP hbitmap;
	HGDIOBJ  hOld;
	BITMAPINFOHEADER bi = {0};

	// full screen window
	hdc_screen = GetDC(NULL);
	if(!hdc_screen) return 1;

	// memory for window 1x1
	hdc_mem = CreateCompatibleDC(hdc_screen);
	if(!hdc_mem) {
		ReleaseDC(NULL, hdc_screen);
		return 2;
	}

	// window dimensions for full screen
	screen->width = GetSystemMetrics(SM_CXSCREEN);
	screen->height = GetSystemMetrics(SM_CYSCREEN);
	// create real memory under pixels
	hbitmap = CreateCompatibleBitmap(hdc_screen, screen->width, screen->height);
	if(!hbitmap) {
		ReleaseDC(NULL, hdc_screen);
		DeleteDC(hdc_mem);
		return 3;
	}

	// unification, connection hdc_mem->hbitmap
	// linking a context to an object
	hOld = SelectObject(hdc_mem, hbitmap);
	if(!hOld) {
		ReleaseDC(NULL, hdc_screen);
		DeleteDC(hdc_mem);
		return 4;
	}

	// copy pixels(bitmap) in memory
	// pixels are located in bitmap
	if(!BitBlt(hdc_mem, 0, 0, screen->width, screen->height, hdc_screen, 0, 0, SRCCOPY)) {
		ReleaseDC(NULL, hdc_screen);
		DeleteObject(hbitmap);
		DeleteDC(hdc_mem);
		return 5;
	}

	// struct which formats is bitmap a general view
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = screen->width;
	bi.biHeight = -screen->height;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;

	screen->size = screen->width * screen->height * 4;
	screen->pixels = malloc(screen->size);
	if(!screen->pixels) {
		ReleaseDC(NULL, hdc_screen);
		DeleteObject(hbitmap);
		DeleteDC(hdc_mem);
		return 6;
	}

	//why is this func here and not at the end ?
	// documentation GetDIBits : The bitmap identified by the hbmp...
	SelectObject(hdc_mem, hOld);

	// coping pixels(bitmap) to memory
	if(GetDIBits(hdc_mem, hbitmap, 0, screen->height, screen->pixels, (BITMAPINFO*)&bi, DIB_RGB_COLORS) == 0) {
		ReleaseDC(NULL, hdc_screen);
		DeleteObject(hbitmap);
		DeleteDC(hdc_mem);
		return 7;
	}

	DeleteObject(hbitmap);
	DeleteDC(hdc_mem);
	ReleaseDC(NULL, hdc_screen);
	return 0;
}

int save_screen(struct Screen *screen, const wchar_t *fname) {
	HANDLE hfile = NULL;
	BITMAPINFOHEADER bi = {0};
	BITMAPFILEHEADER bm_file = {0};
	DWORD written;

	// create file
	hfile = CreateFileW(fname, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hfile == INVALID_HANDLE_VALUE) { // hfale == -1
		return 1;
	}

	// struct which formats is bitmap a general view
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = screen->width;
	bi.biHeight = -screen->height;
	bi.biPlanes = 1;
	bi.biBitCount = 32;

	//
	bm_file.bfType = 0x4d42;
	bm_file.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
	bm_file.bfSize = bm_file.bfOffBits + screen->size;

	//
	WriteFile(hfile, (LPSTR)&bm_file, sizeof(BITMAPFILEHEADER), &written, NULL);
	WriteFile(hfile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &written, NULL);
	WriteFile(hfile, (LPSTR)screen->pixels, screen->size, &written, NULL);

	CloseHandle(hfile);
	return 0;
}
