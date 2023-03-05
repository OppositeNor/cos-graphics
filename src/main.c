#include "graphics/graphics.h"
#include "log/log.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    CGWindow* window = CGCreateWindow(640, 480, "Graphics test", CG_FALSE);
    if (window == NULL)
        return 0;
    CGSetClearScreenColor(CGConstructColor(0.2f, 0.2f, 0.2f, 1.0f));
    CGTriangle *triangle = CGCreateTriangle(
        CGConstructVector2(0.5, 0.5), 
        CGConstructVector2(-0.5, -0.5),
        CGConstructVector2(-0.5, 0.5));
    while(!CGShouldWindowClose(window))
    {
        CGTickRenderStart();
        CGDrawTrangle(triangle);


        CGTickRenderEnd(window);
    }
    free(triangle);
    CGTerminateGraphics();
    return 0;
}