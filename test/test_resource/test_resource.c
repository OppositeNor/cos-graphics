#include "test_resource.h"
#include "../unit_test/unit_test.h"
#include <stdio.h>
void CGTestResourceLoadFile1()
{
    CGByte* data = CGLoadFile(CGSTR("./test/test_file.txt"));
    CGT_EXPECT_NOT_NULL(data);
    CGT_EXPECT_STRING_EQUAL(data, "This is a test file.");
    free(data);
}