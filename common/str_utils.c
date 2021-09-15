#include <stdio.h>
#include <stdlib.h>

void dealloc_str(void **str) {
    free((char *)(*str));
    *str = NULL;
}