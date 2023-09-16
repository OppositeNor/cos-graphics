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

/**
 * @brief Write an array to another array at the specified position.
 * 
 * @param target_array The target array.
 * @param source_array The array to write.
 * @param step_size The size of each element in bytes.
 * @param top The top position. (in element)
 * @param left The left position. (in element)
 * @param bottom The bottom position. (in element)
 * @param right The right position. (in element)
 * @param line_length The number of elements of each line.
*/
void CGWriteArrayPos(void* target_array, const void* source_array, unsigned int step_size,
    unsigned int top, unsigned int left, unsigned int bottom, unsigned int right,
    unsigned int line_length);

#endif