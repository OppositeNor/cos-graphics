#include "graphics/graphics.h"
#include "log/log.h"
#include <stdio.h>

int main()
{
    CGWindow* window = CGCreateWindow(640, 480, "Graphics test", CG_FALSE);
    if (window == NULL)
        return 0;
    CGSetClearScreenColor(CGConstructColor(0.6f, 0.6f, 0.6f, 1.0f));
    while(!CGShouldWindowClose(window))
    {
        CGTickRenderStart();
        CGDrawTrangle(CGCreateTriangle(
            CGConstructVector2(0.5, 0.5), 
            CGConstructVector2(-0.5, -0.5),
            CGConstructVector2(-0.5, 0.5)));
        CGTickRenderEnd(window);
    }
    CGTerminateGraphics();
    return 0;
}