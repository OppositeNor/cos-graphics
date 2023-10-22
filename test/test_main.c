#include "unit_test/unit_test.h"
#include "test_utils/test_utils.h"
#include "test_resource/test_resource.h"
#include "test_graphics/test_graphics.h"
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

    CGGraphicsTestStart();
    CGGraphicsTestCGVector2Cross1();
    CGGraphicsTestCGVector2Cross2();
    CGGraphicsTestCGVector2Cross3();
    CGGraphicsTestEnd();
    
    CGTestResourceStart();
    CGTestResourceLoadFile1();
    CGTestResourceLoadFile2();
    CGTestResourceEnd();

    CGEndUnitTest();
}