#if 0

#include "cos_graphics/graphics.h"
#include "cos_graphics/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef CG_TG_WIN
#include<windows.h>
#include<conio.h>
#elif defined CG_TG_LINUX
#include <unistd.h>
#endif

void KeyCallback(CGWindow* window, int key, int action);
void MouseButtonCallback(CGWindow* window, int button, int action);

CGRenderObjectProperty* g_prop = NULL;

int main()
{
    CGWindow* window = CGCreateWindow(640, 480, CGSTR("Graphics test"), CG_FALSE, CG_TRUE);
    CGSetKeyCallback(KeyCallback);
    CGSetMouseButtonCallback(MouseButtonCallback);
    
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
    CGRenderObjectProperty* quad1_property = CGCreateRenderObjectProperty(
        CGConstructColor(1.0f, 0.0f, 0.0f, 0.5f),
        (CGVector2){-50, 0},
        (CGVector2){1, 1},
        rotation);
    CGRenderObjectProperty* quad2_property = CGCreateRenderObjectProperty(
        CGConstructColor(0.0f, 1.0f, 0.0f, 0.8f),
        (CGVector2){0, 0},
        (CGVector2){1, 1},
        rotation
    );
    
    CGVisualImage* sprite = CGCreateVisualImage(CGSTR("test1"), window);
    CGVisualImage* sprite1 = CGCopyVisualImage(sprite);
    quad2_property->z = -3;
    double tick_end_time = CGGetCurrentTime();
    const double fixed_delta = 1.0 / 60;
    CGRenderObjectProperty* prop = CGCreateRenderObjectProperty((CGColor){1.0f, 1.0f, 1.0f, 1.0f}, (CGVector2){0, -100}, (CGVector2){1, 1}, 0);
    g_prop = CGCreateRenderObjectProperty((CGColor){1.0f, 1.0f, 1.0f, 1.0f}, (CGVector2){0, -100}, (CGVector2){1, 1}, 0);
    
    while(!CGShouldWindowClose(window))
    {
        static double tick_start_time = 0;
        static double delta = 0.01;
        tick_start_time = CGGetCurrentTime();
        static float clock = 0;
        clock += delta * 3;
        quad1.vert_1 = CGConstructVector2(sin(clock / 0.3) * 30 + 150, sin(clock) * 20 - 50);
        quad1.vert_2 = CGConstructVector2(sin(clock / 0.5) * 30 + 100, sin(clock / 0.7) * 20 + 50);
        quad1.vert_3 = CGConstructVector2(sin(clock / 2) * 30 - 150, sin(clock / 0.3 - 3) * 20 + 50);
        quad1.vert_4 = CGConstructVector2(sin(clock / 0.8) * 30 - 100, sin(clock / 0.9 + 5) * 20 - 50);

        CGQuadrangle* quad2 = CGCreateQuadrangle(
            (CGVector2){100, 100},
            (CGVector2){-100, 100},
            (CGVector2){-100, -100},
            (CGVector2){100, -100});
        quad2->is_temp = CG_TRUE;
        CGDrawQuadrangle(quad2, quad2_property, window);
        
        CGTickRenderStart(window);
        CGDrawTriangle(&triangle, NULL, window);
        CGDrawTriangle(&triangle, quad2_property, window);
        CGRotateRenderObject(prop, delta, (CGVector2){100, 100});
        CGDrawVisualImage(sprite, prop, window);
        CGRenderObjectProperty* prop2 = CGCreateRenderObjectProperty((CGColor){1.0f, 1.0f, 1.0f, 1.0f}, (CGVector2){0, 100}, (CGVector2){1, 1}, 0);
        CGDrawVisualImage(sprite1, g_prop, window);
        g_prop->scale.x = 3.0f;
        g_prop->scale.y = 3.0f;
        quad2_property->rotation = clock * 0.2;
        CGDrawQuadrangle(&quad1, quad1_property, window);
        CGWindowDraw(window);

        CGVisualImage* visual_image = CGCreateVisualImage(CGSTR("test1"), window);
        CGDraw(visual_image, NULL, window, CG_RD_TYPE_VISUAL_IMAGE);


        CGTickRenderEnd();
        tick_end_time = CGGetCurrentTime();
        delta = tick_end_time - tick_start_time;
        CGFree(prop2);
    }
    CGFree(prop);
    //CGFree(window);
    CGTerminateGraphics();
    return 0;
}
void KeyCallback(CGWindow* window, int key, int action)
{
    if (action == CG_PRESS)
    {
        switch(key)
        {
            case CG_KEY_W:
                if (g_prop)
                    g_prop->transform.y += 10;
                break;
            case CG_KEY_A:
                if (g_prop)
                    g_prop->transform.x -= 10;
                break;
            case CG_KEY_S:
                if (g_prop)
                    g_prop->transform.y -= 10;
                break;
            case CG_KEY_D:
                if (g_prop)
                    g_prop->transform.x += 10;
                break;
            case CG_KEY_R:
                if (g_prop)
                    g_prop->rotation += 0.1;
                break;
            case CG_KEY_T:
                if (g_prop)
                    g_prop->rotation -= 0.1;
                break;
            case CG_KEY_Q:
                if (g_prop)
                    g_prop->scale.x += 0.1;
                break;
            case CG_KEY_E:
                if (g_prop)
                    g_prop->scale.x -= 0.1;
                break;
            default:
                break;
        }
    }
}

void MouseButtonCallback(CGWindow* window, int button, int action)
{
    if (button == CG_MOUSE_BUTTON_LEFT && action == CG_PRESS)
        CG_PRINT(CGSTR("TEST MOUSE BUTTON"));
}

#endif

#if 1
#include "cos_graphics/graphics.h"
#include "cos_graphics/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef CG_TG_WIN
#include<windows.h>
#include<conio.h>
#elif defined CG_TG_LINUX
#include <unistd.h>
#endif
int main()
{
    CGWindowSubProperty window_property = CGConstructDefaultWindowSubProperty();
    window_property.resizable = CG_TRUE;
    CGWindow* window = CGCreateWindow(1280, 720, CGSTR("Graphics test"), window_property);
    CGSetClearScreenColor(CGConstructColor(0.3f, 0.3f, 0.3f, 0.2f));
    if (window == NULL)
        return 0;
    CGRenderObjectProperty* prop = CGCreateRenderObjectProperty(
        CGConstructColor(1.0f, 1.0f, 1.0f, 1.0f),
        (CGVector2){0, 0},
        (CGVector2){1, 1},
        0.0f);
    CGRenderObjectProperty* prop1 = CGCreateRenderObjectProperty(
        CGConstructColor(1.0f, 1.0f, 1.0f, 1.0f),
        (CGVector2) {-50, 0},
        (CGVector2) {1, 1},
        0.0f);
    prop->scale = (CGVector2){ 1.0f, 1.0f };
    
    double tick_end_time = CGGetCurrentTime();
        CGVisualImage* test_text = CGCreateTextVisualImage(CGSTR("HelloWorld"), NULL, CGConstructTextProperty(120, 120, 30, 2), window);
    while(!CGShouldWindowClose(window))
    {
        static double tick_start_time = 0;
        static double delta = 0.01;
        tick_start_time = CGGetCurrentTime();
        static float clock = 0;
        clock += (float)delta * 3;
        CGTickRenderStart(window);
#if 0
        CGVisualImage* visual_image = CGCreateTVisualImage(CGSTR("test1"), window);
        //visual_image->is_clamped = CG_TRUE;
        visual_image->clamp_top_left = (CGVector2){3.0f, 3.0f};
        // visual_image->img_channels = 1;
        prop->scale = (CGVector2){3.0f, 3.0f};
#endif

        //test_text->is_temp = CG_TRUE;
        prop->scale = (CGVector2){ 0.5f, 0.5f };
        //prop->transform.x = sin(clock) * 100;
        //CGDrawVisualImage(test_text, prop, window);
        //CGSetWindowPosition(window, (CGVector2){300, 300 + sin(clock) * 100});
        CGDrawText(CGSTR("test_text_2"), NULL, CGConstructTextProperty(120, 120, 30, 4), prop, window);
        CGWindowDraw(window);

        CGTickRenderEnd();
        tick_end_time = CGGetCurrentTime();
        delta = tick_end_time - tick_start_time;
    }

    CGTerminateGraphics();
    return 0;
}
#endif