#if 0
#include "cos_graphics/graphics.h"
#include "cos_graphics/log.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    CGWindow* window = CGCreateWindow(640, 480, "Graphics test", CG_FALSE);
    if (window == NULL)
        return 0;
    CGTriangle *triangle = CGCreateTriangle( 
        (CGVector2){100, 100},
        (CGVector2){100, -100},
        (CGVector2){-100, 100});
    
    while(!CGShouldWindowClose(window))
    {
        CGTickRenderStart(window);
        CGDrawTriangle(triangle, window);

        CGTickRenderEnd(window);
    }
    CGDeleteGeometryProperty(triangle->property);
    free(triangle);
    CGTerminateGraphics();
    free(window);
    return 0;
}
#endif

#if 0
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
    CGTriangle triangle = CGConstructTriangle( 
        (CGVector2){100, 100},
        (CGVector2){100, -100},
        (CGVector2){-100, 100});
    float rotation = 0.0f;
    triangle.z = 1;
    quad1.property = CGCreateGeometryProperty(
        CGConstructColor(0.0f, 1.0f, 0.0f, 0.8f),
        (CGVector2){-50, 0},
        (CGVector2){1, 1},
        rotation);
    quad2.property = CGCreateGeometryProperty(
        CGConstructColor(0.0f, 1.0f, 0.0f, 0.8f),
        (CGVector2){-50, 0},
        (CGVector2){1, 1},
        rotation
    );

    CGSprite* sprite = CGCreateSprite("./test2.png", 
        CGCreateSpriteProperty((CGVector2){0, 0}, (CGVector2){1, 1}, 0), window);
    sprite->z = -2;
    double tick_end_time = CGGetCurrentTime();
    while(!CGShouldWindowClose(window))
    {
        static double tick_start_time = 0;
        static double delta = 0.01;
        tick_start_time = CGGetCurrentTime();
        
        static float clock = 0;
        clock += delta * 3;
        quad1.property->scale.x = sin(clock);
        quad1.property->scale.y = cos(clock);
        quad1.property->transform.x = sin(clock + 1.3) * 300;
        quad2.property->scale.x = cos(clock - 2.5);
        quad2.property->scale.y = sin(clock - 2.5);

        CGTickRenderStart(window);
        //CGDrawTriangle(&triangle, window);
        CGDrawQuadrangle(&quad2, window);
        CGDrawQuadrangle(&quad1, window);
        CGDrawSprite(sprite, window);
        CGTickRenderEnd();
        tick_end_time = CGGetCurrentTime();
        delta = tick_end_time - tick_start_time;
    }
    free(sprite->property);
    CGDeleteSprite(sprite);
    CGDestroyWindow(window);
    CGTerminateGraphics();
    window = NULL;
    free(quad1.property);
    free(quad2.property);
    return 0;
}
#endif

#include "cos_graphics/graphics.h"
#include "cos_graphics/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main()
{
    CGWindow* window = CGCreateWindow(1920, 1080, "Graphics test", CG_FALSE);
    CGSetClearScreenColor((CGColor){1.0f, 1.0f, 1.0f, 1.0f});
    window->width = 1920;
    window->height = 1080;
    if (window == NULL)
        return 0;
    float rotation = 0.0f;
    CGSprite* sprite = CGCreateSprite("./test3.png", 
        CGCreateSpriteProperty((CGVector2){0, 0}, (CGVector2){1, 1}, 0), window);
    sprite->z = 1;
    CGSprite* sprite2 = CGCreateSprite("./test2.png", 
        CGCreateSpriteProperty((CGVector2){600, 0}, (CGVector2){1, 1}, 0), window);
    CGSprite* sprite3 = CGCreateSprite("./test3.png", 
        CGCreateSpriteProperty((CGVector2){300, 600}, (CGVector2){1, 1}, 0), window);
    sprite->z = 1;
    CGSprite* sprite4 = CGCreateSprite("./test2.png", 
        CGCreateSpriteProperty((CGVector2){-300, -600}, (CGVector2){1, 1}, 0), window);
    sprite2->z = -1;
    double tick_end_time = CGGetCurrentTime();
    while(!CGShouldWindowClose(window))
    {
        static double tick_start_time = 0;
        static double delta = 0.01;
        tick_start_time = CGGetCurrentTime();

        CGTickRenderStart(window);
        CGDrawSprite(sprite2, window);
        CGDrawSprite(sprite, window);
        CGDrawSprite(sprite3, window);
        CGDrawSprite(sprite4, window);
        CGWindowDraw(window);
        CG_PRINT("");
        CGTickRenderEnd();
        tick_end_time = CGGetCurrentTime();
        delta = tick_end_time - tick_start_time;
    }
    free(sprite->property);
    CGDeleteSprite(sprite);
    CGDestroyWindow(window);
    CGTerminateGraphics();
    window = NULL;
    return 0;
}