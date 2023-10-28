#include "test_graphics.h"
#include "cos_graphics/graphics.h"
#include "../unit_test/unit_test.h"

CGWindow* window;

void CGGraphicsTestStart()
{
    window = CGCreateWindow(800, 600, CGSTR("Test Window"), CGConstructDefaultWindowSubProperty());
    CGT_EXPECT_NOT_NULL(window);
    CGT_EXPECT_NO_ERROR();
}

void CGGraphicsTestCGVector2Cross1()
{
    CGVector2 vec_1 = { 1.0f, 2.0f };
    CGVector2 vec_2 = { 3.0f, 4.0f };
    float result = CGVector2Cross(vec_1, vec_2);
    CGT_EXPECT_REAL_EQUAL(result, 2.0f, 0.0001f);
    CGT_EXPECT_NO_ERROR();
}

void CGGraphicsTestCGVector2Cross2()
{
    CGVector2 vec_1 = { 0.0f, 0.0f };
    CGVector2 vec_2 = { 0.0f, 0.0f };
    float result = CGVector2Cross(vec_1, vec_2);
    CGT_EXPECT_REAL_EQUAL(result, 0.0f, 0.0001f);
    CGT_EXPECT_NO_ERROR();
}

void CGGraphicsTestCGVector2Cross3()
{
    CGVector2 vec_1 = { 1.0f, 0.0f };
    CGVector2 vec_2 = { 0.0f, 1.0f };
    float result = CGVector2Cross(vec_1, vec_2);
    CGT_EXPECT_REAL_EQUAL(result, -1.0f, 0.0001f);
    CGT_EXPECT_NO_ERROR();
}

void CGGraphicsTestCGVector2Add1()
{
    CGVector2 vec_1 = { 1.0f, 2.0f };
    CGVector2 vec_2 = { 3.0f, 4.0f };
    CGVector2 result = CGVector2Add(vec_1, vec_2);
    CGT_EXPECT_REAL_EQUAL(result.x, 4.0f, 0.0001f);
    CGT_EXPECT_REAL_EQUAL(result.y, 6.0f, 0.0001f);
    CGT_EXPECT_NO_ERROR();
}

void CGGraphicsTestCGVector2Add2()
{
    CGVector2 vec_1 = { 0.0f, 0.0f };
    CGVector2 vec_2 = { 0.0f, 0.0f };
    CGVector2 result = CGVector2Add(vec_1, vec_2);
    CGT_EXPECT_REAL_EQUAL(result.x, 0.0f, 0.0001f);
    CGT_EXPECT_REAL_EQUAL(result.y, 0.0f, 0.0001f);
    CGT_EXPECT_NO_ERROR();
}

void CGGraphicsTestCGVector2Add3()
{
    CGVector2 vec_1 = { 1.0f, 0.0f };
    CGVector2 vec_2 = { 0.0f, -1.0f };
    CGVector2 result = CGVector2Add(vec_1, vec_2);
    CGT_EXPECT_REAL_EQUAL(result.x, 1.0f, 0.0001f);
    CGT_EXPECT_REAL_EQUAL(result.y, -1.0f, 0.0001f);
    CGT_EXPECT_NO_ERROR();
}

void CGGraphicsTestCGVector2Sub1()
{
    CGVector2 vec_1 = { 1.0f, 2.0f };
    CGVector2 vec_2 = { 3.0f, 4.0f };
    CGVector2 result = CGVector2Sub(vec_1, vec_2);
    CGT_EXPECT_REAL_EQUAL(result.x, -2.0f, 0.0001f);
    CGT_EXPECT_REAL_EQUAL(result.y, -2.0f, 0.0001f);
    CGT_EXPECT_NO_ERROR();
}

void CGGraphicsTestCGVector2Sub2()
{
    CGVector2 vec_1 = { 0.0f, 0.0f };
    CGVector2 vec_2 = { 0.0f, 0.0f };
    CGVector2 result = CGVector2Sub(vec_1, vec_2);
    CGT_EXPECT_REAL_EQUAL(result.x, 0.0f, 0.0001f);
    CGT_EXPECT_REAL_EQUAL(result.y, 0.0f, 0.0001f);
    CGT_EXPECT_NO_ERROR();
}

void CGGraphicsTestCGVector2Sub3()
{
    CGVector2 vec_1 = { 1.0f, 0.0f };
    CGVector2 vec_2 = { 0.0f, -1.0f };
    CGVector2 result = CGVector2Sub(vec_1, vec_2);
    CGT_EXPECT_REAL_EQUAL(result.x, 1.0f, 0.0001f);
    CGT_EXPECT_REAL_EQUAL(result.y, 1.0f, 0.0001f);
    CGT_EXPECT_NO_ERROR();
}

void CGTestCGSetWindowPosition1()
{
    CGSetWindowPosition(window, (CGVector2){ 0.0f, 0.0f });
    CGT_EXPECT_NO_ERROR();
    CGT_EXPECT_REAL_EQUAL(CGGetWindowPosition(window).x, 0.0f, 0.0001f);
    CGT_EXPECT_NO_ERROR();
    CGT_EXPECT_REAL_EQUAL(CGGetWindowPosition(window).y, 0.0f, 0.0001f);
    CGT_EXPECT_NO_ERROR();
}

void CGTestCGSetWindowPosition2()
{
    CGSetWindowPosition(window, (CGVector2){ 1.0f, 1.0f });
    CGT_EXPECT_NO_ERROR();
    CGT_EXPECT_REAL_EQUAL(CGGetWindowPosition(window).x, 1.0f, 0.0001f);
    CGT_EXPECT_NO_ERROR();
    CGT_EXPECT_REAL_EQUAL(CGGetWindowPosition(window).y, 1.0f, 0.0001f);
    CGT_EXPECT_NO_ERROR();
}

void CGTestCGSetWindowPosition3()
{
    CGSetWindowPosition(window, (CGVector2){ -1.0f, -1.0f });
    CGT_EXPECT_NO_ERROR();
    CGT_EXPECT_REAL_EQUAL(CGGetWindowPosition(window).x, -1.0f, 0.0001f);
    CGT_EXPECT_NO_ERROR();
    CGT_EXPECT_REAL_EQUAL(CGGetWindowPosition(window).y, -1.0f, 0.0001f);
    CGT_EXPECT_NO_ERROR();
}

void test_key_callback_function(CGWindow* window, int key, int action) {}

void CGTestCGSetKeyCallback1()
{
    CGSetKeyCallback(test_key_callback_function);
    CGT_EXPECT_NO_ERROR();
    CGT_EXPECT_INT_EQUAL(CGGetKeyCallback(window), test_key_callback_function);
    CGT_EXPECT_NO_ERROR();
}

void CGTestCGSetKeyCallback2()
{
    CGSetKeyCallback(NULL);
    CGT_EXPECT_NO_ERROR();
    CGT_EXPECT_INT_EQUAL(CGGetKeyCallback(window), NULL);
    CGT_EXPECT_NO_ERROR();
}

void CGGraphicsTestEnd()
{
    CGTerminateGraphics();
    CGT_EXPECT_NO_ERROR();
}