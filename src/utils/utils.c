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

void CGWriteArrayPos(void* target_array, const void* source_array, unsigned int step_size,
    unsigned int top, unsigned int left, unsigned int bottom, unsigned int right,
    unsigned int line_length)
{
    CG_ERROR_CONDITION(target_array == NULL, CGSTR("Cannot write array position with target array pointer \"NULL\"."));
	CG_ERROR_CONDITION(source_array == NULL, CGSTR("Cannot write array position with from array pointer \"NULL\"."));
    CG_ERROR_CONDITION(bottom < top || right < left || line_length == 0 || line_length < (right - left), CGSTR("Invalid dimenssion values"));
    const unsigned int line_size_bytes = line_length * step_size;
    const unsigned int left_bytes = left * step_size;
    for (unsigned int i = top; i < bottom; ++i)
    {
        memcpy((char*)target_array + i * line_size_bytes + left_bytes, (const CGByte*)source_array + i * line_size_bytes, (right - left) * step_size);
    }
}