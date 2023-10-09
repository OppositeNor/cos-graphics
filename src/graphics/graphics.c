#include "cos_graphics/graphics.h"
#include "cos_graphics/log.h"
#include "cos_graphics/utils.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifdef CG_TG_WIN
    #define GLFW_EXPOSE_NATIVE_WIN32
    #include <GLFW/glfw3native.h>
#endif

#include <math.h>
#include <string.h>
#include <stdlib.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include <freetype/ftbitmap.h>

#define CG_MAX_BUFFER_SIZE 256

#define CGGladInitializeCheck()                                 \
    if (!cg_is_glad_initialized && !cg_is_terminating) {        \
        CG_WARNING(CGSTR("GLAD not initialized yet. Initializing GLAD..."));    \
        CGInitGLAD();                                           \
    }((void)0)

static CG_BOOL cg_is_glfw_initialized = CG_FALSE;
static CG_BOOL cg_is_glad_initialized = CG_FALSE;
static CG_BOOL cg_is_freetype_initialized = CG_FALSE;
static CG_BOOL cg_is_terminating = CG_FALSE;

static FT_Library cg_ft_library;
static FT_Face cg_ft_default_face;

static CGRenderObjectProperty* cg_default_geo_property;
static CGRenderObjectProperty* cg_default_visual_image_property;

enum {
    CG_GL_BUFFERS_TRIANGLE_VBO = 0,
    CG_GL_BUFFERS_QUADRANGLE_VBO,
    CG_GL_BUFFERS_QUADRANGLE_EBO,
    CG_GL_BUFFERS_VISUAL_IMAGE_VBO,
    CG_GL_BUFFERS_VISUAL_IMAGE_EBO,

    CG_GL_BUFFER_COUNT  // buffer counter
};

static unsigned int cg_gl_buffers[CG_GL_BUFFER_COUNT] = {0};

static CGKeyCallbackFunction cg_key_callback = NULL;
static CGMouseButtonCallbackFunction cg_mouse_button_callback = NULL;
static CGCursorPositionCallbackFunction cg_cursor_position_callback = NULL;

static float cg_global_buffer_f20[20];

/**
 * @brief vertex shader resource key for a geometry
 */
static const CGChar* cg_default_geo_vshader_rk = CGSTR("default_geometry_shader_vertex");
/**
 * @brief fragment shader path for a geometry
 */
static const CGChar* cg_default_geo_fshader_rk = CGSTR("default_geometry_shader_fragment");

static const CGChar* cg_default_font_rk = CGSTR("default_font");
/**
 * @brief vertex shader path for a geometry
 */
static const CGChar* cg_default_visual_image_vshader_rk = CGSTR("default_visual_image_shader_vertex");
/**
 * @brief fragment shader path for a geometry
 */
static const CGChar* cg_default_visual_image_fshader_rk = CGSTR("default_visual_image_shader_fragment");

static const CGChar* cg_default_bitmap_visual_image_fshader_rk = CGSTR("default_bitmap_visual_image_shader_fragment");

/**
 * @brief default shader for geometry
 */
static CGShaderProgram cg_default_geo_shader_program;

/**
 * @brief shader program for drawing geometry
 */
static CGShaderProgram cg_geo_shader_program;

/**
 * @brief default shader for visual_images
 */
static CGShaderProgram cg_default_visual_image_shader_program;

/**
 * @brief shader program for drawing visual_images
 */
static CGShaderProgram cg_visual_image_shader_program;

/**
 * @brief default shader for bitmap visual_images
 */
static CGShaderProgram cg_default_bitmap_visual_image_shader_program;

/**
 * @brief shader program for drawing text visual_images
 */
static CGShaderProgram cg_bitmap_visual_image_shader_program;

#define CG_EXTRACT_RENDER_NODE_DATA(node) ((CGRenderNodeData*)node->data)
typedef struct
{
    void* object;
    CGRenderObjectProperty* property;
}CGRenderNodeData;

static const float cg_normal_matrix[16] = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};

typedef CGLinkedListNode CGWindowListNode;

// The list that contains windows that are currently listenning to events.
static CGWindowListNode* cg_window_list = NULL;

// initialize glad
static void CGInitGLAD();

// framebuffer size callback
static void CGFrameBufferSizeCallback(GLFWwindow* window, int width, int height);

// initialize glfw
static void CGInitGLFW(CGWindowSubProperty window_property);

// initialize freetype
static void CGInitFreeType();

// compile one specific shader from source
static CG_BOOL CGCompileShader(unsigned int shader_id, const char* shader_source);

// initialize default shader
static void CGInitDefaultShader(const CGChar* shader_v_rk, const CGChar* shader_f_rk, CGShaderProgram* shader_program);

// make a vertices array out of triangle
static float* CGMakeTriangleVertices(const CGTriangle* triangle, float assigned_z);

// make a vertices array out of quadrangle
static float* CGMakeQuadrangleVertices(const CGQuadrangle* quadrangle, float assigned_z);

// make a vertices array out of visual_image
static float* CGMakeVisualImageVertices(const CGVisualImage* visual_image, float assigned_z);

// set buffer value
static void CGBindBuffer(GLenum buffer_type, unsigned int buffer, unsigned int buffer_size, void* buffer_data, unsigned int usage);

// create transform matrix
static float* CGCreateTransformMatrix(CGVector2 transform);

// create scale matrix
static float* CGCreateScaleMatrix(CGVector2 scale);

// create rotation matrix
static float* CGCreateRotateMatrix(float rotate);

// set geometry matrices uniform
static void CGSetPropertyUniforms(CGShaderProgram shader_program, const CGRenderObjectProperty* property);

// render triangle
static void CGRenderTriangle(const CGTriangle* triangle, const CGRenderObjectProperty* property, const CGWindow* window, float assigned_z);

// render quadrangle
static void CGRenderQuadrangle(const CGQuadrangle* quadrangle, const CGRenderObjectProperty* property, const CGWindow* window, float assigned_z);

// render visual_image
static void CGRenderVisualImage(CGVisualImage* visual_image, const CGRenderObjectProperty* property, CGWindow* window, float assigned_z);

// draw a glyph
static void CGDrawGlyph(int offset, const FT_GlyphSlot glyph, const CGRenderObjectProperty* render_property, const CGWindow* window);

// Set an image data to a texture. Note that if you have a texture that is binded, the texture will be unbinded after you call this function.
static void CGSetTextureValue(unsigned int texture_id, CGImage* texture);

// Write a space to the bitmap. For efficiency, this function will not check if the space is out of bound.
static void CGWriteSpace(unsigned current_x, unsigned int bitmap_width, unsigned int bitmap_height, unsigned int space_width, CGUByte* bitmap);

/**
 * @brief Get the bitmap from a string of text.
 * 
 * @param text The text to get bitmap from.
 * @param face The face of the font.
 * @param text_property The property of the text.
 * @param texture_width This will be set to the width of the texture. You can pass NULL if you don't need this value.
 * @param texture_height This will be set to the height of the texture. You can pass NULL if you don't need this value.
 * @param result This will be set to the OpenGL texture id.
 * @return CG_TRUE if the function succeeds.
 * @return CG_FALSE if the function fails. 
 */
static CG_BOOL CGGetTextTexture(const CGChar* text, FT_Face face, const CGTextProperty* text_property,
    unsigned int* texture_width, unsigned int* texture_height, unsigned int* result);

/**
 * @brief Get the glyph and return the index of a character and return the glyph index. 
 * This function will automatically render the glyph to face.
 * 
 * @param face The face of the font.
 * @param character The character to get glyph index from.
 * @return FT_UInt The glyph index of the character.
 */
static CG_BOOL CGGetGlyphFromFace(FT_Face face, CGChar character);

typedef struct
{
    FT_Glyph glyph_instance;
    int delta_y;
}CGGlyphInstance;

typedef struct
{
    unsigned int glyphs_count;
    CGGlyphInstance* glyph_instances;
    union {
        struct {
            unsigned int total_width;
            unsigned int max_char_height;
        }horizontal_layout;
        struct {
            unsigned int total_height;
            unsigned int max_char_width;
        }vertical_layout;
    }glyphs_dimension;
} CGGlyphs;

/**
 * @brief Get the glyphs from a string of text.
 * 
 * @param face The face of the font.
 * @param text The text string to get glyph from.
 * @param glyphs The result glyphs. You have to allocate and free the glyphs->glyphs_instance manually.
 * @return CG_TRUE if the function succeeds.
 * @return CG_FALSE if the function fails.
 */
static CG_BOOL CGGetTextGlyphs(FT_Face face, const CGChar* text, CGGlyphs* glyphs, const CGTextProperty* text_property);

/**
 * @brief Get the bitmap from glyphs.
 * 
 * @param glyphs The glyphs to get bitmap from.
 * @bitmap The bitmap to store the result.
 * @param text_property The property of the text.
 * @return CG_TRUE if the function succeeds.
 * @return CG_FALSE if the function fails.
 */
static CG_BOOL CGGetGlyphsBitmap(const CGGlyphs* glyphs, CGUByte* bitmap, const CGTextProperty* text_property);

/**
 * @brief Create freetype face.
 * 
 * @param font_rk The resource key of the font.
 * @param face This will be set to the created face.
 * @return CG_TRUE if the function succeeds. CG_FALSE if the function fails.
 */
static CG_BOOL CGCreateFreetypeFace(const CGChar* font_rk, FT_Face* face);

/**
 * @brief Multiply two matrices together (A x B)
 * 
 * @param result The result matrix (needs to allocate memory manually)
 * @param mat_1 Matrix A
 * @param mat_2 Matrix B
 * @param dimension_x The x count of the result matrix
 * @param dimension_y The y count of the result matrix
 * @return float* 
 */
static void CGMatMultiply(float* result, const float* mat_1, const float* mat_2, int dimension_x, int dimension_y);


/**
 * @brief Create a render list based on the window.
 * 
 * @param window the window to create the render list on.
 */
static void CGCreateRenderList(CGWindow* window);

/**
 * @brief Add a render list node in the list.
 * 
 * @param list_head The head of the list. If this parameter is NULL, the program will not do anything.
 * @param node The node to be added into the list
 */
static void CGAddRenderListNode(CGRenderNode* list_head, CGRenderNode* node);


/**
 * @brief Destroy the window
 * 
 * @param window The window to be destroyed.
 */
static void CGDestroyWindow(CGWindow* window);

/**
 * @brief Delete the CGShaderSource object used by the shader source
 * 
 * @param source shader source
 */
static void CGDeleteShaderSource(CGShaderSource* shader_source);

/**
 * @brief Delete shader object
 * 
 * @param shader shader object to be deleted
 */
static void CGDeleteShader(CGShader* shader);

/**
 * @brief delete shader program
 * 
 * @param program shader program to be deleted
 */
static void CGDeleteShaderProgram(CGShaderProgram program);

/**
 * @brief Delete CGVisualImage object.
 * 
 * @param visual_image visual_image object instance to be deleted
 */
static void CGDeleteVisualImage(CGVisualImage* visual_image);

/**
 * @brief Key callback function for GLFW
 * 
 * @param window The window that the key event is triggered on
 * @param key The key that is pressed
 * @param action 
 * @param mods The modifier bits. @see <a href="https://www.glfw.org/docs/latest/group__input.html">GLFW documentation</a> 
 */
static void CGGLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

/**
 * @brief Mouse button callback function for GLFW.
 * 
 * @param window The window that the mouse button event is triggered on.
 * @param button The button that is pressed.
 * @param action The action that is triggered.
 * @param mods The modifier bits. @see <a href="https://www.glfw.org/docs/latest/group__input.html">GLFW documentation</a> 
 */
static void CGGLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

/**
 * @brief Cursor position callback function for GLFW.
 * 
 * @param window The window that the cursor position event is triggered on.
 * @param x The x position of the cursor.
 * @param y The y position of the cursor.
 */
static void CGGLFWCursorPositionCallback(GLFWwindow* window, double x, double y);

float CGVectorCross(CGVector2 vec_1, CGVector2 vec_2)
{
    return vec_1.y * vec_2.x - vec_1.x * vec_2.y;
}

CGColor CGConstructColor(float r, float g, float b, float alpha)
{
    CGColor color;
    color.r = r;
    color.g = g;
    color.b = b;
    color.alpha = alpha;
    return color;
}

CGVector2 CGConstructVector2(float x, float y)
{
    return (CGVector2) { x, y };
}

static void CGFrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
    if (window != glfwGetCurrentContext())
        glfwMakeContextCurrent(window);
    
    CGWindowListNode* p = cg_window_list->next;
    for (; p != NULL && ((CGWindow*)(p->data))->glfw_window_instance != window; p = p->next)
    {
        CG_ERROR_CONDITION(p == NULL, CGSTR("Failed to find window instance in window list."));
    }
    CGWindow* cg_window = (CGWindow*)p->data;
    if (cg_window->glfw_window_instance == window)
    {
        switch (cg_window->sub_property.viewport_scale_mode)
        {
        case CG_VIEWPORT_SCALE_EXPAND:
            glViewport(0, 0, width, height);
            cg_window->width = width;
            cg_window->height = height;
            break;
        case CG_VIEWPORT_SCALE_FILL:
            glViewport(0, 0, width, height);
            break;
        case CG_VIEWPORT_SCALE_KEEP_ASPECT_RATIO:
            if ((float)width / (float)height > cg_window->aspect_ratio)
            {
                int new_width = (int)((float)height * cg_window->aspect_ratio);
                int x_offset = (width - new_width) / 2;
                glViewport(x_offset, 0, new_width, height);
            }
            else
            {
                int new_height = (int)((float)width / cg_window->aspect_ratio);
                int y_offset = (height - new_height) / 2;
                glViewport(0, y_offset, width, new_height);
            }
            break;
        default:
            CG_ERROR_COND_EXIT(CG_TRUE, -1, CGSTR("Viewport scale mode not supported."));
            break;
        }
    }
}

static void CGInitGLFW(CGWindowSubProperty window_sub_property)
{
    CG_ERROR_COND_EXIT(glfwInit() != GLFW_TRUE, -1, CGSTR("GLFW initialization failed"));
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DECORATED, window_sub_property.boarderless ? GLFW_FALSE : GLFW_TRUE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, window_sub_property.transparent);
    glfwWindowHint(GLFW_RESIZABLE, window_sub_property.resizable);
    glfwSwapInterval(0);
    cg_window_list = CGCreateLinkedListNode(NULL, 0);
    cg_is_glfw_initialized = CG_TRUE;
}

static void CGInitFreeType()
{
    CG_ERROR_CONDITION(FT_Init_FreeType(&cg_ft_library), CGSTR("FreeType initialization failed."));
    int file_size;
    CGByte* font_file_data = CGLoadResource(cg_default_font_rk, &file_size, NULL);
    CG_ERROR_CONDITION(font_file_data == NULL, CGSTR("Failed to load default font data."));
    CG_ERROR_CONDITION(FT_New_Memory_Face(cg_ft_library, font_file_data, file_size, 0, &cg_ft_default_face), CGSTR("Failed to create freetype face for default font."));
    CGRegisterResource(font_file_data, CG_DELETER(free));
    cg_is_freetype_initialized = CG_TRUE;
}

void CGTerminateGraphics()
{
    cg_is_terminating = CG_TRUE;
    if (cg_is_glad_initialized)
    {
        CGClearTextureResource();
        glDeleteBuffers(CG_GL_BUFFER_COUNT, cg_gl_buffers);
        glDeleteProgram(cg_default_geo_shader_program);
        glDeleteProgram(cg_default_visual_image_shader_program);
        glDeleteProgram(cg_default_bitmap_visual_image_shader_program);
        CGFree(cg_default_geo_property);
        cg_default_geo_property = NULL;
        CGFree(cg_default_visual_image_property);
        cg_default_visual_image_property = NULL;
        cg_is_glad_initialized = CG_FALSE;
    }
    if (cg_is_glfw_initialized)
    {
        cg_is_glfw_initialized = CG_FALSE;
        for (CGWindowListNode* p = cg_window_list->next; p != NULL; p = p->next)
            CGFree(p->data);
        glfwTerminate();
        CGDeleteList(cg_window_list);
        cg_is_glfw_initialized = CG_FALSE;
    }
    if (cg_is_freetype_initialized)
    {
        FT_Done_Face(cg_ft_default_face);
        FT_Done_FreeType(cg_ft_library);
    }
    if (CGResourceSystemInitialized())
        CGTerminateResourceSystem();
    cg_is_terminating = CG_FALSE;
}

static void CGInitDefaultShader(const CGChar* shader_v_rk, const CGChar* shader_f_rk, CGShaderProgram* shader_program)
{
    CG_ERROR_COND_EXIT(shader_v_rk == NULL || shader_f_rk == NULL, -1, CGSTR("Default shader path cannot be set to NULL."));
    CG_ERROR_COND_EXIT(shader_program == NULL, -1, CGSTR("Cannot init a default shader for NULL shader program."));
    CGShaderSource* shader_source = CGCreateShaderSourceFromPath(shader_v_rk, shader_f_rk, NULL, CG_FALSE);
    CG_ERROR_COND_EXIT(shader_source == NULL, -1, CGSTR("Failed to create shader source."));
    CGShader* shader = CGCreateShader(shader_source);
    CG_ERROR_COND_EXIT(shader == NULL, -1, CGSTR("Failed to init default shader."));
    *shader_program = CGCreateShaderProgram(shader);
    CGFree(shader_source);
    CGFree(shader);
}

static void CGInitGLAD()
{
    CG_ERROR_COND_EXIT(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), -1, CGSTR("GLAD setup OpenGL loader failed"));
    CGInitDefaultShader(cg_default_geo_vshader_rk, cg_default_geo_fshader_rk, &cg_default_geo_shader_program);
    cg_geo_shader_program = cg_default_geo_shader_program;

    cg_default_geo_property = CGCreateRenderObjectProperty(
        CGConstructColor(0.8f, 0.8f, 0.8f, 1.0f), 
        CGConstructVector2(0.0f, 0.0f),
        CGConstructVector2(1.0f, 1.0f),
        0.0f);

    CGInitDefaultShader(cg_default_visual_image_vshader_rk, cg_default_visual_image_fshader_rk, &cg_default_visual_image_shader_program);
    cg_visual_image_shader_program = cg_default_visual_image_shader_program;

    CGInitDefaultShader(cg_default_visual_image_vshader_rk, cg_default_bitmap_visual_image_fshader_rk, &cg_default_bitmap_visual_image_shader_program);
    cg_bitmap_visual_image_shader_program = cg_default_bitmap_visual_image_shader_program;
    
    cg_default_visual_image_property = CGCreateRenderObjectProperty(
        CGConstructColor(1.0f, 1.0f, 1.0f, 1.0f), 
        CGConstructVector2(0.0f, 0.0f),
        CGConstructVector2(1.0f, 1.0f),
        0.0f);
    
    glGenBuffers(CG_GL_BUFFER_COUNT, cg_gl_buffers);
    cg_is_glad_initialized = CG_TRUE;
}

CGWindow* CGCreateWindow(int width, int height, const CGChar* title, CGWindowSubProperty sub_property)
{
    if (!cg_is_glfw_initialized)
        CGInitGLFW(sub_property);
    
    if (!CGResourceSystemInitialized())
        CGInitResourceSystem();

    if (!cg_is_freetype_initialized)
        CGInitFreeType();
    CGWindow* window = (CGWindow*)malloc(sizeof(CGWindow));
    CG_ERROR_COND_RETURN(window == NULL, NULL, CGSTR("Failed to allocate memory for window."));
    window->width = width;
    window->height = height;
    window->aspect_ratio = (float)width / (float)height;
    CG_STRCPY(window->title, title);
    window->sub_property = sub_property;
#ifdef CG_USE_WCHAR
    {
        char title_c[256];
        CGCharToChar(title, title_c, 256);
        window->glfw_window_instance = glfwCreateWindow(width, height, title_c, 
            sub_property.use_full_screen ? glfwGetPrimaryMonitor() : NULL, NULL);
    }
#else
    window->glfw_window_instance = glfwCreateWindow(width, height, title,
        sub_property.use_full_screen ? glfwGetPrimaryMonitor() : NULL, NULL);
#endif
    glfwSetWindowAttrib((GLFWwindow*)window->glfw_window_instance, GLFW_FLOATING, sub_property.topmost);
    window->render_list = NULL;
    CGCreateRenderList(window);
    if (window->glfw_window_instance == NULL)
    {
        CG_ERROR(CGSTR("Failed to create GLFW window."));
        glfwTerminate();
        free(window);
        return NULL;
    }
    glfwSetKeyCallback(window->glfw_window_instance, CGGLFWKeyCallback);
    glfwSetMouseButtonCallback(window->glfw_window_instance, CGGLFWMouseButtonCallback);
    glfwSetCursorPosCallback(window->glfw_window_instance, CGGLFWCursorPositionCallback);

    CGCreateViewport(window);
    CGAppendListNode(cg_window_list, CGCreateLinkedListNode(window, 1));
    CGRegisterResource(window, CG_DELETER(CGDestroyWindow));
    return window;
}

#ifdef CG_TG_WIN
HWND CGGetWindowHandle(CGWindow* window)
{
    return glfwGetWin32Window((GLFWwindow*)(window->glfw_window_instance));
}
#endif

CGWindowSubProperty CGConstructDefaultWindowSubProperty()
{
    CGWindowSubProperty property;
    property.boarderless = CG_FALSE;
    property.resizable = CG_FALSE;
    property.use_full_screen = CG_FALSE;
    property.transparent = CG_FALSE;
    property.topmost = CG_FALSE;
    property.viewport_scale_mode = CG_VIEWPORT_SCALE_KEEP_ASPECT_RATIO;
    return property;
}

void CGSetWindowPosition(CGWindow* window, CGVector2 position)
{
    CG_ERROR_CONDITION(window == NULL, CGSTR("Attempting to set window position on a NULL window."));
    glfwSetWindowPos((GLFWwindow*)window->glfw_window_instance, (int)position.x, (int)position.y);
}

CGVector2 CGGetWindowPosition(CGWindow* window)
{
    CG_ERROR_COND_RETURN(window == NULL, CGConstructVector2(0.0f, 0.0f), CGSTR("Attempting to get window position on a NULL window."));
    int x, y;
    glfwGetWindowPos((GLFWwindow*)window->glfw_window_instance, &x, &y);
    return CGConstructVector2((float)x, (float)y);
}

static void CGGLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (cg_key_callback == NULL)
        return;
    CGWindowListNode* p = cg_window_list->next;
    for (; p != NULL && ((CGWindow*)(p->data))->glfw_window_instance != window; p = p->next)
    {
        CG_ERROR_CONDITION(p == NULL, CGSTR("Failed to find window instance in window list."));
    }
    if (p == NULL)
    {
        CG_WARNING(CGSTR("Failed to find window instance in window list."));
        return;
    }
    cg_key_callback((CGWindow*)(p->data), key, action);
}

static void CGGLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (cg_mouse_button_callback == NULL)
        return;
    CGWindowListNode* p = cg_window_list->next;
    for (; p != NULL && ((CGWindow*)(p->data))->glfw_window_instance != window; p = p->next)
    {
        CG_ERROR_CONDITION(p == NULL, CGSTR("Failed to find window instance in window list."));
    }
    if (p == NULL)
    {
        CG_WARNING(CGSTR("Failed to find window instance in window list."));
        return;
    }
    cg_mouse_button_callback((CGWindow*)(p->data), button, action);
}

static void CGGLFWCursorPositionCallback(GLFWwindow* window, double x, double y)
{
    if (cg_cursor_position_callback == NULL)
        return;
    CGWindowListNode* p = cg_window_list->next;
    for (; p != NULL && ((CGWindow*)(p->data))->glfw_window_instance != window; p = p->next)
    {
        CG_ERROR_CONDITION(p == NULL, CGSTR("Failed to find window instance in window list."));
    }
    if (p == NULL)
    {
        CG_WARNING(CGSTR("Failed to find window instance in window list."));
        return;
    }
    cg_cursor_position_callback((CGWindow*)(p->data), (float)x, (float)y);
}

static void CGDestroyWindow(CGWindow* window)
{
    if (cg_is_glfw_initialized && !cg_is_terminating)
        CGRemoveLinkedListNodeByData(&cg_window_list, window);
    if (cg_is_glad_initialized)
    {
        glDeleteVertexArrays(1, &window->triangle_vao);
        glDeleteVertexArrays(1, &window->quadrangle_vao);
        glDeleteVertexArrays(1, &window->visual_image_vao);
    }
    if (cg_is_glfw_initialized && !cg_is_terminating)
        glfwDestroyWindow((GLFWwindow*)window->glfw_window_instance);
    CGDeleteList(window->render_list);

    free(window);
}

void CGSetKeyCallback(CGKeyCallbackFunction callback)
{
    cg_key_callback = callback;
}

CGKeyCallbackFunction CGGetKeyCallback()
{
    return cg_key_callback;
}

void CGSetMouseButtonCallback(CGMouseButtonCallbackFunction callback)
{
    cg_mouse_button_callback = callback;
}

CGMouseButtonCallbackFunction CGGetMouseButtonCallback()
{
    return cg_mouse_button_callback;
}

void CGSetCursorPositionCallback(CGCursorPositionCallbackFunction callback)
{
    cg_cursor_position_callback = callback;
}

CGCursorPositionCallbackFunction CGGetCursorPositionCallback()
{
    return cg_cursor_position_callback;
}

CGVector2 CGGetCursorPosition(CGWindow* window)
{
    double x, y;
    glfwGetCursorPos((GLFWwindow*)window->glfw_window_instance, &x, &y);
    return CGConstructVector2((float)x - window->width / 2, -1 * ((float)y - window->height / 2));
}

void CGCreateViewport(CGWindow* window)
{
    CG_ERROR_CONDITION(window == NULL || window->glfw_window_instance == NULL, CGSTR("Attempting to create a viewport on a NULL window."));
    if (glfwGetCurrentContext() != window->glfw_window_instance)
        glfwMakeContextCurrent((GLFWwindow*)(window->glfw_window_instance));
    if (!cg_is_glad_initialized)
        CGInitGLAD();

    //glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glViewport(0, 0, window->width, window->height);
    float temp_vertices[20] = {0};

    // set triangle vao properties
    glGenVertexArrays(1, &window->triangle_vao);
    glBindVertexArray(window->triangle_vao);
    CGBindBuffer(GL_ARRAY_BUFFER, cg_gl_buffers[CG_GL_BUFFERS_TRIANGLE_VBO], 9 * sizeof(float), temp_vertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // set quadrangle vao properties
    glGenVertexArrays(1, &window->quadrangle_vao);
    glBindVertexArray(window->quadrangle_vao);
    CGBindBuffer(GL_ARRAY_BUFFER, cg_gl_buffers[CG_GL_BUFFERS_QUADRANGLE_VBO], 12 * sizeof(float), temp_vertices, GL_DYNAMIC_DRAW);
    glGenBuffers(1, &cg_gl_buffers[CG_GL_BUFFERS_QUADRANGLE_EBO]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    unsigned int indices[6] = {0, 1, 2, 0, 2, 3};
    // set visual_image vao properties
    glGenVertexArrays(1, &window->visual_image_vao);
    glBindVertexArray(window->visual_image_vao);
    CGBindBuffer(GL_ARRAY_BUFFER, cg_gl_buffers[CG_GL_BUFFERS_VISUAL_IMAGE_VBO], 20 * sizeof(float), temp_vertices, GL_DYNAMIC_DRAW);
    CGBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cg_gl_buffers[CG_GL_BUFFERS_VISUAL_IMAGE_EBO], 6 * sizeof(unsigned int), indices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    glfwSetFramebufferSizeCallback(window->glfw_window_instance, CGFrameBufferSizeCallback);
}

void CGSetClearScreenColor(const CGColor color)
{
    if (cg_is_glad_initialized)
        glClearColor(color.r, color.g, color.b, color.alpha);
}

void CGTickRenderStart(CGWindow* window)
{
    if (glfwGetCurrentContext() != window->glfw_window_instance)
        glfwMakeContextCurrent((GLFWwindow*)window->glfw_window_instance);
    glfwSwapBuffers(window->glfw_window_instance);
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void CGWindowDraw(CGWindow* window)
{
    if (glfwGetCurrentContext() != window->glfw_window_instance)
        glfwMakeContextCurrent((GLFWwindow*)window->glfw_window_instance);
    CGRenderNode* draw_obj = window->render_list->next;
    CGRenderNodeData *data;
    float assign_z = CG_RENDER_FAR;
    while (draw_obj != NULL)
    {
        assign_z -= 0.1f;
        data = (CGRenderNodeData*)(draw_obj->data);
        switch (draw_obj->identifier)
        {
        case CG_RD_TYPE_TRIANGLE:
            CGRenderTriangle(data->object, data->property, window, assign_z);
            if (((CGTriangle*)(data->object))->is_temp)
                CGFree(data->object);
            break;
        case CG_RD_TYPE_QUADRANGLE:
            CGRenderQuadrangle(data->object, data->property, window, assign_z);
            if (((CGQuadrangle*)(data->object))->is_temp)
                CGFree(data->object);
            break;
        case CG_RD_TYPE_VISUAL_IMAGE:
            CGRenderVisualImage(data->object, data->property, window, assign_z);
            if (((CGVisualImage*)(data->object))->is_temp)
                CGFree(data->object);
            break;
        default:
            CG_ERROR_COND_EXIT(CG_TRUE, -1, CGSTR("Cannot find render object identifier: %d"), draw_obj->identifier);
        }
        free(draw_obj->data);
        CGRemoveLinkedListNode(&draw_obj);
    }
    window->render_list->next = NULL;
}

void CGTickRenderEnd()
{
    CGResourceSystemUpdate();
    //check OpenGL error
    int gl_error_code = glGetError();
    CG_ERROR_COND_EXIT(gl_error_code != GL_NO_ERROR, -1, CGSTR("OpenGL Error: Error code: 0x%x."), gl_error_code);
}

CG_BOOL CGShouldWindowClose(CGWindow* window)
{
    return (CG_BOOL)glfwWindowShouldClose(window->glfw_window_instance);
}

double CGGetCurrentTime()
{
    return glfwGetTime();
}

CGShaderSource* CGCreateShaderSource(const char* vertex, const char* fragment, 
    const char* geometry, CG_BOOL use_geometry)
{
    CG_ERROR_COND_RETURN(vertex == NULL || fragment == NULL, NULL, CGSTR("Shader source cannot be NULL."));
    CGShaderSource* result = (CGShaderSource*)malloc(sizeof(CGShaderSource));
    if (result == NULL)
    {
        CG_ERROR(CGSTR("Construct shader source failed"));
        return NULL;
    }
    strcpy(result->vertex, vertex);
    strcpy(result->fragment, fragment);
    if (use_geometry)
        strcpy(result->geometry, geometry);
    result->use_geometry = use_geometry;
    CGRegisterResource(result, CG_DELETER(CGDeleteShaderSource));
    return result;
}

CGShaderSource* CGCreateShaderSourceFromPath(const CGChar* vertex_rk, const CGChar* fragment_rk, 
    const CGChar* geometry_rk, CG_BOOL use_geometry)
{
    CGShaderSource* result = (CGShaderSource*)malloc(sizeof(CGShaderSource));
    if (result == NULL)
    {
        CG_ERROR(CGSTR("Construct shader source failed."));
        return NULL;
    }
    result->vertex = CGLoadResource(vertex_rk, NULL, NULL);
    if (result->vertex == NULL)
    {
        CG_ERROR(CGSTR("Load vertex shader source failed."));
        free(result);
        return NULL;
    }
    result->fragment = CGLoadResource(fragment_rk, NULL, NULL);
    if (result->fragment == NULL)
    {
        CG_ERROR(CGSTR("Load fragment shader source failed."));
        free(result->vertex);
        free(result);
        return NULL;
    }
    if (use_geometry)
    {
        result->geometry = CGLoadResource(geometry_rk, NULL, NULL);
        if (result->geometry == NULL)
        {
            CG_ERROR(CGSTR("Load geometry shader source failed."));
            free(result->vertex);
            free(result->fragment);
            free(result);
            return NULL;
        }
    }
    else
        result->geometry = NULL;
    result->use_geometry = use_geometry;
    CGRegisterResource(result, CG_DELETER(CGDeleteShaderSource));
    return result;
}

static void CGDeleteShaderSource(CGShaderSource* shader_source)
{
    if (shader_source != NULL)
    {
        free(shader_source->vertex);
        free(shader_source->fragment);
        free(shader_source->geometry);
        free(shader_source);
        shader_source = NULL;
    }
}

static CG_BOOL CGCompileShader(unsigned int shader_id, const char* shader_source)
{
    CG_ERROR_COND_RETURN(shader_source == NULL, CG_FALSE, CGSTR("Attempting to compile a shader with a NULL source."));
    glShaderSource(shader_id, 1, &shader_source, NULL);
    glCompileShader(shader_id);

    int success;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);

    char info_log[CG_INFO_LOG_SIZE];

    if (!success)
    {
        glGetShaderInfoLog(shader_id, CG_INFO_LOG_SIZE, NULL, info_log);
#ifdef CG_USE_WCHAR
        CGChar info_log_w[CG_INFO_LOG_SIZE];
        CharToCGChar(info_log, info_log_w, CG_INFO_LOG_SIZE);
        CG_ERROR(CGSTR("Failed to compile shader with id: %d. \nOutput log: %ls."), shader_id, info_log_w);
#else
        CG_ERROR(CGSTR("Failed to compile shader with id: %d. \nOutput log: %s."), shader_id, info_log);
#endif
        return CG_FALSE;
    }
    return CG_TRUE;
}

CGShader* CGCreateShader(CGShaderSource* shader_source)
{
    CG_ERROR_COND_RETURN(shader_source == NULL, NULL, CGSTR("Attempting to compile a NULL shader source."));
    CGShader* shader = (CGShader*)malloc(sizeof(CGShader));
    CG_ERROR_COND_RETURN(shader == NULL, NULL, CGSTR("Construct shader failed."));
    shader->vertex = glCreateShader(GL_VERTEX_SHADER);
    if (!CGCompileShader(shader->vertex, shader_source->vertex))
    {
        CG_ERROR(CGSTR("Failed to compile vertex shader."));
        return NULL;
    }
    shader->fragment = glCreateShader(GL_FRAGMENT_SHADER);
    if (!CGCompileShader(shader->fragment, shader_source->fragment))
    {
        CG_ERROR(CGSTR("Failed to compile fragment shader."));
        return NULL;
    }
    shader->use_geometry = shader_source->use_geometry;
    if (!shader_source->use_geometry)
    {
        shader_source->geometry = NULL;
        CGRegisterResource(shader, CG_DELETER(CGDeleteShader));
        return shader;
    }
    shader->geometry = glCreateShader(GL_GEOMETRY_SHADER);
    if (!CGCompileShader(shader->geometry, shader_source->geometry))
    {
        CG_ERROR(CGSTR("Failed to compile geometry shader."));
        return NULL;
    }
    CGRegisterResource(shader, CG_DELETER(CGDeleteShader));
    return shader;
}

static void CGDeleteShader(CGShader* shader)
{
    glDeleteShader(shader->vertex);
    glDeleteShader(shader->fragment);
    if (shader->use_geometry)
        glDeleteShader(shader->geometry);
    free(shader);
    shader = NULL;
}

CGShaderProgram CGCreateShaderProgram(CGShader* shader)
{
    CGShaderProgram result = glCreateProgram();
    if (shader != NULL)
    {
        glAttachShader(result, shader->vertex);
        glAttachShader(result, shader->fragment);
        if (shader->use_geometry)
            glAttachShader(result, shader->geometry);
        glLinkProgram(result);
    }
    return result;
}

void CGUseShaderProgram(CGShaderProgram program)
{
    if (program != 0)
        cg_geo_shader_program = program;
    else
        cg_geo_shader_program = cg_default_geo_shader_program;
}

static void CGDeleteShaderProgram(CGShaderProgram program)
{
    glDeleteProgram(program);
}

void CGSetShaderUniform1f(CGShaderProgram shader_program, const char* uniform_name, float value)
{
    CG_ERROR_CONDITION(uniform_name == NULL, CGSTR("Attempting to set a uniform with a NULL name."));
    CGGladInitializeCheck();
    GLint uniform_location = glGetUniformLocation(shader_program, uniform_name);
    glUniform1f(uniform_location, value);
}

void CGSetShaderUniform1i(CGShaderProgram shader_program, const char* uniform_name, CG_BOOL value)
{
    CG_ERROR_CONDITION(uniform_name == NULL, CGSTR("Attempting to set a uniform with a NULL name."));
    CGGladInitializeCheck();
    GLint uniform_location = glGetUniformLocation(shader_program, uniform_name);
    glUniform1i(uniform_location, value);
}

void CGSetShaderUniformVec2f(CGShaderProgram shader_program, const char* uniform_name, CGVector2 value)
{
    CG_ERROR_CONDITION(uniform_name == NULL, CGSTR("Attempting to set a uniform with a NULL name."));
    CGGladInitializeCheck();
    GLint uniform_location = glGetUniformLocation(shader_program, uniform_name);
    glUniform2f(uniform_location, value.x, value.y);
}

void CGSetShaderUniformVec4f(
    CGShaderProgram shader_program, const char* uniform_name,
    float val_1, float val_2, float val_3, float val_4)
{
    CGGladInitializeCheck();
    CG_ERROR_CONDITION(uniform_name == NULL, CGSTR("Attempting to set a uniform with a NULL name."));
    GLint uniform_location = glGetUniformLocation(shader_program, uniform_name);
    glUniform4f(uniform_location, val_1, val_2, val_3, val_4);
}

void CGSetShaderUniformMat4f(CGShaderProgram shader_program, const char* uniform_name, const float* data)
{
    CGGladInitializeCheck();
    CG_ERROR_CONDITION(uniform_name == NULL, CGSTR("Attempting to set a uniform with a NULL name."));
    GLint uniform_location = glGetUniformLocation(shader_program, uniform_name);
    glUniformMatrix4fv(uniform_location, 1, GL_FALSE, data);
}

void CGDraw(void* draw_object, CGRenderObjectProperty* draw_property, CGWindow* window, int object_type)
{
    CGRenderNodeData* data = (CGRenderNodeData*)malloc(sizeof(CGRenderNodeData));
    CG_ERROR_CONDITION(data == NULL, CGSTR("Failed to allocate memory for draw object data."));
    data->object = draw_object;
    data->property = draw_property;
    CGAddRenderListNode(window->render_list, CGCreateLinkedListNode(data, object_type));
}

static void CGCreateRenderList(CGWindow* window)
{
    CG_ERROR_COND_EXIT(window == NULL, -1, CGSTR("Failed to create render list: Window must be specified to a non-null window instance."));
    if (window->render_list != NULL)
        free(window->render_list);
    window->render_list = CGCreateLinkedListNode(NULL, CG_LIST_HEAD);
}

static void CGAddRenderListNode(CGRenderNode* list_head, CGRenderNode* node)
{
    if (node == NULL || list_head == NULL)
        return;
    // using list_head as an temperary variable
    while (list_head->next != NULL)
    {
        float current_z = 0;
        if (CG_EXTRACT_RENDER_NODE_DATA(node)->property != NULL)
            current_z = CG_EXTRACT_RENDER_NODE_DATA(node)->property->z;
        float next_z = 0;
        if (CG_EXTRACT_RENDER_NODE_DATA(list_head->next)->property != NULL)
            next_z = CG_EXTRACT_RENDER_NODE_DATA(list_head->next)->property->z;
        if (current_z > next_z)
        {
            CGRenderNode* temp = list_head->next;
            list_head->next = node;
            node->next = temp;
            return;
        }
        list_head = list_head->next;
    }
    CGRenderNode* temp = list_head->next;
    list_head->next = node;
    node->next = temp;
    
}

CGRenderObjectProperty* CGCreateRenderObjectProperty(CGColor color, CGVector2 transform, CGVector2 scale, float rotation)
{
    CGRenderObjectProperty* property = (CGRenderObjectProperty*)malloc(sizeof(CGRenderObjectProperty));
    CG_ERROR_COND_RETURN(property == NULL, NULL, CGSTR("Failed to allocate memory for CGRenderObjectProperty."));
    property->color = color;
    property->transform = transform;
    property->scale = scale;
    property->rotation = rotation;
    property->z = 0;
    property->modify_matrix = NULL;
    CGRegisterResource(property, CG_DELETER(free));
    return property;
}

void CGRotateRenderObject(CGRenderObjectProperty* property, float rotation, CGVector2 center)
{
    property->rotation += rotation;
    float sin_theta = (float)sin(rotation);
    float cos_theta = (float)cos(rotation);
    float delta_x = property->transform.x - center.x;
    float delta_y = property->transform.y - center.y;
    property->transform.x = delta_x * cos_theta - delta_y * sin_theta + center.x;
    property->transform.y = delta_y * cos_theta + delta_x * sin_theta + center.y;
}

static float* CGCreateTransformMatrix(CGVector2 transform)
{
    float* result = (float*)malloc(sizeof(float) * 16);
    CG_ERROR_COND_RETURN(result == NULL, NULL, CGSTR("Failed to allocate memory for transform matrix."));
    memcpy(result, cg_normal_matrix, sizeof(float) * 16);
    result[12] = transform.x;
    result[13] = transform.y;
    
    return result;
}

static float* CGCreateScaleMatrix(CGVector2 scale)
{
    float* result = (float*)malloc(sizeof(float) * 16);
    CG_ERROR_COND_RETURN(result == NULL, NULL, CGSTR("Failed to allocate memory for scale matrix."));
    memcpy(result, cg_normal_matrix, sizeof(float) * 16);
    result[0] = scale.x;
    result[5] = scale.y;
    return result;
}

static float* CGCreateRotateMatrix(float rotate)
{
    float* result = (float*)malloc(sizeof(float) * 16);
    CG_ERROR_COND_RETURN(result == NULL, NULL, CGSTR("Failed to allocate memory for rotation matrix."));
    memcpy(result, cg_normal_matrix, sizeof(float) * 16);
    if (rotate == 0)
        return result;
    float sin_theta = (float)sin(rotate), cos_theta = (float)cos(rotate);
    result[0] = cos_theta;
    result[1] = sin_theta;
    result[4] = -1 * sin_theta;
    result[5] = cos_theta;
    return result;
}

static void CGMatMultiply(float* result, const float* mat_1, const float* mat_2, int dimension_x, int dimension_y)
{
    CG_ERROR_CONDITION(result == NULL || mat_1 == NULL || mat_2 == NULL, CGSTR("Unable to multiply NULL matrix"));
    float *temp1 = NULL, *temp2 = NULL;
    if (mat_1 == result)
    {
        temp1 = (float*)malloc(sizeof(float) * dimension_x * dimension_y);
        CG_ERROR_CONDITION(temp1 == NULL, CGSTR("Failed to allocate memory for temp matrix."));
        memcpy(temp1, mat_1, sizeof(float) * dimension_x * dimension_y);
        mat_1 = temp1;
    }
    if (mat_2 == result)
    {
        temp2 = (float*)malloc(sizeof(float) * dimension_x * dimension_y);
        if (temp2 == NULL)
        {
            if (temp1 != NULL)
                free(temp1);
            CG_ERROR_CONDITION(CG_TRUE, CGSTR("Failed to allocate memory for temp matrix."));
        }
        memcpy(temp2, mat_2, sizeof(float) * dimension_x * dimension_y);
        mat_2 = temp2;
    }
    for (int i = 0; i < dimension_y; ++i)
    {
        for (int j = 0; j < dimension_x; ++j)
        {
            float value = 0;
            for (int k = 0; k < dimension_x; ++k)
            {
                value += mat_1[i * dimension_x + k] * mat_2[k * dimension_y + j];
            }
            result[i * dimension_x + j] = value;
        }
    }
    if (temp1 != NULL)
    {
        free(temp1);
        temp1 = NULL;
    }
    if (temp2 != NULL)
    {
        free(temp2);
        temp2 = NULL;
    }
}

static void CGSetPropertyUniforms(CGShaderProgram shader_program, const CGRenderObjectProperty* property)
{
    CG_ERROR_CONDITION(property == NULL, CGSTR("Attempting to set uniforms out of a NULL property"));
    CGSetShaderUniformVec4f(shader_program, "color", 
        property->color.r, property->color.g, property->color.b, property->color.alpha);
    float result[16] = {0};
    if (property->modify_matrix != NULL)
    {
        memcpy(result, property->modify_matrix, sizeof(float) * 16);
    }
    else
        memcpy(result, cg_normal_matrix, sizeof(float) * 16);

    float* tmp_mat = CGCreateTransformMatrix(property->transform);
    if (tmp_mat != NULL)
    {
        CGMatMultiply(result, tmp_mat, result, 4, 4);
        free(tmp_mat);
    }
    else
        memcpy(result, cg_normal_matrix, sizeof(float) * 16);

    tmp_mat = CGCreateRotateMatrix(property->rotation);
    if (tmp_mat != NULL)
    {
        CGMatMultiply(result, tmp_mat, result, 4, 4);
        free(tmp_mat);
    }

    tmp_mat = CGCreateScaleMatrix(property->scale);
    if (tmp_mat != NULL)
    {
        CGMatMultiply(result, tmp_mat, result, 4, 4);
        free(tmp_mat);
    }
    CGSetShaderUniformMat4f(shader_program, "model_mat", result);
}

CGTriangle CGConstructTriangle(CGVector2 vert_1, CGVector2 vert_2, CGVector2 vert_3)
{
    CGTriangle result;
    result.vert_1 = vert_1;
    result.vert_2 = vert_2;
    result.vert_3 = vert_3;
    result.is_temp = CG_FALSE;
    return result;
}

CGTriangle* CGCreateTriangle(CGVector2 vert_1, CGVector2 vert_2, CGVector2 vert_3)
{
    CGTriangle* result = (CGTriangle*)malloc(sizeof(CGTriangle));
    CG_ERROR_COND_RETURN(result == NULL, NULL, CGSTR("Failed to allocate memory for triangle."));
    result->vert_1 = vert_1;
    result->vert_2 = vert_2;
    result->vert_3 = vert_3;
    result->is_temp = CG_FALSE;
    CGRegisterResource(result, free);
    return result;
}

CGTriangle* CGCreateTTriangle(CGVector2 vert_1, CGVector2 vert_2, CGVector2 vert_3)
{
    CGTriangle* result = (CGTriangle*)malloc(sizeof(CGTriangle));
    CG_ERROR_COND_RETURN(result == NULL, NULL, CGSTR("Failed to allocate memory for triangle."));
    result->vert_1 = vert_1;
    result->vert_2 = vert_2;
    result->vert_3 = vert_3;
    result->is_temp = CG_TRUE;
    CGRegisterResource(result, free);
    return result;
}

static float* CGMakeTriangleVertices(const CGTriangle* triangle, float assigned_z)
{
    CG_ERROR_COND_RETURN(triangle == NULL, NULL, CGSTR("Cannot make vertices array out of a triangle of value NULL."));
    static const float denom = (CG_RENDER_FAR - CG_RENDER_NEAR);
    float depth = (assigned_z - CG_RENDER_NEAR) / denom;
    CGSetFloatArrayValue(9, cg_global_buffer_f20,
        triangle->vert_1.x, triangle->vert_1.y, depth,
        triangle->vert_2.x, triangle->vert_2.y, depth,
        triangle->vert_3.x, triangle->vert_3.y, depth
    );
    return cg_global_buffer_f20;
}

static void CGBindBuffer(GLenum buffer_type, unsigned int buffer, unsigned int buffer_size, void* buffer_data, unsigned int usage)
{
    CGGladInitializeCheck();
    glBindBuffer(buffer_type, buffer);
    glBufferData(buffer_type, buffer_size, buffer_data, usage);
}

static void CGRenderTriangle(const CGTriangle* triangle, const CGRenderObjectProperty* property, const CGWindow* window, float assigned_z)
{
    CG_ERROR_CONDITION(window == NULL || window->glfw_window_instance == NULL, CGSTR("Attempting to draw triangle on a NULL window."));
    CG_ERROR_CONDITION(triangle == NULL, CGSTR("Attempting to draw a NULL triangle object."));
    float* triangle_vertices = CGMakeTriangleVertices(triangle, assigned_z);
    CG_ERROR_CONDITION(triangle_vertices == NULL, CGSTR("Failed to draw triangle."));
    CGGladInitializeCheck();
    if (glfwGetCurrentContext() != window->glfw_window_instance)
        glfwMakeContextCurrent(window->glfw_window_instance);
    if (property == NULL)
        property = cg_default_geo_property;

    //draw
    glBindVertexArray(window->triangle_vao);
    glUseProgram(cg_geo_shader_program);
    glBindBuffer(GL_ARRAY_BUFFER, cg_gl_buffers[CG_GL_BUFFERS_TRIANGLE_VBO]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 9 * sizeof(float), triangle_vertices);

    CGSetPropertyUniforms(cg_geo_shader_program, property);
    CGSetShaderUniform1f(cg_geo_shader_program, "render_width", (float)window->width / 2.0f);
    CGSetShaderUniform1f(cg_geo_shader_program, "render_height", (float)window->height / 2.0f);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static float* CGMakeQuadrangleVertices(const CGQuadrangle* quadrangle, float assigned_z)
{
    CG_ERROR_COND_RETURN(quadrangle == NULL, NULL, CGSTR("Cannot make vertices array out of a quadrangle of value NULL."));
    float* vertices = cg_global_buffer_f20;
    float depth = (assigned_z - CG_RENDER_NEAR) / (CG_RENDER_FAR - CG_RENDER_NEAR);
    CGSetFloatArrayValue(12, vertices, 
        quadrangle->vert_1.x, quadrangle->vert_1.y, depth,
        quadrangle->vert_2.x, quadrangle->vert_2.y, depth,
        quadrangle->vert_3.x, quadrangle->vert_3.y, depth,
        quadrangle->vert_4.x, quadrangle->vert_4.y, depth
    );
    return vertices;
}

static float* CGMakeVisualImageVertices(const CGVisualImage* visual_image, float assigned_z)
{
    CG_ERROR_COND_RETURN(visual_image == NULL, NULL, CGSTR("Cannot make vertices array out of a visual_image of value NULL"));
    float depth = (assigned_z - CG_RENDER_NEAR) / (CG_RENDER_FAR - CG_RENDER_NEAR);
    double temp_half_width = (double)visual_image->img_width / 2;
    double temp_half_height = (double)visual_image->img_height / 2;
    CGSetFloatArrayValue(20, cg_global_buffer_f20,
        -1 * temp_half_width,      temp_half_height, depth, 0.0, 0.0,
             temp_half_width,      temp_half_height, depth, 1.0, 0.0,
             temp_half_width, -1 * temp_half_height, depth, 1.0, 1.0,
        -1 * temp_half_width, -1 * temp_half_height, depth, 0.0, 1.0
    );
    return cg_global_buffer_f20;
}

CGQuadrangle CGConstructQuadrangle(CGVector2 vert_1, CGVector2 vert_2, CGVector2 vert_3, CGVector2 vert_4)
{
    CGQuadrangle result;
    result.vert_1 = vert_1;
    result.vert_2 = vert_2;
    result.vert_3 = vert_3;
    result.vert_4 = vert_4;
    result.is_temp = CG_FALSE;
    return result;
}

CGQuadrangle* CGCreateQuadrangle(CGVector2 vert_1, CGVector2 vert_2, CGVector2 vert_3, CGVector2 vert_4)
{
    CGQuadrangle* result = (CGQuadrangle*)malloc(sizeof(CGQuadrangle));
    CG_ERROR_COND_RETURN(result == NULL, NULL, CGSTR("Failed to allocate memory for CGQuadrangle object"));
    result->vert_1 = vert_1;
    result->vert_2 = vert_2;
    result->vert_3 = vert_3;
    result->vert_4 = vert_4;
    result->is_temp = CG_FALSE;
    CGRegisterResource(result, free);
    return result;
}

CGQuadrangle* CGCreateTQuadrangle(CGVector2 vert_1, CGVector2 vert_2, CGVector2 vert_3, CGVector2 vert_4)
{
    CGQuadrangle* result = (CGQuadrangle*)malloc(sizeof(CGQuadrangle));
    CG_ERROR_COND_RETURN(result == NULL, NULL, CGSTR("Failed to allocate memory for CGQuadrangle object"));
    result->vert_1 = vert_1;
    result->vert_2 = vert_2;
    result->vert_3 = vert_3;
    result->vert_4 = vert_4;
    result->is_temp = CG_TRUE;
    CGRegisterResource(result, free);
    return result;
}

static void CGRenderQuadrangle(const CGQuadrangle* quadrangle, const CGRenderObjectProperty* property, const CGWindow* window, float assigned_z)
{
    CG_ERROR_CONDITION(window == NULL || window->glfw_window_instance == NULL, CGSTR("Cannot draw quadrangle on a NULL window."));
    CG_ERROR_CONDITION(quadrangle == NULL, CGSTR("Attempting to draw a NULL quadrangle."));
    CGGladInitializeCheck();
    float* vertices = CGMakeQuadrangleVertices(quadrangle, assigned_z);
    CG_ERROR_CONDITION(vertices == NULL, CGSTR("Failed to draw quadrangle."));
    if (property == NULL)
        property = cg_default_geo_property;

    CG_BOOL has_reflex = CG_FALSE;
    unsigned int indices[6];
    // triangulate
    for (int i = 0; i < 4; ++i)
    {
        if (CGVectorCross(quadrangle->vertices[((i - 1) + 4) % 4], quadrangle->vertices[(i + 1) % 4]) > 0)
        {
            has_reflex = CG_TRUE;
            indices[0] = i, indices[1] = (i + 1) % 4, indices[2] = (i + 2) % 4;
            indices[3] = i, indices[4] = (i + 2) % 4, indices[5] = (i + 3) % 4;
        }
    }
    if (!has_reflex)
    {
        indices[0] = 0, indices[1] = 1, indices[2] = 2;
        indices[3] = 0, indices[4] = 2, indices[5] = 3;
    }
    
    //draw
    glBindVertexArray(window->quadrangle_vao);
    glUseProgram(cg_default_geo_shader_program);
    glBindBuffer(GL_ARRAY_BUFFER, cg_gl_buffers[CG_GL_BUFFERS_QUADRANGLE_VBO]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 12, vertices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cg_gl_buffers[CG_GL_BUFFERS_QUADRANGLE_EBO]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indices, GL_DYNAMIC_DRAW);
    CGSetPropertyUniforms(cg_geo_shader_program, property);
    CGSetShaderUniform1f(cg_geo_shader_program, "render_width", (float)window->width / 2.0f);
    CGSetShaderUniform1f(cg_geo_shader_program, "render_height", (float)window->height / 2.0f);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void CGSetTextureValue(unsigned int texture_id, CGImage* texture)
{
    CG_ERROR_CONDITION(texture == NULL, CGSTR("Cannot bind a NULL texture."));
    CGGladInitializeCheck();
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    switch(texture->channels)
    {
    case 1:
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, texture->width, texture->height, 0, GL_RED, GL_UNSIGNED_BYTE, texture->data);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    case 3:
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->width, texture->height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture->data);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        break;
    case 4:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->data);
        break;
    default:
        glBindTexture(GL_TEXTURE_2D, 0);
        CG_ERROR_CONDITION(CG_TRUE, CGSTR("Invalid image channel count. CosGraphics currently only supports images with 3 or 4 channels."));
    }
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned int CGCreateTexture(CGImage* image)
{
    CG_ERROR_COND_RETURN(image == NULL, 0, CGSTR("Cannot create texture from NULL image."));
    CGGladInitializeCheck();
    unsigned int texture_id;
    glGenTextures(1, &texture_id);
    CGSetTextureValue(texture_id, image);
    return texture_id;
}

void CGDeleteTexture(unsigned int texture_id)
{
    CGGladInitializeCheck();
    glDeleteTextures(1, &texture_id);
}

CGVisualImage* CGCreateVisualImage(const CGChar* img_rk, CGWindow* window)
{
    CG_ERROR_COND_RETURN(img_rk == NULL, NULL, CGSTR("Cannot create image with NULL texture path."));
    CG_ERROR_COND_RETURN(window == NULL || window->glfw_window_instance == NULL, NULL, CGSTR("Cannot create image with NULL window."));
    CGGladInitializeCheck();
    CGVisualImage* visual_image = (CGVisualImage*)malloc(sizeof(CGVisualImage));
    CG_ERROR_COND_RETURN(visual_image == NULL, NULL, CGSTR("Failed to allocate memory for visual_image."));
    if (glfwGetCurrentContext() != window->glfw_window_instance)
        glfwMakeContextCurrent(window->glfw_window_instance);
    visual_image->in_window = window;
    visual_image->is_temp = CG_FALSE;
    CGImage* image = CGLoadImageFromResource(img_rk);
    CG_ERROR_COND_RETURN(image == NULL, NULL, CGSTR("Failed to create visual_image."));
    visual_image->img_width = image->width;
    visual_image->img_height = image->height;
    visual_image->img_channels = image->channels;
    visual_image->clamp_top_left = (CGVector2){0.0f, 0.0f};
    visual_image->clamp_bottom_right = (CGVector2){image->width, image->height};
    visual_image->is_clamped = CG_FALSE;
    if (image == NULL)
    {
        free(visual_image);
        CG_ERROR_COND_RETURN(CG_TRUE, NULL, CGSTR("Failed to allocate memory for visual_image texture."));
    }
    visual_image->texture_id = CGGetTextureResource(img_rk);
    CGFree(image);
    CGRegisterResource(visual_image, CG_DELETER(CGDeleteVisualImage));
    return visual_image;
}

CGVisualImage* CGCreateTVisualImage(const CGChar* img_rk, CGWindow* window)
{
    CGVisualImage* result = CGCreateVisualImage(img_rk, window);
    CG_ERROR_COND_RETURN(result == NULL, NULL, CGSTR("Failed to create visual_image."));
    result->is_temp = CG_TRUE;
    return result;
}

CGVisualImage* CGCopyVisualImage(CGVisualImage* visual_image)
{
    CG_ERROR_COND_RETURN(visual_image == NULL, NULL, CGSTR("Cannot copy a NULL visual_image."));
    CGGladInitializeCheck();
    CGVisualImage* result = (CGVisualImage*)malloc(sizeof(CGVisualImage));
    CG_ERROR_COND_RETURN(result == NULL, NULL, CGSTR("Failed to allocate memory for visual image."));
    result->img_width = visual_image->img_width;
    result->img_height = visual_image->img_height;
    result->img_channels = visual_image->img_channels;
    result->clamp_top_left = visual_image->clamp_top_left;
    result->clamp_bottom_right = visual_image->clamp_bottom_right;
    result->is_clamped = visual_image->is_clamped;
    result->is_temp = CG_FALSE;
    CGWindow* in_window = result->in_window = visual_image->in_window;
    if (glfwGetCurrentContext() != in_window->glfw_window_instance)
        glfwMakeContextCurrent(in_window->glfw_window_instance);
    
    result->texture_id = CGCopyTextureResource(visual_image->texture_id);

    CGRegisterResource(result, CG_DELETER(CGDeleteVisualImage));
    return result;
}

static CG_BOOL CGGetGlyphFromFace(FT_Face face, CGChar character)
{ 
#ifdef CG_USE_WCHAR
    CG_ERROR_COND_RETURN(FT_Load_Char(face, character, FT_LOAD_RENDER), CG_FALSE, CGSTR("Glyph index with char: \'%lc\' (unicode: %#x) not found."), character, character);
#else
    CG_ERROR_COND_RETURN(FT_Load_Char(face, character, FT_LOAD_RENDER), CG_FALSE, CGSTR("Glyph index with char: \'%c\' (unicode: %#x) not found."), character, character);
#endif
    return CG_TRUE;
}


static CG_BOOL CGGetTextGlyphs(FT_Face face, const CGChar* text, CGGlyphs* glyphs, const CGTextProperty* text_property)
{
    CG_ERROR_COND_RETURN(face == NULL, CG_FALSE, CGSTR("Cannot get glyphs from NULL face."));
    CG_ERROR_COND_RETURN(glyphs == NULL || glyphs->glyph_instances == NULL, CG_FALSE, CGSTR("Cannot get glyphs from NULL glyphs array."));
    CG_ERROR_COND_RETURN(text_property == NULL, CG_FALSE, CGSTR("Cannot get glyphs from NULL text property."));
    unsigned int glyph_index;
    int max_char_top = 0;
    int min_char_bottom = 0x7fffffff;
    for (unsigned int i = 0; i < glyphs->glyphs_count; ++i)
    {
        if (text[i] == (CGChar)' ')
        {
            glyphs->glyph_instances[i].glyph_instance = NULL;
            glyphs->glyphs_dimension.horizontal_layout.total_width += text_property->space_width + text_property->kerning;
            continue;
        }
        CG_ERROR_COND_RETURN(!CGGetGlyphFromFace(face, text[i]), CG_FALSE, CGSTR("Failed to get text image."));
        CG_ERROR_COND_RETURN(FT_Get_Glyph(face->glyph, &(glyphs->glyph_instances[i].glyph_instance)), CG_FALSE, CGSTR("Failed to get glyph instance from face."));
        glyphs->glyph_instances[i].delta_y = face->glyph->bitmap_top;
        if (face->glyph->bitmap_top > max_char_top)
            max_char_top = face->glyph->bitmap_top;
        glyphs->glyphs_dimension.horizontal_layout.total_width += face->glyph->bitmap.width + text_property->kerning;
        if (face->glyph->bitmap_top - (int)face->glyph->bitmap.rows < min_char_bottom)
            min_char_bottom = face->glyph->bitmap_top - face->glyph->bitmap.rows;
    }
    for (unsigned int i = 0; i < glyphs->glyphs_count; ++i)
		glyphs->glyph_instances[i].delta_y = max_char_top - glyphs->glyph_instances[i].delta_y;
    glyphs->glyphs_dimension.horizontal_layout.max_char_height = (unsigned int)(max_char_top - min_char_bottom);
    return CG_TRUE;
}

static void CGWriteSpace(unsigned current_x, unsigned int bitmap_width,  unsigned int bitmap_height, unsigned int space_width, CGUByte* bitmap)
{
    for (unsigned int i = 0; i < bitmap_height; ++i)
    {
        for (unsigned int j = 0; j < space_width; ++j)
        {
            for (unsigned int k = 0; k < 4; ++k)
                bitmap[(i * bitmap_width + current_x + j) * 4 + k] = 0;
        }
    }
}

static void CGWriteChar(unsigned int current_x, 
    const CGGlyphs* glyphs, unsigned int glyph_index, CGUByte* target_bitmap)
{
    unsigned int write_pos;
    const unsigned int bitmap_width = glyphs->glyphs_dimension.horizontal_layout.total_width;
    const unsigned int bitmap_height = glyphs->glyphs_dimension.horizontal_layout.max_char_height;
    FT_BitmapGlyph glyph = (FT_BitmapGlyph)glyphs->glyph_instances[glyph_index].glyph_instance;
    const unsigned int delta_y = glyphs->glyph_instances[glyph_index].delta_y;
    for (unsigned int i = 0; i < bitmap_height; ++i)
    {
        for (unsigned int j = 0; j < glyph->bitmap.width; ++j)
        {
            write_pos = (i * bitmap_width + current_x + j) * 4;
            if (i < delta_y || i >= delta_y + glyph->bitmap.rows)
            {
                for (unsigned int k = 0; k < 4; ++k)
                    target_bitmap[write_pos + k] = 0x00;
                continue;
            }
            for (unsigned int k = 0; k < 3; ++k)
                target_bitmap[write_pos + k] = 0xff;
            target_bitmap[write_pos + 3] = glyph->bitmap.buffer[(i - delta_y) * glyph->bitmap.width + j];
        }
    }
}

static CG_BOOL CGGetGlyphsBitmap(const CGGlyphs* glyphs, CGUByte* bitmap, const CGTextProperty* text_property)
{
    CG_ERROR_COND_RETURN(glyphs == NULL, CG_FALSE, CGSTR("Cannot get glyphs bitmap from NULL glyphs"));
    CG_ERROR_COND_RETURN(bitmap == NULL, CG_FALSE, CGSTR("Cannot get glyphs bitmap from NULL bitmap."));
    unsigned int current_x = 0;
    for (unsigned int i = 0; i < glyphs->glyphs_count; ++i)
    {
        if (glyphs->glyph_instances[i].glyph_instance == NULL)
        {
            CGWriteSpace(current_x, 
                glyphs->glyphs_dimension.horizontal_layout.total_width, glyphs->glyphs_dimension.horizontal_layout.max_char_height,
                text_property->space_width, bitmap);
            current_x += text_property->space_width;
        }
        else
        {
            CGWriteChar(current_x, glyphs, i, bitmap);
            current_x += ((FT_BitmapGlyph)(glyphs->glyph_instances[i].glyph_instance))->bitmap.width;
        }

        CGWriteSpace(current_x,
            glyphs->glyphs_dimension.horizontal_layout.total_width, glyphs->glyphs_dimension.horizontal_layout.max_char_height,
            text_property->kerning, bitmap);
        current_x += text_property->kerning;
    }
    return CG_TRUE;
}

static CG_BOOL CGGetTextTexture(const CGChar* text, FT_Face face, const CGTextProperty* text_property, 
    unsigned int* texture_width, unsigned int* texture_height, unsigned int* result)
{
    CG_ERROR_COND_RETURN(text == NULL, CG_FALSE, CGSTR("Cannot get texture id from NULL text."));
    CG_ERROR_COND_RETURN(result == NULL, CG_FALSE, CGSTR("Cannot get texture id from NULL result."));
    CG_ERROR_COND_RETURN(text_property == NULL, GL_FALSE, CGSTR("Cannot get texture id from NULL text property."));

    CGGladInitializeCheck();

    CGGlyphs glyphs;
    glyphs.glyphs_dimension.horizontal_layout.max_char_height = 0;
    glyphs.glyphs_dimension.horizontal_layout.total_width = 0;
    glyphs.glyphs_count = CG_STRLEN(text);
    glyphs.glyph_instances = (CGGlyphInstance*)malloc(glyphs.glyphs_count * sizeof(CGGlyphInstance));
    CG_ERROR_COND_RETURN(glyphs.glyph_instances == NULL, CG_FALSE, CGSTR("Failed to allocate memory for glyphs."));
    if (FT_Set_Pixel_Sizes(face, text_property->text_width, text_property->text_height))
    {
        free(glyphs.glyph_instances);
        CG_ERROR_COND_RETURN(CG_TRUE, CG_FALSE, CGSTR("Failed to set font pixel size."));
    }
    if (!CGGetTextGlyphs(face, text, &glyphs, text_property))
    {
        free(glyphs.glyph_instances);
        CG_ERROR_COND_RETURN(CG_TRUE, CG_FALSE, CGSTR("Failed to get glyphs from text."));
    }

    CGUByte* image_data = (CGUByte*)malloc(
        glyphs.glyphs_dimension.horizontal_layout.total_width * glyphs.glyphs_dimension.horizontal_layout.max_char_height * 4);
    if (image_data == NULL)
    {
        for (unsigned int i = 0; i < glyphs.glyphs_count; ++i)
        {
            if (glyphs.glyph_instances[i].glyph_instance != NULL)
                FT_Done_Glyph(glyphs.glyph_instances[i].glyph_instance);
        }
        free(glyphs.glyph_instances);
        CG_ERROR_COND_RETURN(CG_TRUE, CG_FALSE, CGSTR("Failed to allocate memory for text image."));
    }
    if (!CGGetGlyphsBitmap(&glyphs, image_data, text_property))
    {
        free(image_data);
        for (unsigned int i = 0; i < glyphs.glyphs_count; ++i)
        {
            if (glyphs.glyph_instances[i].glyph_instance != NULL)
                FT_Done_Glyph(glyphs.glyph_instances[i].glyph_instance);
        }
        free(glyphs.glyph_instances);
        CG_ERROR_COND_RETURN(CG_TRUE, CG_FALSE, CGSTR("Failed to get bitmap from glyphs."));
    }
    for (unsigned int i = 0; i < glyphs.glyphs_count; ++i)
    {
        if (glyphs.glyph_instances[i].glyph_instance != NULL)
            FT_Done_Glyph(glyphs.glyph_instances[i].glyph_instance);
    }
    free(glyphs.glyph_instances);
    
    glGenTextures(1, result);
    glBindTexture(GL_TEXTURE_2D, *result);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 
		glyphs.glyphs_dimension.horizontal_layout.total_width, glyphs.glyphs_dimension.horizontal_layout.max_char_height, 
		0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    free(image_data);

    if (texture_width != NULL)
        *texture_width = glyphs.glyphs_dimension.horizontal_layout.total_width;
    if (texture_height != NULL)
        *texture_height = glyphs.glyphs_dimension.horizontal_layout.max_char_height;
    return CG_TRUE;
}

static CG_BOOL CGCreateFreetypeFace(const CGChar* font_rk, FT_Face* face)
{
    unsigned int resource_size;
    CGByte* resource = CGLoadReusableResource(font_rk, CG_DELETER(free), &resource_size);
    #ifdef CG_USE_WCHAR
    CG_ERROR_COND_RETURN(resource == NULL, CG_FALSE, CGSTR("Failed to load font resource with rk: %ls"), font_rk);
    CG_ERROR_COND_RETURN(FT_New_Memory_Face(cg_ft_library, resource, resource_size, 0, face), CG_FALSE, 
        CGSTR("Failed to create font face with rk: %ls"), font_rk);
    #else
    CG_ERROR_COND_RETURN(resource == NULL, CG_FALSE, CGSTR("Failed to load font resource with rk: %s"), font_rk);
    CG_ERROR_COND_RETURN(FT_New_Memory_Face(cg_ft_library, resource, resource_size, 0, face), CG_FALSE, 
        CGSTR("Failed to create font face with rk: %s"), font_rk);
    #endif
    return CG_TRUE;
}

CGTextProperty CGConstructTextProperty(unsigned int text_width, unsigned int text_height, unsigned int space_width, unsigned int kerning)
{
    return (CGTextProperty){.text_width = text_width, .text_height = text_height, .space_width = space_width, .kerning = kerning};
}

CGVisualImage* CGCreateTextVisualImage(const CGChar* text_rk, const CGChar* font_rk, CGTextProperty text_property, CGWindow* window)
{
    CG_ERROR_COND_RETURN(text_rk == NULL, NULL, CGSTR("Cannot create text visual_image with NULL text resource key."));
    CG_ERROR_COND_RETURN(window == NULL || window->glfw_window_instance == NULL, NULL, CGSTR("Cannot create text visual_image with NULL window."));
    CGChar* text = (CGChar*)CGLoadResource(text_rk, NULL, NULL);
    CG_ERROR_COND_RETURN(text == NULL, NULL, CGSTR("Failed to load text resource."));

    // create face.
    FT_Face face;
    if (font_rk == NULL)
        face = cg_ft_default_face;
    else if (!CGCreateFreetypeFace(font_rk, &face))
    {
        free(text);
        #ifdef CG_USE_WCHAR
        CG_ERROR_COND_RETURN(CG_TRUE, NULL, CGSTR("Failed to create freetype face with rk: \"%ls\"."), font_rk);
        #else
        CG_ERROR_COND_RETURN(CG_TRUE, NULL, CGSTR("Failed to create freetype face with rk: \"%s\"."), font_rk);
        #endif
    }

    // get texture
    unsigned int texture_width, texture_height, texture_id;
    if (!CGGetTextTexture(text, face, &text_property, &texture_width, &texture_height, &texture_id))
    {
        if (font_rk != NULL)
            FT_Done_Face(face);
        #ifdef CG_USE_WCHAR
        CG_ERROR(CGSTR("Failed to get bitmap from text: %ls"), text);
        #else
        CG_ERROR(CGSTR("Failed to get bitmap from text: %s"), text);
        #endif
        free(text);
        return NULL;
    }
    free(text);
    if (font_rk != NULL)
        FT_Done_Face(face);

    CGVisualImage* result = (CGVisualImage*)malloc(sizeof(CGVisualImage));
    CG_ERROR_COND_RETURN(result == NULL, NULL, CGSTR("Failed to allocate memory for visual_image."));
    result->in_window = window;
    result->is_temp = CG_FALSE;
    result->img_width = texture_width;
    result->img_height = texture_height;
    result->img_channels = 1;
    result->clamp_top_left = (CGVector2){0.0f, 0.0f};
    result->clamp_bottom_right = (CGVector2){ (float)texture_width, (float)texture_height };
    result->is_clamped = CG_FALSE;
    result->texture_id = texture_id;

    CGRegisterResource(result, CG_DELETER(CGDeleteVisualImage));
    return result;
}

static void CGDeleteVisualImage(CGVisualImage* visual_image)
{
    if (visual_image == NULL)
        return;
    CGFreeTextureResource(visual_image->texture_id);
    free(visual_image);
}

static void CGRenderVisualImage(CGVisualImage* visual_image, const CGRenderObjectProperty* property, CGWindow* window, float assigned_z)
{
    CG_ERROR_CONDITION(visual_image == NULL, CGSTR("Failed to draw visual_image: VisualImage must be specified to a non-null visual_image instance."));
    CG_ERROR_CONDITION(window == NULL || window->glfw_window_instance == NULL, CGSTR("Failed to draw visual_image: Attempting to draw visual_image on a NULL window"));
    CGGladInitializeCheck();
    float* vertices = CGMakeVisualImageVertices(visual_image, assigned_z);
    glBindVertexArray(window->visual_image_vao);
    glUseProgram(cg_visual_image_shader_program);
    glBindBuffer(GL_ARRAY_BUFFER, cg_gl_buffers[CG_GL_BUFFERS_VISUAL_IMAGE_VBO]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 20 * sizeof(float), vertices);

    glBindTexture(GL_TEXTURE_2D, visual_image->texture_id);
    CGSetPropertyUniforms(cg_visual_image_shader_program, property);
    CGSetShaderUniform1f(cg_visual_image_shader_program, "render_width", (float)window->width / 2.0f);
    CGSetShaderUniform1f(cg_visual_image_shader_program, "render_height", (float)window->height / 2.0f);
    CGSetShaderUniform1i(cg_visual_image_shader_program, "is_clamped", visual_image->is_clamped);
    CGSetShaderUniformVec2f(cg_visual_image_shader_program, "clamp_top_left", visual_image->clamp_top_left);
    CGSetShaderUniformVec2f(cg_visual_image_shader_program, "clamp_bottom_right", visual_image->clamp_bottom_right);
    CGSetShaderUniformVec2f(cg_visual_image_shader_program, "image_dimension", (CGVector2){visual_image->img_width, visual_image->img_height});
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

static void CGDrawGlyph( int offset, const FT_GlyphSlot glyph, const CGRenderObjectProperty* render_property, const CGWindow* window)
{
    CG_ERROR_CONDITION(glyph == NULL, CGSTR("Failed to draw bitmap: Bitmap must be specified to a non-null bitmap instance."));

    unsigned int texture_id = 0;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, glyph->bitmap.width, glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, glyph->bitmap.buffer);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glGenerateMipmap(GL_TEXTURE_2D);

    glUseProgram(cg_bitmap_visual_image_shader_program);
    glBindVertexArray(window->visual_image_vao);
    glBindBuffer(GL_ARRAY_BUFFER, cg_gl_buffers[CG_GL_BUFFERS_VISUAL_IMAGE_VBO]);
    double bottom = ((double)glyph->bitmap_top - (double)glyph->bitmap.rows);
    double right = (double)(glyph->bitmap_left + glyph->bitmap.width);
    CGSetFloatArrayValue(20, cg_global_buffer_f20,
        (double)(glyph->bitmap_left + offset), (double)glyph->bitmap_top, 0.0, 0.0, 0.0,
        (double)(right + offset), (double)glyph->bitmap_top, 0.0, 1.0, 0.0,
        (double)(right + offset), (double)bottom, 0.0, 1.0, 1.0,
        (double)(glyph->bitmap_left + offset), (double)bottom, 0.0, 0.0, 1.0
    );
    glBufferSubData(GL_ARRAY_BUFFER, 0, 20 * sizeof(float), cg_global_buffer_f20);
    CGSetPropertyUniforms(cg_bitmap_visual_image_shader_program, render_property);
    CGSetShaderUniform1f(cg_bitmap_visual_image_shader_program, "render_width", (float)window->width / 2.0f);
    CGSetShaderUniform1f(cg_bitmap_visual_image_shader_program, "render_height", (float)window->height / 2.0f);
    CGSetShaderUniformVec2f(cg_bitmap_visual_image_shader_program, "image_dimension", (CGVector2){glyph->bitmap.width, glyph->bitmap.rows});

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &texture_id);
}

CG_BOOL CGDrawText(const CGChar* text_rk, const CGChar* font_rk, CGTextProperty text_property, const CGRenderObjectProperty* render_property, const CGWindow* window)
{
    CG_ERROR_COND_RETURN(text_rk == NULL, CG_FALSE, CGSTR("Cannot draw text with NULL text resource key."));
    if (render_property == NULL)
        render_property = cg_default_geo_property;
    CGGladInitializeCheck();
    
    CGChar* text = (CGChar*)CGLoadResource(text_rk, NULL, NULL);
    CG_ERROR_COND_RETURN(text == NULL, CG_FALSE, CGSTR("Failed to load text resource."));
    FT_Face face;
    if (font_rk == NULL)
        face = cg_ft_default_face;
    else if (!CGCreateFreetypeFace(font_rk, &face))
    {
        free(text);
        #ifdef CG_USE_WCHAR
        CG_ERROR_COND_RETURN(CG_TRUE, CG_FALSE, CGSTR("Failed to create freetype face with rk: \"%ls\"."), font_rk);
        #else
        CG_ERROR_COND_RETURN(CG_TRUE, CG_FALSE, CGSTR("Failed to create freetype face with rk: \"%s\"."), font_rk);
        #endif
    }
    if (FT_Set_Pixel_Sizes(face, text_property.text_width, text_property.text_height))
    {
        free(text);
        CG_ERROR_COND_RETURN(CG_TRUE, CG_FALSE, CGSTR("Failed to set pixel size for font."), font_rk);
    }

    unsigned int char_count = CG_STRLEN(text);
    int offset = 0;
    for (unsigned int i = 0; i < char_count; ++i)
    {
        if (text[i] == ' ')
        {
            offset += text_property.space_width + text_property.kerning;
            continue;
        }
        if (!CGGetGlyphFromFace(face, text[i]))
        {
            if (font_rk != NULL)
                FT_Done_Face(face);
            #ifdef CG_USE_WCHAR
            CG_ERROR(CGSTR("Failed to load glyph with char: \'%lc\' (unicode: %#x)."), text[i], text[i]);
            #else
            CG_ERROR(CGSTR("Failed to load glyph with char: \'%c\' (unicode: %#x)."), text[i], text[i]);
            #endif
            free(text);
            return CG_FALSE;
        }
        CGDrawGlyph(offset, face->glyph, render_property, window);
        offset += face->glyph->bitmap.width + text_property.kerning;
    }

    if (font_rk != NULL)
        FT_Done_Face(face);
    free(text);
    return CG_TRUE;
}