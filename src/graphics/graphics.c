#include "cos_graphics/graphics.h"
#include "cos_graphics/log.h"
#include "cos_graphics/resource.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define CG_MAX_BUFFER_SIZE 256

#define CGGladInitializeCheck()                                 \
    if (!cg_is_glad_initialized) {                              \
        CG_WARNING("In function: %s, GLAD not initialized yet. Initializing GLAD...", __func__);    \
        CGInitGLAD();                                           \
    }((void)0)

CG_BOOL cg_is_glfw_initialized = CG_FALSE;
CG_BOOL cg_is_glad_initialized = CG_FALSE;

CGRenderObjectProperty* cg_default_geo_property;
CGRenderObjectProperty* cg_default_visual_image_property;

#define CG_BUFFERS_TRIANGLE_VBO 0
#define CG_BUFFERS_QUADRANGLE_VBO 1
#define CG_BUFFERS_QUADRANGLE_EBO 2
#define CG_BUFFERS_VISUAL_IMAGE_VBO 3
#define CG_BUFFERS_VISUAL_IMAGE_EBO 4

unsigned int cg_buffers[CG_MAX_BUFFER_SIZE] = {0};
unsigned int cg_buffer_count;

/**
 * @brief vertex shader path for a geometry
 */
const char* cg_default_geo_vshader_path = "./shaders/default_geo_shader.vert";
/**
 * @brief fragment shader path for a geometry
 */
const char* cg_default_geo_fshader_path = "./shaders/default_geo_shader.frag";
/**
 * @brief vertex shader path for a geometry
 */
const char* cg_default_visual_image_vshader_path = "./shaders/default_visual_image_shader.vert";
/**
 * @brief fragment shader path for a geometry
 */
const char* cg_default_visual_image_fshader_path = "./shaders/default_visual_image_shader.frag";

/**
 * @brief default shader for geometry
 */
CGShaderProgram cg_default_geo_shader_program;

/**
 * @brief shader program for drawing geometry
 */
CGShaderProgram cg_geo_shader_program;

/**
 * @brief default shader for visual_images
 */
CGShaderProgram cg_default_visual_image_shader_program;

/**
 * @brief shader program for drawing visual_images
 */
CGShaderProgram cg_visual_image_shader_program;

#define CG_RETURN_RENDER_PROPERTY(object, identifier, property_name)      \
    switch (identifier)                               \
    {                                                       \
    case CG_RD_TYPE_TRIANGLE:                               \
        return &((CGTriangle*)object)->property_name;   \
    case CG_RD_TYPE_QUADRANGLE:                             \
        return &((CGQuadrangle*)object)->property_name; \
    case CG_RD_TYPE_VISUAL_IMAGE:                                 \
        return &((CGVisualImage*)object)->property_name;     \
    default:                                                \
        CG_ERROR_COND_RETURN(CG_TRUE, 0, "Failed to get the \"%s\" property from node: Cannot find render identifier: %d.", #property_name, identifier);    \
    }((void)0)

#define CG_EXTRACT_RENDER_NODE_DATA(node) ((CGRenderNodeData*)node->data)
typedef struct
{
    void* object;
    CGRenderObjectProperty* property;
}CGRenderNodeData;

// get the "z" property of the render node object.
float* CGGetDepthPointer(void* object, int identifier);

// get assigned z property of the render node object
float* CGGetAssignedZPointer(void* object, int identifier);

const float cg_normal_matrix[16] = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};

// compile one specific shader from source
CG_BOOL CGCompileShader(unsigned int shader_id, const char* shader_source);

// initialize default shader
void CGInitDefaultShader(const char* shader_vpath, const char* shader_fpath, CGShaderProgram* shader_program);

// make a vertices array out of triangle
float* CGMakeTriangleVertices(const CGTriangle* triangle, float assigned_z);

// make a vertices array out of quadrangle
float* CGMakeQuadrangleVertices(const CGQuadrangle* quadrangle, float assigned_z);

// make a vertices array out of visual_image
float* CGMakeVisualImageVertices(const CGVisualImage* visual_image, float assigned_z);

// set buffer value
void CGBindBuffer(GLenum buffer_type, unsigned int buffer, unsigned int buffer_size, void* buffer_data, unsigned int usage);

// create transform matrix
float* CGCreateTransformMatrix(CGVector2 transform);

// create scale matrix
float* CGCreateScaleMatrix(CGVector2 scale);

// create rotation matrix
float* CGCreateRotateMatrix(float rotate);

// set geometry matrices uniform
void CGSetPropertyUniforms(CGShaderProgram shader_program, const CGRenderObjectProperty* property);

// render triangle
void CGRenderTriangle(const CGTriangle* triangle, const CGRenderObjectProperty* property, const CGWindow* window, float assigned_z);

// render quadrangle
void CGRenderQuadrangle(const CGQuadrangle* quadrangle, const CGRenderObjectProperty* property, const CGWindow* window, float assigned_z);

// render visual_image
void CGRenderVisualImage(CGVisualImage* visual_image, const CGRenderObjectProperty* property, CGWindow* window, float assigned_z);

// bind texture to vao
void CGSetTextureValue(unsigned int texture_id, unsigned int vao, CGImage* texture);

/**
 * @brief Multiply two matrices together (A x B)
 * 
 * @param result The result matrix (needs to allocate memory manually)
 * @param mat_1 Matrix A
 * @param mat_2 Matrix B
 * @param demention_x The x count of the result matrix
 * @param demention_y The y count of the result matrix
 * @return float* 
 */
void CGMatMultiply(float* result, const float* mat_1, const float* mat_2, int demention_x, int demention_y);


/**
 * @brief Create a render list based on the window.
 * 
 * @param window the window to create the render list on.
 */
void CGCreateRenderList(CGWindow* window);

/**
 * @brief Add a render list node in the list.
 * 
 * @param list_head The head of the list. If this parameter is NULL, the program will not do anything.
 * @param node The node to be added into the list
 */
void CGAddRenderListNode(CGRenderNode* list_head, CGRenderNode* node);

/**
 * @brief Reorganize the render list, and prepare for the render.
 * 
 * @param window window that holds the render list.
 */
void CGReorganizeRenderList(CGWindow* window);

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
    CGVector2 vector;
    vector.x = x;
    vector.y = y;
    return vector;
}

void CGFrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
    if (window != glfwGetCurrentContext())
        glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);
}

void CGInitGLFW(CG_BOOL window_resizable)
{
    CG_ERROR_EXP_EXIT(glfwInit() != GLFW_TRUE, -1, "GLFW initialization failed");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, window_resizable);
    glfwSwapInterval(0);
    cg_is_glfw_initialized = CG_TRUE;
}

void CGTerminateGraphics()
{
    if (cg_is_glad_initialized)
    {
        glDeleteBuffers(cg_buffer_count, cg_buffers);
        cg_buffer_count = 0;
        glDeleteProgram(cg_default_geo_shader_program);
        glDeleteProgram(cg_default_visual_image_shader_program);
        free(cg_default_geo_property);
        cg_default_geo_property = NULL;
        free(cg_default_visual_image_property);
        cg_default_visual_image_property = NULL;
        cg_is_glad_initialized = CG_FALSE;
    }
    if (cg_is_glfw_initialized)
    {
        glfwTerminate();
        cg_is_glfw_initialized = CG_FALSE;
    }
}

void CGInitDefaultShader(const char* shader_vpath, const char* shader_fpath, CGShaderProgram* shader_program)
{
    CG_ERROR_COND_EXIT(shader_vpath == NULL || shader_fpath == NULL, -1, "Default shader path cannot be set to NULL.");
    CG_ERROR_COND_EXIT(shader_program == NULL, -1, "Cannot init a default shader for NULL shader program.");
    CGShaderSource* shader_source = CGCreateShaderSourceFromPath(shader_vpath, shader_fpath, NULL, CG_FALSE);
    CG_ERROR_COND_EXIT(shader_source == NULL, -1, "Failed to create shader source.");
    CGShader* shader = CGCreateShader(shader_source);
    CG_ERROR_COND_EXIT(shader == NULL, -1, "Failed to init default shader.");
    *shader_program = CGCreateShaderProgram(shader);
    CGDeleteShaderSource(shader_source);
    CGDeleteShader(shader);
}

void CGInitGLAD()
{
    CG_ERROR_EXP_EXIT(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), -1, "GLAD setup OpenGL loader failed");

    CGInitDefaultShader(cg_default_geo_vshader_path, cg_default_geo_fshader_path, &cg_default_geo_shader_program);
    cg_geo_shader_program = cg_default_geo_shader_program;

    cg_default_geo_property = CGCreateRenderObjectProperty(
        CGConstructColor(0.8f, 0.8f, 0.8f, 1.0f), 
        CGConstructVector2(0.0f, 0.0f),
        CGConstructVector2(1.0f, 1.0f),
        0.0f);

    CGInitDefaultShader(cg_default_visual_image_vshader_path, cg_default_visual_image_fshader_path, &cg_default_visual_image_shader_program);
    cg_visual_image_shader_program = cg_default_visual_image_shader_program;
    
    cg_default_visual_image_property = CGCreateRenderObjectProperty(
        CGConstructColor(1.0f, 1.0f, 1.0f, 1.0f), 
        CGConstructVector2(0.0f, 0.0f),
        CGConstructVector2(1.0f, 1.0f),
        0.0f);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenBuffers(5, cg_buffers);
    cg_buffer_count = 5;
    cg_is_glad_initialized = CG_TRUE;
}

CGWindow* CGCreateWindow(int width, int height, const char* title, CG_BOOL use_full_screen, CG_BOOL resizable)
{
    if (!cg_is_glfw_initialized)
        CGInitGLFW(resizable);

    CGWindow* window = (CGWindow*)malloc(sizeof(CGWindow));
    CG_ERROR_COND_RETURN(window == NULL, NULL, "Failed to allocate memory for window.");
    window->width = width;
    window->height = height;
    strcpy(window->title, title);
    window->use_full_screen = use_full_screen;
    window->glfw_window_instance = glfwCreateWindow(width, height, title, 
        use_full_screen ? glfwGetPrimaryMonitor() : NULL, NULL);
    
    window->render_list = NULL;
    CGCreateRenderList(window);
    if (window->glfw_window_instance == NULL)
    {
        CG_ERROR("Failed to create GLFW window.");
        glfwTerminate();
        free(window);
        return NULL;
    }
    CGCreateViewport(window);

    return window;
}

void CGDestroyWindow(CGWindow* window)
{
    glDeleteVertexArrays(1, &window->triangle_vao);
    glDeleteVertexArrays(1, &window->quadrangle_vao);
    glDeleteVertexArrays(1, &window->visual_image_vao);
    glfwDestroyWindow((GLFWwindow*)window->glfw_window_instance);
    CGDeleteList(window->render_list);
    
    free(window);
}

void CGCreateViewport(CGWindow* window)
{
    CG_ERROR_CONDITION(window == NULL || window->glfw_window_instance == NULL, "Attempting to create a viewport on a NULL window.");
    if (glfwGetCurrentContext() != window->glfw_window_instance)
        glfwMakeContextCurrent((GLFWwindow*)(window->glfw_window_instance));
    if (!cg_is_glad_initialized)
        CGInitGLAD();

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.2, 0.2, 0.2, 1.0);
    glViewport(0, 0, window->width, window->height);
    float temp_vertices[20] = {0};

    unsigned int cg_quadrangle_indices[6] = {
        0, 1, 2,
        0, 2, 3
    };

    // set triangle vao properties
    glGenVertexArrays(1, &window->triangle_vao);
    glBindVertexArray(window->triangle_vao);
    CGBindBuffer(GL_ARRAY_BUFFER, cg_buffers[CG_BUFFERS_TRIANGLE_VBO], 9 * sizeof(float), temp_vertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // set quadrangle vao properties
    glGenVertexArrays(1, &window->quadrangle_vao);
    glBindVertexArray(window->quadrangle_vao);
    CGBindBuffer(GL_ARRAY_BUFFER, cg_buffers[CG_BUFFERS_QUADRANGLE_VBO], 12 * sizeof(float), temp_vertices, GL_DYNAMIC_DRAW);
    CGBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cg_buffers[CG_BUFFERS_QUADRANGLE_EBO], 6 * sizeof(unsigned int), cg_quadrangle_indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // set visual_image vao properties
    glGenVertexArrays(1, &window->visual_image_vao);
    glBindVertexArray(window->visual_image_vao);
    CGBindBuffer(GL_ARRAY_BUFFER, cg_buffers[CG_BUFFERS_VISUAL_IMAGE_VBO], 20 * sizeof(float), temp_vertices, GL_DYNAMIC_DRAW);
    CGBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cg_buffers[CG_BUFFERS_VISUAL_IMAGE_EBO], 6 * sizeof(unsigned int), cg_quadrangle_indices, GL_STATIC_DRAW);
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
    CGReorganizeRenderList(window);
    CGRenderNode* draw_obj = window->render_list->next;
    CGRenderNodeData *data;
    CG_ERROR_CONDITION(data == NULL, "Failed to allocate memory for drawing window.");
    while (draw_obj != NULL)
    {
        data = (CGRenderNodeData*)(draw_obj->data);
        switch (draw_obj->identifier)
        {
        case CG_RD_TYPE_TRIANGLE:
            CGRenderTriangle(data->object, data->property, window, *CGGetAssignedZPointer(draw_obj, draw_obj->identifier));
            break;
        case CG_RD_TYPE_QUADRANGLE:
            CGRenderQuadrangle(data->object, data->property, window, *CGGetAssignedZPointer(draw_obj, draw_obj->identifier));
            break;
        case CG_RD_TYPE_VISUAL_IMAGE:
            CGRenderVisualImage(data->object, data->property, window, *CGGetAssignedZPointer(data->object, draw_obj->identifier));
            break;
        default:
            CG_ERROR_COND_EXIT(CG_TRUE, -1, "Cannot find render object identifier: %d", draw_obj->identifier);
        }
        free(draw_obj->data);
        CGRemoveLinkedListNode(&draw_obj);
    }
    window->render_list->next = NULL;
}

void CGTickRenderEnd()
{
    //check OpenGL error
    int gl_error_code = glGetError();
    CG_ERROR_COND_EXIT(gl_error_code != GL_NO_ERROR, -1, "OpenGL Error: Error code: 0x%x.", gl_error_code);
}

CG_BOOL CGShouldWindowClose(CGWindow* window)
{
    return (CG_BOOL)glfwWindowShouldClose(window->glfw_window_instance);
}

double CGGetCurrentTime()
{
    return glfwGetTime();
}

void CGMakeStr(char* to, const char* from, const char* error_message)
{
    if (from != NULL)
    {
        to = (char*)malloc(strlen(from) * sizeof(char));
        if (to == NULL)
        {
            CG_ERROR(error_message);
            to = NULL;
        }
        strcpy(to, from);
    }
    else
        to = NULL;
}

CGShaderSource* CGCreateShaderSource(const char* vertex, const char* fragment, 
    const char* geometry, CG_BOOL use_geometry)
{
    CGShaderSource* result = (CGShaderSource*)malloc(sizeof(CGShaderSource));
    if (result == NULL)
    {
        CG_ERROR("Construct shader source failed");
        return NULL;
    }
    CGMakeStr(result->vertex, vertex, "Construct shader source failed.");
    CGMakeStr(result->fragment, fragment, "Construct shader source failed.");
    if (use_geometry)
        CGMakeStr(result->geometry, geometry, "Construct shader source failed.");
    result->use_geometry = use_geometry;

    return result;
}

CGShaderSource* CGCreateShaderSourceFromPath(const char* vertex_path, const char* fragment_path, 
    const char* geometry_path, CG_BOOL use_geometry)
{
    CGShaderSource* result = (CGShaderSource*)malloc(sizeof(CGShaderSource));
    if (result == NULL)
    {
        CG_ERROR("Construct shader source failed.");
        return NULL;
    }
    result->vertex = CGLoadFile(vertex_path);
    if (result->vertex == NULL)
    {
        CG_ERROR("Load vertex shader source failed.");
        free(result);
        return NULL;
    }
    result->fragment = CGLoadFile(fragment_path);
    if (result->fragment == NULL)
    {
        CG_ERROR("Load fragment shader source failed.");
        free(result->vertex);
        free(result);
        return NULL;
    }
    if (use_geometry)
    {
        result->geometry = CGLoadFile(geometry_path);
        if (result->geometry == NULL)
        {
            CG_ERROR("Load geometry shader source failed.");
            free(result->vertex);
            free(result->fragment);
            free(result);
            return NULL;
        }
    }
    else
        result->geometry = NULL;
    result->use_geometry = use_geometry;
    return result;
}

void CGDeleteShaderSource(CGShaderSource* shader_source)
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

CG_BOOL CGCompileShader(unsigned int shader_id, const char* shader_source)
{
    CG_ERROR_COND_RETURN(shader_source == NULL, CG_FALSE, "Attempting to compile a shader with a NULL source.");
    glShaderSource(shader_id, 1, &shader_source, NULL);
    glCompileShader(shader_id);

    int success;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);

    char info_log[CG_INFO_LOG_SIZE];

    if (!success)
    {
        glGetShaderInfoLog(shader_id, CG_INFO_LOG_SIZE, NULL, info_log);
        CG_ERROR("Failed to compile shader with id: %d. \nOutput log: %s.", shader_id, info_log);
        return CG_FALSE;
    }
    return CG_TRUE;
}

CGShader* CGCreateShader(CGShaderSource* shader_source)
{
    CG_ERROR_COND_RETURN(shader_source == NULL, NULL, "Attempting to compile a NULL shader source.");
    CGShader* shader = (CGShader*)malloc(sizeof(CGShader));
    CG_ERROR_COND_RETURN(shader == NULL, NULL, "Construct shader failed.");
    shader->vertex = glCreateShader(GL_VERTEX_SHADER);
    if (!CGCompileShader(shader->vertex, shader_source->vertex))
    {
        CG_ERROR("Failed to compile vertex shader.");
        return NULL;
    }
    shader->fragment = glCreateShader(GL_FRAGMENT_SHADER);
    if (!CGCompileShader(shader->fragment, shader_source->fragment))
    {
        CG_ERROR("Failed to compile fragment shader.");
        return NULL;
    }
    shader->use_geometry = shader_source->use_geometry;
    if (!shader_source->use_geometry)
    {
        shader_source->geometry = NULL;
        return shader;
    }
    shader->geometry = glCreateShader(GL_GEOMETRY_SHADER);
    if (!CGCompileShader(shader->geometry, shader_source->geometry))
    {
        CG_ERROR("Failed to compile geometry shader.");
        return NULL;
    }
    return shader;
}

void CGDeleteShader(CGShader* shader)
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

void CGDeleteShaderProgram(CGShaderProgram program)
{
    glDeleteProgram(program);
}

void CGSetShaderUniform1f(CGShaderProgram shader_program, const char* uniform_name, float value)
{
    CG_ERROR_CONDITION(uniform_name == NULL, "Attempting to set a uniform with a NULL name.");
    CGGladInitializeCheck();
    GLint uniform_location = glGetUniformLocation(shader_program, uniform_name);
    glUniform1f(uniform_location, value);
}

void CGSetShaderUniformVec4f(
    CGShaderProgram shader_program, const char* uniform_name,
    float val_1, float val_2, float val_3, float val_4)
{
    CGGladInitializeCheck();
    CG_ERROR_CONDITION(uniform_name == NULL, "Attempting to set a uniform with a NULL name.");
    GLint uniform_location = glGetUniformLocation(shader_program, uniform_name);
    glUniform4f(uniform_location, val_1, val_2, val_3, val_4);
}

void CGSetShaderUniformMat4f(CGShaderProgram shader_program, const char* uniform_name, const float* data)
{
    CGGladInitializeCheck();
    CG_ERROR_CONDITION(uniform_name == NULL, "Attempting to set a uniform with a NULL name.");
    GLint uniform_location = glGetUniformLocation(shader_program, uniform_name);
    glUniformMatrix4fv(uniform_location, 1, GL_FALSE, data);
}

void CGDraw(void* draw_object, CGRenderObjectProperty* draw_property, CGWindow* window, int object_type)
{
    CGRenderNodeData* data = (CGRenderNodeData*)malloc(sizeof(CGRenderNodeData));
    data->object = draw_object;
    data->property = draw_property;
    CGAddRenderListNode(window->render_list, CGCreateLinkedListNode(data, object_type));
}

float* CGGetDepthPointer(void* object, int identifier)
{
    CG_RETURN_RENDER_PROPERTY(object, identifier, z);
}

float* CGGetAssignedZPointer(void* object, int identifier)
{
    CG_RETURN_RENDER_PROPERTY(object, identifier, assigned_z);
}

void CGCreateRenderList(CGWindow* window)
{
    CG_ERROR_COND_EXIT(window == NULL, -1, "Failed to create render list: Window must be specified to a non-null window instance.");
    if (window->render_list != NULL)
        free(window->render_list);
    window->render_list = CGCreateLinkedListNode(NULL, CG_LIST_HEAD);
}

void CGAddRenderListNode(CGRenderNode* list_head, CGRenderNode* node)
{
    if (node == NULL || list_head == NULL)
        return;
    // using list_head as an temperary variable
    while (list_head->next != NULL)
    {
        if (*CGGetDepthPointer(CG_EXTRACT_RENDER_NODE_DATA(node)->object, node->identifier) > 
            *CGGetDepthPointer(CG_EXTRACT_RENDER_NODE_DATA(list_head->next)->object, node->identifier))
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

void CGReorganizeRenderList(CGWindow* window)
{
    CG_ERROR_CONDITION(window == NULL, "Cannot reorganize list with a NULL window");
    if(window->render_list == NULL)
        return;
    CGRenderNode* p = window->render_list->next;
    float assign_z = CG_RENDER_FAR;
    while(p != NULL)
    {
        assign_z -= 0.1;
        *CGGetAssignedZPointer(CG_EXTRACT_RENDER_NODE_DATA(p)->object, p->identifier) = assign_z;
        p = p->next;
    }
}

CGRenderObjectProperty* CGCreateRenderObjectProperty(CGColor color, CGVector2 transform, CGVector2 scale, float rotation)
{
    CGRenderObjectProperty* property = (CGRenderObjectProperty*)malloc(sizeof(CGRenderObjectProperty));
    CG_ERROR_COND_RETURN(property == NULL, NULL, "Failed to allocate memory for CGRenderObjectProperty.");
    property->color = color;
    property->transform = transform;
    property->scale = scale;
    property->rotation = rotation;
    return property;
}

float* CGCreateTransformMatrix(CGVector2 transform)
{
    float* result = (float*)malloc(sizeof(float) * 16);
    CG_ERROR_COND_RETURN(result == NULL, NULL, "failed to allocate memory for transform matrix.");
    memcpy(result, cg_normal_matrix, sizeof(float) * 16);
    result[12] = transform.x;
    result[13] = transform.y;
    
    return result;
}

float* CGCreateScaleMatrix(CGVector2 scale)
{
    float* result = (float*)malloc(sizeof(float) * 16);
    CG_ERROR_COND_RETURN(result == NULL, NULL, "Failed to allocate memory for scale matrix.");
    memcpy(result, cg_normal_matrix, sizeof(float) * 16);
    result[0] = scale.x;
    result[5] = scale.y;
    return result;
}

float* CGCreateRotateMatrix(float rotate)
{
    float* result = (float*)malloc(sizeof(float) * 16);
    CG_ERROR_COND_RETURN(result == NULL, NULL, "Failed to allocate memory for rotation matrix.");
    memcpy(result, cg_normal_matrix, sizeof(float) * 16);
    if (rotate == 0)
        return result;
    float sin_theta = sin(rotate), cos_theta = cos(rotate);
    result[0] = cos_theta;
    result[1] = sin_theta;
    result[4] = -1 * sin_theta;
    result[5] = cos_theta;
    return result;
}

void CGMatMultiply(float* result, const float* mat_1, const float* mat_2, int demention_x, int demention_y)
{
    CG_ERROR_CONDITION(result == NULL || mat_1 == NULL || mat_2 == NULL, "Unable to multiply NULL matrix");
    for (int i = 0; i < demention_y; ++i)
    {
        for (int j = 0; j < demention_x; ++j)
        {
            float value = 0;
            for (int k = 0; k < demention_x; ++k)
            {
                value += mat_1[i * demention_x + k] * mat_2[k * demention_y + j];
            }
            result[i * demention_x + j] = value;
        }
    }
}

void CGSetPropertyUniforms(CGShaderProgram shader_program, const CGRenderObjectProperty* property)
{
    CG_ERROR_CONDITION(property == NULL, "Attempting to set uniforms out of a NULL property");
    CGSetShaderUniformVec4f(shader_program, "color", 
        property->color.r, property->color.g, property->color.b, property->color.alpha);
    float result[16] = {0};
    float* tmp_mat = CGCreateTransformMatrix(property->transform);
    if (tmp_mat != NULL)
    {
        memcpy(result, tmp_mat, sizeof(float) * 16);
        free(tmp_mat);
    }
    else
        memcpy(result, cg_normal_matrix, sizeof(float) * 16);

    tmp_mat = CGCreateScaleMatrix(property->scale);
    if (tmp_mat != NULL)
    {
        CGMatMultiply(result, tmp_mat, result, 4, 4);
        free(tmp_mat);
    }

    tmp_mat = CGCreateRotateMatrix(property->rotation);
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
    result.z = 0;
    return result;
}

CGTriangle* CGCreateTriangle(CGVector2 vert_1, CGVector2 vert_2, CGVector2 vert_3)
{
    CGTriangle* result = (CGTriangle*)malloc(sizeof(CGTriangle));
    CG_ERROR_COND_RETURN(result == NULL, NULL, "Failed to allocate memory for triangle.");
    result->vert_1 = vert_1;
    result->vert_2 = vert_2;
    result->vert_3 = vert_3;
    result->z = 0;
    return result;
}

float* CGMakeTriangleVertices(const CGTriangle* triangle, float assigned_z)
{
    CG_ERROR_COND_RETURN(triangle == NULL, NULL, "Cannot make vertices array out of a triangle of value NULL.");
    float* result = (float*)malloc(sizeof(float) * 9);
    CG_ERROR_COND_RETURN(result == NULL, NULL, "Failed to allocate memory for triangle vertexes.");
    static const double denom = (CG_RENDER_FAR - CG_RENDER_NEAR);
    float depth = (assigned_z - CG_RENDER_NEAR) / denom;
    CGSetFloatArrayValue(9, result,
        triangle->vert_1.x, triangle->vert_1.y, depth,
        triangle->vert_2.x, triangle->vert_2.y, depth,
        triangle->vert_3.x, triangle->vert_3.y, depth
    );
    return result;
}

void CGBindBuffer(GLenum buffer_type, unsigned int buffer, unsigned int buffer_size, void* buffer_data, unsigned int usage)
{
    CGGladInitializeCheck();
    glBindBuffer(buffer_type, buffer);
    glBufferData(buffer_type, buffer_size, buffer_data, usage);
}

void CGRenderTriangle(const CGTriangle* triangle, const CGRenderObjectProperty* property, const CGWindow* window, float assigned_z)
{
    CG_ERROR_CONDITION(window == NULL || window->glfw_window_instance == NULL, "Attempting to draw triangle on a NULL window.");
    CG_ERROR_CONDITION(triangle == NULL, "Attempting to draw a NULL triangle object.");
    float* triangle_vertices = CGMakeTriangleVertices(triangle, assigned_z);
    CG_ERROR_CONDITION(triangle_vertices == NULL, "Failed to draw triangle.");
    CGGladInitializeCheck();
    if (glfwGetCurrentContext() != window->glfw_window_instance)
        glfwMakeContextCurrent(window->glfw_window_instance);
    if (property == NULL)
        property = cg_default_geo_property;

    //draw
    glBindVertexArray(window->triangle_vao);
    glUseProgram(cg_geo_shader_program);
    glBindBuffer(GL_ARRAY_BUFFER, cg_buffers[CG_BUFFERS_TRIANGLE_VBO]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 9 * sizeof(float), triangle_vertices);
    free(triangle_vertices);

    CGSetPropertyUniforms(cg_geo_shader_program, property);
    CGSetShaderUniform1f(cg_geo_shader_program, "render_width", (float)window->width);
    CGSetShaderUniform1f(cg_geo_shader_program, "render_height", (float)window->height);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

float* CGMakeQuadrangleVertices(const CGQuadrangle* quadrangle, float assigned_z)
{
    CG_ERROR_COND_RETURN(quadrangle == NULL, NULL, "Cannot make vertices array out of a quadrangle of value NULL.");
    float* vertices = (float*)malloc(sizeof(float) * 12);
    CG_ERROR_COND_RETURN(vertices == NULL, NULL, "Failed to allocate vertices memories.");
    float depth = (assigned_z - CG_RENDER_NEAR) / (CG_RENDER_FAR - CG_RENDER_NEAR);
    CGSetFloatArrayValue(12, vertices, 
        quadrangle->vert_1.x, quadrangle->vert_1.y, depth,
        quadrangle->vert_2.x, quadrangle->vert_2.y, depth,
        quadrangle->vert_3.x, quadrangle->vert_3.y, depth,
        quadrangle->vert_4.x, quadrangle->vert_4.y, depth
    );
    return vertices;
}

float* CGMakeVisualImageVertices(const CGVisualImage* visual_image, float assigned_z)
{
    CG_ERROR_COND_RETURN(visual_image == NULL, NULL, "Cannot make vertices array out of a visual_image of value NULL");
    float* vertices = (float*)malloc(sizeof(float) * 20);
    CG_ERROR_COND_RETURN(vertices == NULL, NULL, "Failed to allocate memory for vertices");
    float depth = (assigned_z - CG_RENDER_NEAR) / (CG_RENDER_FAR - CG_RENDER_NEAR);
    CGSetFloatArrayValue(20, vertices,
        -1 * visual_image->demention.x / 2,      visual_image->demention.y / 2, depth, 0.0, 0.0,
             visual_image->demention.x / 2,      visual_image->demention.y / 2, depth, 1.0, 0.0,
             visual_image->demention.x / 2, -1 * visual_image->demention.y / 2, depth, 1.0, 1.0,
        -1 * visual_image->demention.x / 2, -1 * visual_image->demention.y / 2, depth, 0.0, 1.0
    );
    return vertices;
}

CGQuadrangle CGConstructQuadrangle(CGVector2 vert_1, CGVector2 vert_2, CGVector2 vert_3, CGVector2 vert_4)
{
    CGQuadrangle result;
    result.vert_1 = vert_1;
    result.vert_2 = vert_2;
    result.vert_3 = vert_3;
    result.vert_4 = vert_4;
    result.z = 0.0f;
    return result;
}

CGQuadrangle* CGCreateQuadrangle(CGVector2 vert_1, CGVector2 vert_2, CGVector2 vert_3, CGVector2 vert_4)
{
    CGQuadrangle* result = (CGQuadrangle*)malloc(sizeof(CGQuadrangle));
    CG_ERROR_COND_RETURN(result == NULL, NULL, "Failed to allocate memory for CGQuadrangle object");
    result->vert_1 = vert_1;
    result->vert_2 = vert_2;
    result->vert_3 = vert_3;
    result->vert_4 = vert_4;
    result->z = 0.0f;
    return result;
}

void CGRenderQuadrangle(const CGQuadrangle* quadrangle, const CGRenderObjectProperty* property, const CGWindow* window, float assigned_z)
{
    CG_ERROR_CONDITION(window == NULL || window->glfw_window_instance == NULL, "Attempting to draw quadrangle on a NULL window.");
    CG_ERROR_CONDITION(quadrangle == NULL, "Attempting to draw a NULL quadrangle.");
    CGGladInitializeCheck();
    float* vertices = CGMakeQuadrangleVertices(quadrangle, assigned_z);
    CG_ERROR_CONDITION(vertices == NULL, "Failed to draw quadrangle.");
    if (property == NULL)
        property = cg_default_geo_property;
    
    //draw
    glBindVertexArray(window->quadrangle_vao);
    glUseProgram(cg_default_geo_shader_program);
    glBindBuffer(GL_ARRAY_BUFFER, cg_buffers[CG_BUFFERS_QUADRANGLE_VBO]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 12, vertices);
    free(vertices);
    
    CGSetPropertyUniforms(cg_geo_shader_program, property);
    CGSetShaderUniform1f(cg_geo_shader_program, "render_width", (float)window->width);
    CGSetShaderUniform1f(cg_geo_shader_program, "render_height", (float)window->height);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CGSetTextureValue(unsigned int texture_id, unsigned int vao, CGImage* texture)
{
    CG_ERROR_CONDITION(texture == NULL, "Cannot bind a NULL texture.");
    CGGladInitializeCheck();
    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    switch(texture->channels)
    {
    case 3:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->width, texture->height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture->data);
        break;
    case 4:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->data);
        break;
    default:
        CGDeleteImage(texture);
        free(texture);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        CG_ERROR_CONDITION(CG_TRUE, "Invalid image channel count. CosGraphics currently only supports images with 3 or 4 channels.");
    }
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}

CGVisualImage* CGCreateVisualImage(const char* img_path, CGWindow* window)
{
    CG_ERROR_COND_RETURN(img_path == NULL, NULL, "Cannot create image with NULL texture path.");
    CG_ERROR_COND_RETURN(window == NULL || window->glfw_window_instance == NULL, NULL, "Cannot create image with NULL window.");
    CGGladInitializeCheck();
    CGVisualImage* visual_image = (CGVisualImage*)malloc(sizeof(CGVisualImage));
    CG_ERROR_COND_RETURN(visual_image == NULL, NULL, "Failed to allocate memory for visual_image.");
    if (glfwGetCurrentContext() != window->glfw_window_instance)
        glfwMakeContextCurrent(window->glfw_window_instance);
    visual_image->in_window = window;
    CGImage* image = CGLoadImage(img_path);
    CG_ERROR_COND_RETURN(image == NULL, NULL, "Failed to create visual_image.");
    visual_image->demention.x = image->width;
    visual_image->demention.y = image->height;
    if (image == NULL)
    {
        free(visual_image);
        CG_ERROR_COND_RETURN(CG_TRUE, NULL, "Failed to allocate memory for visual_image texture.");
    }
    glGenTextures(1, &visual_image->texture_id);
    CGSetTextureValue(visual_image->texture_id, window->visual_image_vao, image);
    CGDeleteImage(image);
    return visual_image;
}

void CGDeleteVisualImage(CGVisualImage* visual_image)
{
    glDeleteTextures(1, &visual_image->texture_id);
    free(visual_image);
}

void CGRenderVisualImage(CGVisualImage* visual_image, const CGRenderObjectProperty* property, CGWindow* window, float assigned_z)
{
    CG_ERROR_CONDITION(visual_image == NULL, "Failed to draw visual_image: VisualImage must be specified to a non-null visual_image instance.");
    CG_ERROR_CONDITION(window == NULL || window->glfw_window_instance == NULL, "Failed to draw visual_image: Attempting to draw visual_image on a NULL window");
    CGGladInitializeCheck();
    float* vertices = CGMakeVisualImageVertices(visual_image, assigned_z);
    CG_ERROR_CONDITION(vertices == NULL, "Failed to draw visual_image");
    glBindVertexArray(window->visual_image_vao);
    glUseProgram(cg_visual_image_shader_program);
    glBindBuffer(GL_ARRAY_BUFFER, cg_buffers[CG_BUFFERS_VISUAL_IMAGE_VBO]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 20 * sizeof(float), vertices);
    free(vertices);
    glBindTexture(GL_TEXTURE_2D, visual_image->texture_id);
    CGSetPropertyUniforms(cg_visual_image_shader_program, property);
    CGSetShaderUniform1f(cg_visual_image_shader_program, "render_width", (float)window->width);
    CGSetShaderUniform1f(cg_visual_image_shader_program, "render_height", (float)window->height);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
