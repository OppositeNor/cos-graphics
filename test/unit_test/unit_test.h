#ifdef __cplusplus
extern "C" {
#endif

#include "cos_graphics/defs.h"
#include <memory.h>
#include <string.h>

static CG_BOOL cgt_is_test_failed;

#define CG_MODIFY_OUTPUT_MESSAGE
#define CG_PRINT(...) CGPrint(CGSTR("CosGraphics Unit Test"), CGSTR("Output"), __VA_ARGS__)
#define CG_PRINT_WITH_FUNCTION(...) CGPrint(CGSTR("CosGraphics Unit Test"), CGSTR("Output"), __VA_ARGS__)
#define CG_ERROR(...) CGError()
#define CG_WARNING(...) CGPrint(CGSTR("CosGraphics Unit Test"), CGSTR("Warning"), __VA_ARGS__)

#include "cos_graphics/log.h"

#define CGT_UNIT_TEST(cond, ...)      \
    cgt_is_test_failed = !cond;     \
    CGTUpadate(cgt_is_test_failed); \
    if (cgt_is_test_failed)         \
    {                               \
        CG_WARNING(CGSTR("A unit test failed at: %s, line: %d"), __FILE__, __LINE__);     \
        CG_WARNING(__VA_ARGS__);     \
    }

#define CGT_EXPECT_INT_EQUAL(val_1, val_2)\
    CGT_UNIT_TEST((val_1) == (val_2), CGSTR("Expected equal values: %ld, %ld"), (long long)val_1, (long long)val_2)
#define CGT_EXPECT_INT_NOT_EQUAL(val_1, val_2)\
    CGT_UNIT_TEST((val_1) != (val_2), CGSTR("Expected not equal values: %ld, %ld"), (long long)val_1, (long long)val_2)

#define CGT_EXPECT_REAL_EQUAL(val_1, val_2, error)\
    CGT_UNIT_TEST(fabs((val_1) - (val_2)) < error, CGSTR("Expected equal values: %lf, %lf"), (double)val_1, (double)val_2)
#define CGT_EXPECT_REAL_NOT_EQUAL(val_1, val_2)\
    CGT_UNIT_TEST(fabs((val_1) - (val_2)) >= error, CGSTR("Expected not equal values: %lf, %lf"), (double)val_1, (double)val_2)

#define CGT_EXPECT_STRING_EQUAL(val_1, val_2)\
    CGT_UNIT_TEST(strcmp(val_1, val_2) == 0, CGSTR("Expected equal strings: %s, %s"), val_1, val_2)
#define CGT_EXPECT_STRING_NOT_EQUAL(val_1, val_2)\
    CGT_UNIT_TEST(strcmp(val_1, val_2) != 0, CGSTR("Expected not equal strings: %s, %s"), val_1, val_2)

#define CGT_EXPECT_W_STRING_EQUAL(val_1, val_2)\
    CGT_UNIT_TEST(wcscmp(val_1, val_2) == 0, CGSTR("Expected equal strings: %ls, %ls"), val_1, val_2)
#define CGT_EXPECT_W_STRING_NOT_EQUAL(val_1, val_2)\
    CGT_UNIT_TEST(wcscmp(val_1, val_2) != 0, CGSTR("Expected not equal strings: %ls, %ls"), val_1, val_2)

#ifdef CG_USE_WCHAR
    #define CGT_EXPECT_CG_STRING_EQUAL(val_1, val_2) CGT_EXPECT_W_STRING_EQUAL(val_1, val_2)
    #define CGT_EXPECT_CG_STRING_NOT_EQUAL(val_1, val_2) CGT_EXPECT_W_STRING_NOT_EQUAL(val_1, val_2)
#else
    #define CGT_EXPECT_CG_STRING_EQUAL(val_1, val_2) CGT_EXPECT_W_STRING_EQUAL(val_1, val_2)
    #define CGT_EXPECT_CG_STRING_NOT_EQUAL(val_1, val_2) CGT_EXPECT_W_STRING_NOT_EQUAL(val_1, val_2)
#endif

#define CGT_EXPECT_ERROR() CGT_UNIT_TEST(CGIsHasError(), CGSTR("Expected error."))


void CGStartUnitTest();
void CGEndUnitTest();
void CGTUpadate(CG_BOOL is_failed);

CG_BOOL CGTIsMemoryEqual(const void* array_1, const void* array_2, unsigned int size);

#ifdef __cplusplus
}
#endif