#include "global.h"

void abort(const char *format, ...) {
	if (!DEBUG)
		return;

	va_list argptr;
	va_start(argptr, format);
	vfprintf(stderr,format, argptr);
	fprintf(stderr,"\n");
	exit(1);
}

void alert(const char *format, ...) {
	if (!DEBUG)
		return;
	va_list argptr;
	va_start(argptr, format);
	vprintf(format, argptr);
	printf("\n");
}
/* thanks dradtke */
