#include "unit_test.h"
static unsigned int total_test_count = 0;
static unsigned int failed_test_count = 0;

void CGStartUnitTest()
{
    CG_PRINT(CGSTR("Starting unit test. All of the error below printed out are for testing purposes. Please ignore."));
    total_test_count = 0;
    failed_test_count = 0;
    
}

void CGEndUnitTest()
{
    if (failed_test_count == 0)
        CG_PRINT(CGSTR("All tests passed"));
    else
    {
        CG_PRINT(CGSTR("Total test count: %d"), total_test_count);
        CG_PRINT(CGSTR("Failed test count: %d"), failed_test_count);
    }
}

void CGTUpadate(CG_BOOL is_failed)
{
    ++total_test_count;
    if (is_failed)
        ++failed_test_count;
}

CG_BOOL CGTIsMemoryEqual(const void* array_1, const void* array_2, unsigned int size)
{
    memcmp(array_1, array_2, size) == 0, CGSTR("Expected equal arrays");
}