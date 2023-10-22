#include "test_utils.h"
#include "../include/cos_graphics/utils.h"
#include <string.h>
#include <memory.h>

void CGTestCGCharToChar1()
{
    char buffer[256];
    CGCharToChar(CGSTR("Hello World"), buffer, 256);
    CGT_EXPECT_STRING_EQUAL(buffer, "Hello World");
}

void CGTestCGCharToChar2()
{
    char buffer[256];
    CGCharToChar(CGSTR(""), buffer, 256);
    CGT_EXPECT_STRING_EQUAL(buffer, "");
}

void CGTestCGCharToChar3()
{
    char buffer[256];
    CGCharToChar(NULL, buffer, 256);
    CGT_EXPECT_ERROR();
    CGResetError();
}

void CGTestCGCharToChar4()
{
    CGCharToChar(CGSTR("hello"), NULL, 256);
    CGT_EXPECT_ERROR();
    CGResetError();
}

void CGTestCGCharToChar5()
{
    char buffer[256];
    CGCharToChar(CGSTR("This! is a test... for string overflow..."), buffer, 6);
    CGT_EXPECT_ERROR();
    CGResetError();
}

void CGTestCharToCGChar1()
{
    CGChar buffer[256];
    CharToCGChar("Hello World", buffer, 256);
    CGT_EXPECT_CG_STRING_EQUAL(buffer, CGSTR("Hello World"));
}

void CGTestCharToCGChar2()
{
    CGChar buffer[256];
    CharToCGChar("", buffer, 256);
    CGT_EXPECT_CG_STRING_EQUAL(buffer, CGSTR(""));
}

void CGTestCharToCGChar3()
{
    CGChar buffer[256];
    CharToCGChar(NULL, buffer, 256);
    CGT_EXPECT_ERROR();
    CGResetError();
}

void CGTestCharToCGChar4()
{
    CharToCGChar("hello", NULL, 256);
    CGT_EXPECT_ERROR();
    CGResetError();
}

void CGTestCharToCGChar5()
{
    CGChar buffer[256];
    CharToCGChar("This! is a test... for string overflow...", buffer, 6);
    CGT_EXPECT_ERROR();
    CGResetError();
}
