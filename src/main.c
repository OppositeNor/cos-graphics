#if 0
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
        (CGVector2){100, 100),
        (CGVector2){100, -100),
        (CGVector2){-100, 100));
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
#endif

#if 1
#include "graphics/graphics.h"
#include "log/log.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    CGWindow* window = CGCreateWindow(640, 480, "Graphics test", CG_FALSE);
    if (window == NULL)
        return 0;
    CGQuadrangle quad1 = CGConstructQuadrangle(
        (CGVector2){-100, -100},
        (CGVector2){-100, 50},
        (CGVector2){50, 100},
        (CGVector2){100, -100});
    CGQuadrangle quad2 = CGConstructQuadrangle(
        (CGVector2){-100, -100},
        (CGVector2){-100, 50},
        (CGVector2){50, 100},
        (CGVector2){100, -100});
    float rotation = 0.0f;
    quad1.property = CGCreateGeometryProperty(
        CGConstructColor(0.5f, 0.6f, 0.2f, 1.0f),
        (CGVector2){-50, 0},
        (CGVector2){1, 1},
        rotation);
    
    while(!CGShouldWindowClose(window))
    {
        static float clock = 0;
        clock += 0.1f;
        quad1.property->rotation = clock;
        CGTickRenderStart();
        CGDrawQuadrangle(&quad1);
        CGDrawQuadrangle(&quad2);
        CGTickRenderEnd(window);
    }
    CGTerminateGraphics();
    free(window);
    window = NULL;
    CGDeleteGeometryProperty(quad1.property);
    return 0;
}
#endif