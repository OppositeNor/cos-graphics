#include "unit_test/unit_test.h"
#include "test_utils/test_utils.h"
#include "test_resource/test_resource.h"
int main()
{
    CGStartUnitTest();
    CGTestCGCharToChar1();
    CGTestCGCharToChar2();
    CGTestCGCharToChar3();
    CGTestCGCharToChar4();
    CGTestCGCharToChar5();

    CGTestCharToCGChar1();
    CGTestCharToCGChar2();
    CGTestCharToCGChar3();
    CGTestCharToCGChar4();
    CGTestCharToCGChar5();

    
    CGTestResourceLoadFile1();
    CGEndUnitTest();
}