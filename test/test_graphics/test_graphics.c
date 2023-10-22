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

//todo: add more tests

void CGGraphicsTestEnd()
{
    CGTerminateGraphics();
    CGT_EXPECT_NO_ERROR();
}