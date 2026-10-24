#include "screen.h"
#include <stdio.h>
#include <windows.h>

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
	printf("Screen time: %f \n", time);

	if(save_screen(screen, L"screen.bmp") != 0) goto EXIT_ERROR;

	free_screen(screen);
	return 0;

EXIT_ERROR:	
	free_screen(screen);
	return 1;
}
