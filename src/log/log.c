#include "cos_graphics/log.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void CGPrint(const char* sender, const char* type, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    printf("[%s] <%s>: ", sender, type);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
}

void CGPrintWithFunction(const char* sender, const char* type, const char* function_name, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    printf("[%s] <%s>, function %s: ", sender, type, function_name);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
}