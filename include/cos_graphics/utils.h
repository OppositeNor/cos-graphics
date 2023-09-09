#ifndef _CG_UTILS_H_
#define _CG_UTILS_H_
#include "cos_graphics/defs.h"

/**
 * @brief Convert CGChar to char.
 * 
 * @param str The CGChar string.
 * @param buffer The buffer to store the converted string.
 * @param buffer_size The size of the buffer.
 */
void CGCharToChar(const CGChar* str, char* buffer, unsigned int buffer_size);

/**
 * @brief Convert char to CGChar.
 * 
 * @param str The char string.
 * @param buffer The buffer to store the converted string.
 * @param buffer_size The size of the buffer.
 */
void CharToCGChar(const char* str, CGChar* buffer, unsigned int buffer_size);

#endif