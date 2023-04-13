#ifndef _CG_GRAPHICS_H_
#define _CG_GRAPHICS_H_
#include "defs.h"
#include "linked_list.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CG_RENDER_FAR
    /**
     * @brief This value is the smallest assigned z index being started. Note that it does not have any
     * relationship with the z value of each render object. If you have too many objects that needs to 
     * be rendered, and you found out that part of them are not be rendered, you can try to set this to
     * a smaller value.
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
#define CGDrawTriangle(triangle_object, window) CGDraw(triangle_object, window, CG_RD_TYPE_TRIANGLE)

/**
 * @brief Draw quadrangle.
 */
#define CGDrawQuadrangle(quadrangle_object, window) CGDraw(quadrangle_object, window, CG_RD_TYPE_QUADRANGLE)

/**
 * @brief Draw sprite.
 */
#define CGDrawSprite(sprite_object, window) CGDraw(sprite_object, window, CG_RD_TYPE_SPRITE)

typedef CGLinkedListNode CGRenderNode, CGAnimationNode;

/**
 * @brief 2D vector
 */
typedef struct {
    float x;
    float y;
} CGVector2;

/**
 * @brief Window
 */
typedef struct {
    char title[256];
    int width;
    int height;
    CG_BOOL use_full_screen;
    void* glfw_window_instance;
    unsigned int triangle_vao;
    unsigned int quadrangle_vao;
    unsigned int sprite_vao;
    CGRenderNode* render_list;
    CGAnimationNode* animation_list;
} CGWindow;

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

CGVector2 CGConstructVector2(float x, float y);

/**
 * @brief Create window object
 * 
 * @param width width of the window.
 * @param height height of the window
 * @param title title of the window
 * @param use_full_screen use full screen
 * @return CGWindow* The window instance. Returns NULL if failed to create window. 
 * The "width" property and "height" property is set to to the paramiter automatically; however
 * you can set it manually if you don't like the render size of the window. Note that if you 
 * set this property manually, the size of the window will not change, but the render size
 * will change instead. You will find the window not scalling, but the content inside the window
 * scalling. You can change these two properties any time and it will be updated in real-time.
 */
CGWindow* CGCreateWindow(int width, int height,const char* title, CG_BOOL use_full_screen, CG_BOOL resizable);

/**
 * @brief Destroy the window
 * 
 * @param window The window to be destroyed.
 */
void CGDestroyWindow(CGWindow* window);

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
 * @param vertex_path vertex shader path
 * @param fragment_path fragment shader path
 * @param geometry_path geometry shader source
 * @param use_geometry use geometry source
 * @return CGShaderSource* 
 */
CGShaderSource* CGCreateShaderSourceFromPath(const char* vertex_path, const char* fragment_path, 
    const char* geometry_path, CG_BOOL use_geometry);

/**
 * @brief Delete the CGShaderSource object used by the shader source
 * 
 * @param source shader source
 */
void CGDeleteShaderSource(CGShaderSource* shader_source);

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

/**
 * @brief Delete shader object
 * 
 * @param shader shader object to be deleted
 */
void CGDeleteShader(CGShader* shader);

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
 * @brief delete shader program
 * 
 * @param program shader program to be deleted
 */
void CGDeleteShaderProgram(CGShaderProgram program);

/**
 * @brief Set opengl shader 1f uniform
 * 
 * @param shader_program shader program
 * @param uniform_name uniform name
 * @param value value
 */
void CGSetShaderUniform1f(CGShaderProgram shader_program, const char* uniform_name, float value);

/**
 * @brief Set opengl shader vector 4f uniform
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
 * @brief Set opengl shader matrix 4f uniform
 * 
 * @param shader_program shader program
 * @param uniform_name uniform name
 * @param data uniform data
 */
void CGSetShaderUniformMat4f(CGShaderProgram shader_program, const char* uniform_name, const float* data);

/***********RENDER LIST***********/

enum CGIdentifiers{
    CG_RD_TYPE_TRIANGLE = 0,
    CG_RD_TYPE_QUADRANGLE,
    CG_RD_TYPE_SPRITE,

    CG_AN_TYPE_ANIMATION_SPRITE
};

/************GEOMETRIES************/

/**
 * @brief Geometry properties for geometry objects.
 */
typedef struct{
    /**
     * @brief color of the geometry
     */
    CGColor color;
    /**
     * @brief transform of the geometry
     */
    CGVector2 transform;
    /**
     * @brief scale of the geometry
     */
    CGVector2 scale;
    /**
     * @brief rotation of the geometry (in radians)
     */
    float rotation;
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
 * @brief Register a object to the render list.
 * 
 * @param draw_object the object to be drawn.
 * @param window the window to be draw on.
 * @param object_type the type of the object
 */
void CGDraw(void* draw_object, CGWindow* window, int object_type);

/**
 * @brief Triangle
 */
typedef struct{
    /**
     * @brief The depth of the geometry.
     */
    float z;
    /**
     * @brief The assigned z for rendering sortting.
     */
    float assigned_z;
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
     * @brief Geometry properties of the triangle.
     */
    CGRenderObjectProperty* property;
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
 * @brief Set property to a triangle
 * 
 * @param triangle triangle to set on
 * @param property triangle property
 */
void CGSetTriangleProperty(CGTriangle* triangle, CGRenderObjectProperty* property);

typedef struct{
    /**
     * @brief The depth of the geometry.
     */
    float z;
    /**
     * @brief The assigned z for rendering sortting.
     */
    float assigned_z;
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
    /**
     * @brief Geometry property for the quadrangle
     */
    CGRenderObjectProperty* property;
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


/************SPRITES************/

typedef struct{
    /**
     * @brief Texture's OpenGL ID
     */
    unsigned int texture_id;
    /**
     * @brief The depth of the geometry.
     */
    float z;
    /**
     * @brief The assigned z for rendering sortting.
     */
    float assigned_z;
    /**
     * @brief Sprite property.
     */
    CGRenderObjectProperty* property;
    /**
     * @brief The window that the sprite is created in.
     */
    CGWindow* in_window;
    /**
     * @brief the width and height of the sprite
     */
    CGVector2 demention;
}CGSprite;

/**
 * @brief Create CGSprite object
 * 
 * @param img_path texture image path
 * @param property sprite property object
 * @param window the window that the sprite is going to be drawn.
 * @return CGSprite* The created CGSprite object
 */
CGSprite* CGCreateSprite(const char* img_path, CGRenderObjectProperty* property, CGWindow* window);

/**
 * @brief Delete CGSprite object. Note that you have to free the sprite's property manually.
 * 
 * @param sprite sprite object instance to be deleted
 */
void CGDeleteSprite(CGSprite* sprite);

/********ANIMATION SPRITES********/

typedef struct CGAnimationSprite{
    /**
     * @brief The number of frames in this animation sprite object.
     */
    unsigned int frame_count;
    /**
     * @brief The current frame of this animation sprite object.
     */
    unsigned int current_frame;
    /**
     * @brief frame rate.
     */
    float frame_rate;
    /**
     * @brief The depth of the geometry.
     */
    float z;
    /**
     * @brief The assigned z for rendering sortting.
     */
    float assigned_z;
    /**
     * @brief Is animation playing.
     */
    CG_BOOL is_playing;
    /**
     * @brief The node of the animation sprite in the animation sprite list.
     */
    CGAnimationNode* node;
    /**
     * @brief The processing id of the thread for playing the animation sprite.
     */
    unsigned int animation_process_id;
    /**
     * @brief The texture ids of this animation sprite object.
     */
    unsigned int* texture_ids;
    /**
     * @brief Animation sprite property.
     */
    CGRenderObjectProperty* property;
    /**
     * @brief The window that the animation sprite is created in.
     */
    CGWindow* in_window;
    /**
     * @brief the width and height of the animation sprite.
     */
    CGVector2 demention;
    /**
     * @brief Animation finish callback function. This callback function will be called 
     * after the animation is finished.
     * @param anim_sprite The animation sprite object that the animation is finished.
     */
    void (*finish_callback)(struct CGAnimationSprite*);
}CGAnimationSprite;

/**
 * @brief Create CGAnimationSprite object
 * 
 * @param img_paths the list of the animation sprite frame image path.
 * @param frame_count the count of frames of the animated sprite
 * @param property The animation sprite property
 * @param window the window that the animation sprite is going to be drawn.
 * @return CGAnimationSprite* THe created CGAnimationSprite object instance
 */
CGAnimationSprite* CGCreateAnimationSprite(
    const char** img_paths, 
    unsigned int frame_count, 
    float frame_rate,
    CGRenderObjectProperty* property, 
    CGWindow* window);

/**
 * @brief Set animation sprite finish callback function.
 * 
 * @param anim_sprite The animation sprite object to set on
 * @param finish_callback The callback function to be set
 */
void CGSetAnimationSpriteFinishCallback(CGAnimationSprite* anim_sprite, void (*finish_callback)(CGAnimationSprite*));

/**
 * @brief Delete animation sprite object. Note that you have to free the animation sprite's property manually.
 * 
 * @param anim_sprite The animation sprite object to be deleted
 */
void CGDeleteAnimationSprite(CGAnimationSprite* anim_sprite);

/**
 * @brief Draw animation sprite on window
 * 
 * @param anim_sprite The animation sprite object to be drawn
 * @param window The window to draw on
 */
void CGPlayAnimationSprite(CGAnimationSprite* anim_sprite);

#ifdef __cplusplus
}
#endif

#endif  //_CG_GRAPHICS_H_
