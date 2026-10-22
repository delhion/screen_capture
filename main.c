#include <windows.h>
#include <stdlib.h>

int main(void) {
	HDC hdc_screen, hdc_mem;
	HBITMAP hbitmap;
	HANDLE hfile = NULL;
	BITMAPINFOHEADER bi = {0};
	BITMAPFILEHEADER bm_file;
	BYTE *pixels;
	DWORD size_screen, written;
	int width, height;

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
	width = GetSystemMetrics(SM_CXSCREEN);
	height = GetSystemMetrics(SM_CYSCREEN);
	// create real memory under pixels
	hbitmap = CreateCompatibleBitmap(hdc_screen, width, height);
	if(!hbitmap) {
		ReleaseDC(NULL, hdc_screen);
		DeleteDC(hdc_mem);
		return 3; 
	}
	
	// unification 
	SelectObject(hdc_mem, hbitmap);
	
	// copy pixels in memory
	if(!BitBlt(hdc_mem, 0, 0, width, height, hdc_screen, 0, 0, SRCCOPY)) {
		ReleaseDC(NULL, hdc_screen);
		DeleteObject(hbitmap);
		DeleteDC(hdc_mem);
		return 4;
	}

	// struct which formats is bitmap a general view
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = -height;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;

	size_screen = width * height * 4;
	pixels = malloc(size_screen);
	if(!pixels) {
		ReleaseDC(NULL, hdc_screen);
		DeleteObject(hbitmap);
		DeleteDC(hdc_mem);
		return 5;
	}

	// 
	if(GetDIBits(hdc_mem, hbitmap, 0, height, pixels, (BITMAPINFO*)&bi, DIB_RGB_COLORS) == 0) {
		ReleaseDC(NULL, hdc_screen);
		DeleteObject(hbitmap);
		DeleteDC(hdc_mem);
		free(pixels);
		return 6;
	}

	// create file
	hfile = CreateFileW(L"scrin.bmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hfile == INVALID_HANDLE_VALUE) { // hfale == -1
		ReleaseDC(NULL, hdc_screen);
		DeleteObject(hbitmap);
		DeleteDC(hdc_mem);
		free(pixels);
		return 7;
	}
	
	bm_file.bfType = 0x4d42;
	bm_file.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);	
	bm_file.bfSize = bm_file.bfOffBits + size_screen;

	WriteFile(hfile, (LPSTR)&bm_file, sizeof(BITMAPFILEHEADER), &written, NULL);
	WriteFile(hfile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &written, NULL);
	WriteFile(hfile, (LPSTR)pixels, size_screen, &written, NULL);

	CloseHandle(hfile);

	DeleteObject(hbitmap);
	DeleteDC(hdc_mem);
	ReleaseDC(NULL, hdc_screen);
	free(pixels);
	return 0;
}
