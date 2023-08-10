#include "cos_graphics/log.h"
#include "cos_graphics/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void CGPrint(const CGChar* sender, const CGChar* type, const CGChar* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
#ifdef CG_USE_WCHAR
    CG_PRINTF(CGSTR("[%ls] <%ls>: "), sender, type);
#else
    CG_PRINTF(CGSTR("[%s] <%s>: "), sender, type);
#endif
    CG_VPRINTF(fmt, args);
    va_end(args);
    CG_PRINTF(CGSTR("\n"));
}

void CGPrintWithFunction(const CGChar* sender, const CGChar* type, const char* function_name, const CGChar* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
#ifdef CG_USE_WCHAR
    CGChar function_name_wchar[256];
    CharToCGChar(function_name, function_name_wchar, 256);
    CG_PRINTF(CGSTR("[%ls] <%ls>: in function %ls: "), sender, type, function_name_wchar);
#else
    CG_PRINTF(CGSTR("[%s] <%s>: in function %s: "), sender, type, function_name);
#endif
    CG_VPRINTF(fmt, args);
    va_end(args);
    CG_PRINTF(CGSTR("\n"));
}