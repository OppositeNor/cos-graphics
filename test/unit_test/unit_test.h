#ifndef _CGT_UNIT_TEST_H_
#define _CGT_UNIT_TEST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "cos_graphics/defs.h"
#include <memory.h>
#include <string.h>
#include <math.h>

static CG_BOOL cgt_is_test_failed;

#define CG_MODIFY_OUTPUT_MESSAGE
#define CG_PRINT(...) CGPrint(CGSTR("CosGraphics Unit Test"), CGSTR("Output"), __VA_ARGS__)
#define CG_PRINT_WITH_FUNCTION(...) CGPrint(CGSTR("CosGraphics Unit Test"), CGSTR("Output"), __VA_ARGS__)
#define CG_ERROR(...) CGError()
#define CG_WARNING(...) CGPrint(CGSTR("CosGraphics Unit Test"), CGSTR("Warning"), __VA_ARGS__)

#include "cos_graphics/log.h"

#define CGT_UNIT_TEST(cond, ...)      \
    cgt_is_test_failed = !(cond);     \
    CGTUpadate(cgt_is_test_failed); \
    if (cgt_is_test_failed)         \
    {                               \
        CG_WARNING(CGSTR("A unit test failed at: %hs, line: %d"), __FILE__, __LINE__);     \
        CG_WARNING(__VA_ARGS__);     \
    }

#define CGT_EXPECT_INT_EQUAL(val_1, val_2)\
    CGT_UNIT_TEST((val_1) == (val_2), CGSTR("Expected interger value to be: %ld, but get %ld instead."), (long long)val_2, (long long)val_1)
#define CGT_EXPECT_INT_NOT_EQUAL(val_1, val_2)\
    CGT_UNIT_TEST((val_1) != (val_2), CGSTR("Expected interger value to not be: %ld, but get %ld instead."), (long long)val_2, (long long)val_1)

#define CGT_EXPECT_REAL_EQUAL(val_1, val_2, error)\
    CGT_UNIT_TEST(fabs((val_1) - (val_2)) < error, CGSTR("Expected real value to be: %lf, but get %lf instead."), (double)val_2, (double)val_1)
#define CGT_EXPECT_REAL_NOT_EQUAL(val_1, val_2)\
    CGT_UNIT_TEST(fabs((val_1) - (val_2)) >= error, CGSTR("Expected real value to not be: %lf, but get %lf instead."), (double)val_2, (double)val_1)

#define CGT_EXPECT_STRING_EQUAL(val_1, val_2)\
    CGT_UNIT_TEST(strcmp(val_1, val_2) == 0, CGSTR("Expected string value to be: \"%hs\", but get \"%hs\" instead."), val_2, val_1)
#define CGT_EXPECT_STRING_NOT_EQUAL(val_1, val_2)\
    CGT_UNIT_TEST(strcmp(val_1, val_2) == 0, CGSTR("Expected string value to not be: \"%hs\", but get \"%hs\" instead."), val_2, val_1)

#define CGT_EXPECT_W_STRING_EQUAL(val_1, val_2)\
    CGT_UNIT_TEST(wcscmp(val_1, val_2) == 0, CGSTR("Expected string value to be: %ls, but get %ls instead."), val_2, val_1)
#define CGT_EXPECT_W_STRING_NOT_EQUAL(val_1, val_2)\
    CGT_UNIT_TEST(wcscmp(val_1, val_2) == 0, CGSTR("Expected string value to not be: %ls, but get %ls instead."), val_2, val_1)

#ifdef CG_USE_WCHAR
    #define CGT_EXPECT_CG_STRING_EQUAL(val_1, val_2) CGT_EXPECT_W_STRING_EQUAL(val_1, val_2)
    #define CGT_EXPECT_CG_STRING_NOT_EQUAL(val_1, val_2) CGT_EXPECT_W_STRING_NOT_EQUAL(val_1, val_2)
#else
    #define CGT_EXPECT_CG_STRING_EQUAL(val_1, val_2) CGT_EXPECT_W_STRING_EQUAL(val_1, val_2)
    #define CGT_EXPECT_CG_STRING_NOT_EQUAL(val_1, val_2) CGT_EXPECT_W_STRING_NOT_EQUAL(val_1, val_2)
#endif

#define CGT_EXPECT_MEM_EQUAL(val_1, val_2) \
    CGT_UNIT_TEST(memcmp(val_1, val_2, size) == 0, CGSTR("Expected equal memory."))
#define CGT_EXPECT_MEM_NOT_EQUAL(val_1, val_2) \
    CGT_UNIT_TEST(memcmp(val_1, val_2, size) != 0, CGSTR("Expected not equal memory."))

#define CGT_EXPECT_ERROR() CGT_UNIT_TEST(CGIsHasError(), CGSTR("Expected error."))
#define CGT_EXPECT_NO_ERROR() CGT_UNIT_TEST(!CGIsHasError(), CGSTR("An error occured. expected no error."))

#define CGT_EXPECT_NOT_NULL(ptr) CGT_UNIT_TEST(ptr != NULL, CGSTR("Expected not null pointer, but get %p instead."), ptr);

void CGStartUnitTest();
void CGEndUnitTest();
void CGTUpadate(CG_BOOL is_failed);

CG_BOOL CGTIsMemoryEqual(const void* array_1, const void* array_2, unsigned int size);

#ifdef __cplusplus
}
#endif

#endif  // _CGT_UNIT_TEST_H_