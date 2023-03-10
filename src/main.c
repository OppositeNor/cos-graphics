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
#include "cos_graphics/graphics.h"
#include "cos_graphics/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main()
{
    CGWindow* window = CGCreateWindow(640, 480, "Graphics test", CG_FALSE);
    if (window == NULL)
        return 0;
    CGQuadrangle quad1 = CGConstructQuadrangle(
        (CGVector2){-100, -50},
        (CGVector2){-50, 50},
        (CGVector2){50, 100},
        (CGVector2){50, -100});
    CGQuadrangle quad2 = CGConstructQuadrangle(
        (CGVector2){-100, -100},
        (CGVector2){-50, 50},
        (CGVector2){50, 100},
        (CGVector2){100, -100});
    float rotation = 0.0f;
    quad1.property = CGCreateGeometryProperty(
        CGConstructColor(1.0f, 0.0f, 0.0f, 1.0f),
        (CGVector2){-50, 0},
        (CGVector2){1, 1},
        rotation);
    quad2.property = CGCreateGeometryProperty(
        CGConstructColor(1.0f, 0.0f, 0.0f, 1.0f),
        (CGVector2){-50, 0},
        (CGVector2){1, 1},
        rotation
    );
    while(!CGShouldWindowClose(window))
    {
        static float clock = 0;
        clock += 0.05f;
        quad1.property->scale.x = sin(clock);
        quad1.property->scale.y = cos(clock);
        quad2.property->scale.x = cos(clock - 2.5);
        quad2.property->scale.y = sin(clock - 2.5);
        CGTickRenderStart();
        CGDrawQuadrangle(&quad2, window);
        CGDrawQuadrangle(&quad1, window);
        CGTickRenderEnd(window);
    }
    CGTerminateGraphics();
    free(window);
    window = NULL;
    CGDeleteGeometryProperty(quad1.property);
    return 0;
}
#endif