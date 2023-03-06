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
        CGConstructVector2(100, 100),
        CGConstructVector2(100, -100),
        CGConstructVector2(-100, 100));
    triangle->property;
    unsigned long long delta;
    
    while(!CGShouldWindowClose(window))
    {
        CGTickRenderStart();
        CGDrawTriangle(triangle);

        CGTickRenderEnd(window);
    }
    free(triangle);
    CGTerminateGraphics();
    free(window);
    return 0;
}