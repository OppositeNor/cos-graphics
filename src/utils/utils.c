#include "cos_graphics/utils.h"
#include "cos_graphics/log.h"
#include <string.h>

void CGCharToChar(const CGChar* str, char* buffer, unsigned int buffer_size)
{
    CG_ERROR_CONDITION(str == NULL, CGSTR("Cannot convert CGChar to char with string pointer \"NULL\"."));
    CG_ERROR_CONDITION(buffer == NULL, CGSTR("Cannot convert CGChar to char with buffer pointer \"NULL\"."));
    CG_ERROR_CONDITION(buffer_size == 0, CGSTR("Cannot convert CGChar to char with buffer size \"0\"."));
    CG_ERROR_CONDITION(buffer_size < CG_STRLEN(str), CGSTR("Cannot convert CGChar to char with buffer size \"%d\" and string length \"%d\"."), buffer_size, CG_STRLEN(str));
    unsigned int i = 0;
    for (; str[i] != (CGChar)'\0' && i < buffer_size; ++i)
        buffer[i] = (char)str[i];
    buffer[i] = '\0';
}

void CharToCGChar(const char* str, CGChar* buffer, unsigned int buffer_size)
{
    CG_ERROR_CONDITION(str == NULL, CGSTR("Cannot convert char to CGChar with string pointer \"NULL\"."));
    CG_ERROR_CONDITION(buffer == NULL, CGSTR("Cannot convert char to CGChar with buffer pointer \"NULL\"."));
    CG_ERROR_CONDITION(buffer_size == 0, CGSTR("Cannot convert CGChar to char with buffer size \"0\"."));
    CG_ERROR_CONDITION(buffer_size < strlen(str), CGSTR("Cannot convert CGChar to char with buffer size \"%d\" and string length \"%d\"."), buffer_size, strlen(str));
    unsigned int i = 0;
    for (; str[i] != '\0' && i < buffer_size; ++i)
        buffer[i] = (CGChar)str[i];
    buffer[i] = (CGChar)'\0';
}