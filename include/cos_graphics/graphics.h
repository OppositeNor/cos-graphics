#ifndef _CG_GRAPHICS_H_
#define _CG_GRAPHICS_H_
#include "defs.h"
#include "linked_list.h"
#include "resource.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CG_TG_WIN
    #include <windows.h>
#endif

#ifndef CG_RENDER_FAR
    /**
     * @brief This value is the smallest assigned z index being started. Note that it does not have any
     * relationship with the z value of each render object. If you have too many objects that needs to 
     * be rendered, and you found out that part of them are not be rendered, you can try to set this to
     * a larger value.
     * You can modify this by defining CG_RENDER_FAR before including this header file.
     * @example 
     * // this is how you can change CG_RENDER_FAR to 8
     * #define CG_RENDER_FAR 8
     * #include "cos_graphics/graphics.h"
     */
    #define CG_RENDER_FAR 128
#endif

#ifndef CG_RENDER_NEAR
    /**
     * @brief This value is the smallest assigned z index being started. Note that it does not have any
     * relationship with the z value of each render object. If you have too many objects that needs to 
     * be rendered, and you found out that part of them are not be rendered, you can try to set this to
     * a smaller value.
     * You can modify this by defining CG_RENDER_FAR before including this header file.
     * @example 
     * // this is how you can change CG_RENDER_NEAR to -8
     * #define CG_RENDER_NEAR -8
     * #include "cos_graphics/graphics.h"
     */
    #define CG_RENDER_NEAR -128
#endif

/**
 * @brief Draw triangle.
 */
#define CGDrawTriangle(triangle_object, property, window) CGDraw(triangle_object, property, window, CG_RD_TYPE_TRIANGLE)

/**
 * @brief Draw quadrangle.
 */
#define CGDrawQuadrangle(quadrangle_object, property, window) CGDraw(quadrangle_object, property, window, CG_RD_TYPE_QUADRANGLE)

/**
 * @brief Draw visual_image.
 */
#define CGDrawVisualImage(visual_image_object, property, window) CGDraw(visual_image_object, property, window, CG_RD_TYPE_VISUAL_IMAGE)

typedef CGLinkedListNode CGRenderNode, CGAnimationNode;

/**
 * @brief 2D vector
 */
typedef struct {
    float x;
    float y;
} CGVector2;

/**
 * @brief Window subproperty.
 */
typedef struct{
    /**
     * @brief Is the window resizable.
     * @default CG_FALSE
     */
    CG_BOOL resizable;
    /**
     * @brief Is the window full screen.
     * @default CG_FALSE
     */
    CG_BOOL use_full_screen;
    /**
     * @brief Is the window boarderless.
     * @default CG_FALSE
     */
    CG_BOOL boarderless;
    /**
     * @brief Is the window transparent.
     * @default CG_FALSE
     */
    CG_BOOL transparent;
    /**
     * @brief Is the window topmost (always on top of other windows)
     * @default CG_FALSE
     */
    CG_BOOL topmost;
} CGWindowSubProperty;

/**
 * @brief Window
 */
typedef struct{
    /**
     * @brief The title of the window.
     */
    CGChar title[256];
    int width;
    int height;
    void* glfw_window_instance;
    unsigned int triangle_vao;
    unsigned int quadrangle_vao;
    unsigned int visual_image_vao;
    CGRenderNode* render_list;
    CGWindowSubProperty sub_property;
} CGWindow;

/**
 * @brief The callback function for key event
 * @param window The window that the event is triggered
 * @param key The key that is pressed
 * @param action The action of the key. It can be CG_PRESS, CG_RELEASE, or CG_REPEAT
 */
typedef void (*CGKeyCallbackFunction)(CGWindow* window, int key, int action);

typedef void (*CGMouseButtonCallbackFunction)(CGWindow* window, int button, int action);

typedef void (*CGCursorPositionCallbackFunction)(CGWindow* window, double x, double y);

/**
 * @brief Viewport
 */
typedef struct {
    /**
     * @brief position of viewport
     */
    CGVector2 position;
    /**
     * @brief demension of viewport
     */
    CGVector2 demension;
} CGViewport;

/**
 * @brief Color
 */
typedef struct {
    float r, g, b, alpha;
}CGColor;

/**
 * @brief Construct a Color
 * 
 * @param r red value (range from 0.0 to 1.0)
 * @param g green value (range from 0.0 to 1.0)
 * @param b blue value (range from 0.0 to 1.0)
 * @param alpha alpha value (range from 0.0 to 1.0)
 * @return CGColor the color object
 */
CGColor CGConstructColor(float r, float g, float b, float alpha);

/**
 * @brief Construct a 2D vector
 * 
 * @param x The x value of the vector
 * @param y The y value of the vector
 * @return CGVector2 The vector that is constructed
 */
CGVector2 CGConstructVector2(float x, float y);

/**
 * @brief Create window object
 * 
 * @param width width of the window.
 * @param height height of the window
 * @param title title of the window
 * @param property window sub property
 * @return CGWindow* The window instance. Returns NULL if failed to create window. 
 * The "width" property and "height" property is set to to the paramiter automatically; however
 * you can set it manually if you don't like the render size of the window. Note that if you 
 * set this property manually, the size of the window will not change, but the render size
 * will change instead. You will find the window not scalling, but the content inside the window
 * scalling. You can change these two properties any time and it will be updated in real-time.
 */
CGWindow* CGCreateWindow(int width, int height, const CGChar* title, CGWindowSubProperty sub_property);

#ifdef CG_TG_WIN
    /**
     * @brief Get the HWND of the window.
     * @warning This function is only available on Windows.
     * @param window The window to get HWND.
     * @return The HWND of the window.
     */
    HWND CGGetWindowHandle(CGWindow* window);
#endif

/**
 * @brief Construct a default window property object.
 * 
 * @return CGWindowSubProperty The default window property object.
 */
CGWindowSubProperty CGConstructDefaultWindowSubProperty();

/**
 * @brief Set the position of the window.
 * 
 * @param window The window that the position is going to be set.
 * @param position The position of the upper left corner of the window in screen cordnate to be set to.
 */
void CGSetWindowPosition(CGWindow* window, CGVector2 position);

/**
 * @brief Get the position of the window.
 * 
 * @param window The window to get position.
 * @return CGVector2 The position of the window.
 */
CGVector2 CGGetWindowPosition(CGWindow* window);

/**
 * @brief Set the callback function for key events.
 * 
 * @param callback The callback function for key events.
 */
void CGSetKeyCallback(CGKeyCallbackFunction callback);

/**
 * @brief Get the callback function for key events.
 * 
 * @return CGKeyCallbackFunction The callback function.
 */
CGKeyCallbackFunction CGGetKeyCallback();

/**
 * @brief Set the callback function for mouse button events.
 * 
 * @param callback The callback function for mouse button events.
 */
void CGSetMouseButtonCallback(CGMouseButtonCallbackFunction callback);

/**
 * @brief Get the callback function for cursor button events.
 * 
 * @return CGCursorButtonCallbackFunction The callback function.
 */
CGMouseButtonCallbackFunction CGGetMouseButtonCallback();

/**
 * @brief Set the callback function for cursor position events.
 * 
 * @param callback The callback function for cursor position events.
 */
void CGSetCursorPositionCallback(CGCursorPositionCallbackFunction callback);

/**
 * @brief Get the callback function for cursor position events.
 * 
 * @return CGCursorPositionCallbackFunction The callback function.
 */
CGCursorPositionCallbackFunction CGGetCursorPositionCallback();

/**
 * @brief Get the cursor position.
 * 
 * @param window The window that the cursor position is going to be get.
 * @return CGVector2 The position of the cursor.
 */
CGVector2 CGGetCursorPosition(CGWindow* window);

/**
 * @brief Create viewport object
 * 
 * @param window window instance
 * @return CGViewport* viewport instance
 */
void CGCreateViewport(CGWindow* window);

/**
 * @brief terminate graphics.
 * 
 */
void CGTerminateGraphics();

/**
 * @brief Set clear screen color
 * 
 * @param color color to be set
 */
void CGSetClearScreenColor(const CGColor color);

/**
 * @brief start the tick render. Call this every frame before the render.
 */
void CGTickRenderStart(CGWindow* window);

/**
 * @brief Draw the window every frame
 * 
 * @param window The window to be drawn
 */
void CGWindowDraw(CGWindow* window);

/**
 * @brief end of the tick render. Call this every frame after the render
 * 
 * @param window window instance
 */
void CGTickRenderEnd();

/**
 * @brief should the window be closed
 * 
 * @param window window instance
 */
CG_BOOL CGShouldWindowClose(CGWindow* window);

/**
 * @brief Get current time
 * 
 * @return double number of seconds after this program is initialized
 */
double CGGetCurrentTime();

/**************SHADER**************/

/**
 * @brief Shader sources
 * 
 */
typedef struct{
    /**
     * @brief Vertex shader source
     * 
     */
    char* vertex;
    /**
     * @brief fragment shader source
     * 
     */
    char* fragment;
    /**
     * @brief geometry shader source
     * 
     */
    char* geometry;
    /**
     * @brief use geometry shader source
     * 
     */
    CG_BOOL use_geometry;
}CGShaderSource;

/**
 * @brief Create a shader source object
 * 
 * @warning If you are using wide character, the shader souces will still needs to be in char type (8 bits) 
 * instead of w_char type. Thus you shouldent wirte your shader in UTF-16 but in UTF-8 instead.
 * 
 * @param vertex vertex source. If you want to set this manually, you can set this parameter to NULL.
 * @param fragment fragment source If you want to set this manually, you can set this parameter to NULL.
 * @param geometry geometry source If you want to set this manually, you can set this parameter to NULL.
 * @param use_geometry use geometry source. If false, this shader source will ignore the geometry source
 * @return CGShaderSource The CGShaderSource object instance. Returns NULL if failed.
 */
CGShaderSource* CGCreateShaderSource(const char* vertex, const char* fragment, 
    const char* geometry, CG_BOOL use_geometry);

/**
 * @brief Create a CGShaderSource object from file
 * 
 * @param vertex_rk The resource key of the vertex shader
 * @param fragment_rk The resource key of the fragment shader
 * @param geometry_rk The resource key of the geometry shader
 * @param use_geometry use geometry source
 * @return CGShaderSource* 
 */
CGShaderSource* CGCreateShaderSourceFromPath(const CGChar* vertex_rk, const CGChar* fragment_rk, 
    const CGChar* geometry_rk, CG_BOOL use_geometry);

/**
 * @brief Shaders
 * 
 */
typedef struct{
    /**
     * @brief vertex shader
     * 
     */
    unsigned int vertex;
    /**
     * @brief fragment shader
     * 
     */
    unsigned int fragment;
    /**
     * @brief geometry shader
     * 
     */
    unsigned int geometry;
    /**
     * @brief use geometry shader
     * 
     */
    CG_BOOL use_geometry;
}CGShader;

/**
 * @brief create a CGShader program from shader_source
 * 
 * @param shader_source shader source
 * @return CGShader shader object with shader ids of the compiled shader source
 */
CGShader* CGCreateShader(CGShaderSource* shader_source);

typedef unsigned int CGShaderProgram;

/**
 * @brief Create shader program
 * 
 * @param shader linked shader object. If you want to link it manually, you should set this to NULL.
 * @return CGShaderProgram shader program
 */
CGShaderProgram CGCreateShaderProgram(CGShader* shader);

/**
 * @brief use shader program
 * 
 * @param program the shader program. If program is 0, the shader program will be set to the default shader program
 */
void CGUseShaderProgram(CGShaderProgram program);

/**
 * @brief Set OpenGL shader 1f uniform
 * 
 * @param shader_program shader program
 * @param uniform_name uniform name
 * @param value value
 */
void CGSetShaderUniform1f(CGShaderProgram shader_program, const char* uniform_name, float value);

/**
 * @brief Set OpenGL shader 1i uniform
 * 
 * @param shader_program shader program
 * @param uniform_name uniform name
 * @param value value
 */
void CGSetShaderUniform1i(CGShaderProgram shader_program, const char* uniform_name, CG_BOOL value);

/**
 * @brief Set OpenGL shader vector 2f uniform
 * 
 * @param shader_program shader program
 * @param uniform_name uniform name
 * @param value value
 */
void CGSetShaderUniformVec2f(CGShaderProgram shader_program, const char* uniform_name, CGVector2 value);

/**
 * @brief Set OpenGL shader vector 4f uniform
 * 
 * @param shader_program shader program
 * @param uniform_name uniform name
 * @param val_1 value 1
 * @param val_2 value 2
 * @param val_3 value 3
 * @param val_4 value 4
 */
void CGSetShaderUniformVec4f(
    CGShaderProgram shader_program, const char* uniform_name,
    float val_1, float val_2, float val_3, float val_4
);

/**
 * @brief Set OpenGL shader matrix 4f uniform
 * 
 * @param shader_program shader program
 * @param uniform_name uniform name
 * @param data uniform data
 */
void CGSetShaderUniformMat4f(CGShaderProgram shader_program, const char* uniform_name, const float* data);

/***********RENDER LIST***********/

enum CGIdentifiers{
    CG_LIST_HEAD = 0,

    CG_RD_TYPE_TRIANGLE,
    CG_RD_TYPE_QUADRANGLE,
    CG_RD_TYPE_VISUAL_IMAGE
};

/************GEOMETRIES************/

/**
 * @brief Object properties for objects that will be rendered on screen.
 */
typedef struct{
    /**
     * @brief rotation of the object (in radians)
     */
    float rotation;
    /**
     * @brief The depth of the object.
     */
    float z;
    /**
     * @brief color of the object
     */
    CGColor color;
    /**
     * @brief transform of the object
     */
    CGVector2 transform;
    /**
     * @brief scale of the object
     */
    CGVector2 scale;
}CGRenderObjectProperty;

/**
 * @brief Create CGRenderObjectProperty object
 * 
 * @param color color
 * @param transform transform
 * @param scale scale
 * @param rotation rotation (clockwise)
 * @return CGRenderObjectProperty* property object instance
 */
CGRenderObjectProperty* CGCreateRenderObjectProperty(CGColor color, CGVector2 transform, CGVector2 scale, float rotation);

/**
 * @brief Rotate a render object
 * 
 * @param property The property of the object. This will be set to the new property after the object is rotated
 * @param rotation The rotation in radius.
 * @param center The center of the rotation.
 */
void CGRotateRenderObject(CGRenderObjectProperty* property, float rotation, CGVector2 center);

/**
 * @brief Register a object to the render list.
 * 
 * @param draw_object the object to be drawn.
 * @param draw_property the property of the object. You can set this to NULL if you want to use the default property.
 * @param window the window to be draw on.
 * @param object_type the type of the object
 */
void CGDraw(void* draw_object, CGRenderObjectProperty* draw_property, CGWindow* window, int object_type);

/**
 * @brief Triangle
 */
typedef struct{
    /**
     * @brief Is this a temporary triangle. If true, the triangle will be automatically deleted after the render.
     * This will be set to CG_FALSE by default.
     */
    CG_BOOL is_temp;
    /**
     * @brief first vertex position
     */
    CGVector2 vert_1;
    /**
     * @brief second vertex position
     */
    CGVector2 vert_2;
    /**
     * @brief third vertex position
     */
    CGVector2 vert_3;
}CGTriangle;

/**
 * @brief Construct a triangle
 * 
 * @param vert_1 vertex 1
 * @param vert_2 vertex 2
 * @param vert_3 vertex 3
 * @return CGTriangle trangle instance
 */
CGTriangle CGConstructTriangle(CGVector2 vert_1, CGVector2 vert_2, CGVector2 vert_3);
/**
 * @brief Create a triangle
 * 
 * @param vert_1 vertex 1
 * @param vert_2 vertex 2
 * @param vert_3 vertex 3
 * @return CGTriangle* triangle instance
 */
CGTriangle* CGCreateTriangle(CGVector2 vert_1, CGVector2 vert_2, CGVector2 vert_3);
/**
 * @brief Create a temporary triangle
 * 
 * @param vert_1 vertex 1
 * @param vert_2 vertex 2
 * @param vert_3 vertex 3
 * @return CGTriangle* triangle instance
 */
CGTriangle* CGCreateTTriangle(CGVector2 vert_1, CGVector2 vert_2, CGVector2 vert_3);

typedef struct{
    /**
     * @brief Is this a temporary triangle. If true, the triangle will be automatically deleted after the render.
     * This will be set to CG_FALSE by default.
     */
    CG_BOOL is_temp;
    /**
     * @brief first vertex position
     */
    CGVector2 vert_1;
    /**
     * @brief second vertex position
     */
    CGVector2 vert_2;
    /**
     * @brief third vertex position
     */
    CGVector2 vert_3;
    /**
     * @brief forth vertex position
     */
    CGVector2 vert_4;
}CGQuadrangle;

/**
 * @brief Construct a CGQuadrangle object
 * 
 * @param vert_1 vertex 1
 * @param vert_2 vertex 2
 * @param vert_3 vertex 3
 * @param vert_4 vertex 4
 * @return CGQuadrangle constructed triangle data
 */
CGQuadrangle CGConstructQuadrangle(CGVector2 vert_1, CGVector2 vert_2, CGVector2 vert_3, CGVector2 vert_4);

/**
 * @brief Create quadrangle object
 * 
 * @param vert_1 vertex 1
 * @param vert_2 vertex 2
 * @param vert_3 vertex 3
 * @param vert_4 vertex 4
 * @return CGQuadrangle* created quadrangle object
 */
CGQuadrangle* CGCreateQuadrangle(CGVector2 vert_1, CGVector2 vert_2, CGVector2 vert_3, CGVector2 vert_4);

/**
 * @brief Create a temporary quadrangle object. A temporary quadrangle object will
 * be automatically deleted after the render.
 * @param vert_1 vertex 1
 * @param vert_2 vertex 2
 * @param vert_3 vertex 3
 * @param vert_4 vertex 4
 * @return CGQuadrangle* created quadrangle object
 */
CGQuadrangle* CGCreateTQuadrangle(CGVector2 vert_1, CGVector2 vert_2, CGVector2 vert_3, CGVector2 vert_4);


/************VISUAL_IMAGES************/

typedef struct{
    /**
     * @brief Is this a temporary triangle. If true, the triangle will be automatically deleted after the render.
     * This will be set to CG_FALSE by default.
     */
    CG_BOOL is_temp;

    /**
     * @brief Is the image clamped.
     */
    CG_BOOL is_clamped;

    /**
     * @brief Texture's OpenGL ID
     */
    unsigned int texture_id;

    /**
     * @brief The width of the texture
     */
    unsigned int img_width;

    /**
     * @brief The height of the texture
     */
    unsigned int img_height;

    /**
     * @brief The number of channels of the texture
     */
    unsigned int img_channels;
    /**
     * @brief The window that the visual_image is created in.
     */
    CGWindow* in_window;

    /**
     * @brief The top left point where the image is going to be clamped.
     */
    CGVector2 clamp_top_left;

    /**
     * @brief The bottom right point where the image is going to be clamped.
     */
    CGVector2 clamp_bottom_right;
}CGVisualImage;

/**
 * @brief Create an OpenGL texture.
 * 
 * @param image The image that the texture is going to be created from.
 * @return unsigned int The texture id of the created texture.
 */
unsigned int CGCreateTexture(CGImage* image);

/**
 * @brief Delete an OpenGL texture.
 * 
 * @param texture_id The texture id of the texture to be deleted.
 */
void CGDeleteTexture(unsigned int texture_id);

/**
 * @brief Create CGVisualImage object
 * 
 * @param img_rk The resource key of the texture.
 * @param window the window that the visual_image is going to be drawn.
 * @return CGVisualImage* The created CGVisualImage object
 */
CGVisualImage* CGCreateVisualImage(const CGChar* img_rk, CGWindow* window);

/**
 * @brief Create a temporary CGVisualImage object. A temporary CGVisualImage object will
 * be automatically deleted after the render.
 * @param img_rk The resource key of the texture.
 * @param window The window that the visual_image is going to be drawn.
 * @return CGVisualImage* The created CGVisualImage object
 */
CGVisualImage* CGCreateTVisualImage(const CGChar* img_rk, CGWindow* window);

/**
 * @brief Copy CGVisualImage object
 * @note The is_temp property will still be set to CG_FALSE no matter what the original value is.
 * If you wish it to be a temporary object, you should set is_temp property manually.
 * @param visual_image The visual_image to be copied.
 * @return CGVisualImage* The copied visual_image
 */
CGVisualImage* CGCopyVisualImage(CGVisualImage* visual_image);

typedef struct {
    /**
     * @brief The width of a space (in pixels)
     */
    unsigned int text_width;
    /**
     * @brief The height of the text (in pixels)
     */
    unsigned int text_height;
    /**
     * @brief The width of a space (in pixels)
     */
    unsigned int space_width;
    /**
     * @brief The space between characters (in pixels) 
     */
    unsigned int kerning;
}CGTextProperty;

/**
 * @brief Construct a CGTextProperty object.
 * 
 * @param text_width The width of a space (in pixels)
 * @param text_height The height of the text (in pixels)
 * @param space_width The widht of a space (in pixels)
 * @return CGTextProperty The constructed CGTextProperty object.
 */
CGTextProperty CGConstructTextProperty(unsigned int text_width, unsigned int text_height, unsigned int space_width, unsigned int kerning);

/**
 * @brief Create a text image.
 * 
 * @bug Unsolved memory leak.
 * @note Try not to make a text a temporary object. It will be very inefficient. Also 
 * currently there's a memory leak bug when you try to create a temporary text image. and
 * I am still trying to fix it. If you want to draw a temporary text, please use @ref CGDrawText
 * instead.
 * 
 * @param text_rk The resource key of the text to be drawn.
 * @param font_rk The key of the font to be used. You can set this to NULL if you want to use the default font. 
 * @param text_property The property of the text.
 * @param window The window that the text is going to be drawn.
 * @return CGVisualImage* The created visual image.
 */
CGVisualImage* CGCreateTextVisualImage(const CGChar* text_rk, const CGChar* font_rk, CGTextProperty text_property, CGWindow* window);

/**
 * @brief Draw a text on the screen. Compare to @ref CGCreateTextVisualImage, this function is more efficient
 * if you only want to draw an image but not getting an image visual image object.
 * @param window The window that the text will be drawn on
 * @param text_rk The resource key of the text.
 * @param font_rk The resource key of the font. If you want to use the default font, you can set this to NULL.
 * @param text_property The property of the text.
 * @param render_property The render property.
 * @return CG_TRUE if the text is successfully drawn. CG_FALSE if failed.
 */
CG_BOOL CGDrawText(const CGChar* text_rk, const CGChar* font_rk, CGTextProperty text_property, const CGRenderObjectProperty* render_property, const CGWindow* window);

#ifdef __cplusplus
}
#endif

#endif  //_CG_GRAPHICS_H_
