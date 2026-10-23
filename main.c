#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

struct Screen {
	BYTE *pixels;
	int width;
	int height;
	DWORD size;
};

int capture_screen(struct Screen *);
int save_screen(struct Screen *, const wchar_t *);
struct Screen *create_screen(void);
void free_screen(struct Screen *);

int main(void) {
	LARGE_INTEGER tact, start, finish;
	double time;

	if(!QueryPerformanceFrequency(&tact))
		return -1;

	struct Screen *screen = create_screen();
	if(!screen) goto EXIT_ERROR;
	
	QueryPerformanceCounter(&start);
	if(capture_screen(screen) != 0) goto EXIT_ERROR;
	QueryPerformanceCounter(&finish);

	time = ((double)finish.QuadPart - (double)start.QuadPart) / (double)tact.QuadPart;
	printf("Time: %f\n", time);

	if(save_screen(screen, L"screen.bmp") != 0) goto EXIT_ERROR;

	free_screen(screen);
	return 0;

EXIT_ERROR:	
	free_screen(screen);
	return 1;
}

int capture_screen(struct Screen *screen) {
	HDC hdc_screen, hdc_mem;
	HBITMAP hbitmap;
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
	
	// unification 
	SelectObject(hdc_mem, hbitmap);
	
	// copy pixels in memory
	if(!BitBlt(hdc_mem, 0, 0, screen->width, screen->height, hdc_screen, 0, 0, SRCCOPY)) {
		ReleaseDC(NULL, hdc_screen);
		DeleteObject(hbitmap);
		DeleteDC(hdc_mem);
		return 4;
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
		return 5;
	}

	// 
	if(GetDIBits(hdc_mem, hbitmap, 0, screen->height, screen->pixels, (BITMAPINFO*)&bi, DIB_RGB_COLORS) == 0) {
		ReleaseDC(NULL, hdc_screen);
		DeleteObject(hbitmap);
		DeleteDC(hdc_mem);
		return 6;
	}
	
	DeleteObject(hbitmap);
	DeleteDC(hdc_mem);
	ReleaseDC(NULL, hdc_screen);
	return 0;
}

int save_screen(struct Screen *screen, const wchar_t *fname) {
	HANDLE hfile = NULL;
	BITMAPINFOHEADER bi = {0};
	BITMAPFILEHEADER bm_file;
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
