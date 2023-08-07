#include "cg_resource_wrapper/log.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void CGRWPrint(const CGRWChar* sender, const CGRWChar* type, const CGRWChar* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
#ifdef CGRW_USE_WCHAR
    CGRW_PRINTF(CGSTR("[%ls] <%ls>: "), sender, type);
#else
    CGRW_PRINTF(CGSTR("[%s] <%s>: "), sender, type);
#endif
    CGRW_VPRINTF(fmt, args);
    va_end(args);
    CGRW_PRINTF(CGSTR("\n"));
}