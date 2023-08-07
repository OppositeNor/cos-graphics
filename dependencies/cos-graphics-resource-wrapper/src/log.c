#include "cg_resource_wrapper/log.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void CGRWPrint(const CGRWChar* sender, const CGRWChar* type, const CGRWChar* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    CGRW_PRINTF(CGSTR("[%s] <%s>: "), sender, type);
    CGRW_VPRINTF(fmt, args);
    va_end(args);
    printf("\n");
}