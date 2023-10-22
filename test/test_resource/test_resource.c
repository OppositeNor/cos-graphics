#include "test_resource.h"
#include "../unit_test/unit_test.h"
#include <stdio.h>

void CGTestResourceStart()
{
    CGInitResourceSystem();
}
void CGTestResourceEnd()
{
    CGTerminateResourceSystem();
}

void CGTestResourceLoadFile1()
{
    CGByte* data = CGLoadFile(CGSTR("./test/test_file.txt"));
    CGT_EXPECT_NOT_NULL(data);
    CGT_EXPECT_STRING_EQUAL(data, "This is a test file.");
    CGT_EXPECT_NO_ERROR();
    free(data);
}

void CGTestResourceLoadFile2()
{
    CGByte* data = CGLoadFile(CGSTR("./this/is/a/made/up/path.txt"));
    CGT_EXPECT_ERROR();
    free(data);
}

//todo: add more tests