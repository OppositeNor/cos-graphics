#include "graphics.h"
#include "log/log.h"
#include "file_manager/file_manager.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string.h>
#include <stdlib.h>

#define CGGladInitializeCheck() \
    if (!cg_is_glad_initialized)\
        CGInitGLAD();           \
    ((void)0)

CG_BOOL cg_is_glfw_initialized = CG_FALSE;
CG_BOOL cg_is_glad_initialized = CG_FALSE;

unsigned int cg_vbo;

/**
 * @brief vertex shader path for a geometry
 */
const char* cg_default_geo_vshader_path = "./src/graphics/shaders/default_geo_shader.vert";
/**
 * @brief fragment shader path for a geometry
 */
const char* cg_default_geo_fshader_path = "./src/graphics/shaders/default_geo_shader.frag";

/**
 * @brief default shader for geometry
 */
CGShaderProgram cg_default_geo_shader_program;

/**
 * @brief shader program for drawing geometry
 */
CGShaderProgram cg_geo_shader_program;

/**
 * @brief default vao for geometry
 */
unsigned int cg_default_geo_shader_vao;

// compile one specific shader from source
CG_BOOL CGCompileShader(unsigned int shader_id, const char* shader_source);

// make the triangle vertices array from an triangle
float* CGMakeTriangleVertices(CGTriangle* triangle);

// set vbo value
void CGSetVBOValue(unsigned int* vbo, unsigned int vertices_size, float* vertices, unsigned int usage, CG_BOOL donot_unbind);

// create geometry's vbo
unsigned int CGCreateGeoVBO(unsigned int vertices_size, float* vertices, unsigned int usage, CG_BOOL donot_unbind);

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

void CGInitGLFW()
{
    if (!glfwInit())
    {
        CG_ERROR("GLFW initialization failed");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    cg_is_glfw_initialized = CG_TRUE;
}

void CGTerminateGraphics()
{
    if (cg_is_glfw_initialized)
    {
        glfwTerminate();
        cg_is_glfw_initialized = CG_FALSE;
    }
    if (cg_is_glad_initialized)
    {
        glDeleteProgram(cg_default_geo_shader_program);
        glDeleteVertexArrays(1, &cg_default_geo_shader_vao);
    }
}

void CGInitGLAD()
{
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        CG_ERROR("GLAD setup OpenGL loader failed");
        exit(-1);
    }

    //initialize default shader for geometries
    CGShaderSource* shader_source = 
        CGCreateShaderSourceFromPath(
            cg_default_geo_vshader_path,
            cg_default_geo_fshader_path, NULL, CG_FALSE
        );
    if (shader_source == NULL)
    {
        CG_ERROR("Failed to create default shader.");
        exit(-1);
    }
    CGShader* shader = CGCreateShader(shader_source);
    if (shader == NULL)
    {
        CG_ERROR("Failed to create default shader.");
        exit(-1);
    }
    cg_default_geo_shader_program = CGCreateShaderProgram(shader);
    cg_geo_shader_program = cg_default_geo_shader_program;
    CGDeleteShaderSource(shader_source);
    CGDeleteShader(shader);
    
    // initialize default vao for geometry shader
    glGenVertexArrays(1, &cg_default_geo_shader_vao);
    glBindVertexArray(cg_default_geo_shader_vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glGenBuffers(1, &cg_vbo);
    glBindVertexArray(0);

    cg_is_glad_initialized = CG_TRUE;
}

CGWindow* CGCreateWindow(int width, int height, const char* title, CG_BOOL use_full_screen)
{
    if (!cg_is_glfw_initialized)
        CGInitGLFW();
    
    CGWindow* window = (CGWindow*)malloc(sizeof(CGWindow));
    if (window == NULL)
    {
        CG_ERROR("Create window failed");
        return NULL;
    }
    window->width = width;
    window->height = height;
    strcpy(window->title, title);
    window->use_full_screen = use_full_screen;
    window->glfw_window_instance = glfwCreateWindow(width, height, title, 
        use_full_screen ? glfwGetPrimaryMonitor() : NULL, NULL);
    if (window->glfw_window_instance == NULL)
    {
        CG_ERROR("Create window failed");
        glfwTerminate();
        free(window);
        return NULL;
    }
    if (CGCreateViewport(window) == NULL)
        return NULL;

    return window;
}

CGViewport* CGCreateViewport(CGWindow* window)
{
    if (window == NULL || window->glfw_window_instance == NULL)
    {
        CG_ERROR("Attempting to create a viewport on a NULL window");
        return NULL;
    }
    if (glfwGetCurrentContext() != window->glfw_window_instance)
        glfwMakeContextCurrent((GLFWwindow*)(window->glfw_window_instance));
    CGGladInitializeCheck();
    if (!cg_is_glad_initialized)
        return NULL;
    CGViewport* viewport = (CGViewport*)malloc(sizeof(CGViewport));
    if (viewport == NULL)
        return NULL;
    
    viewport->demension.x = window->width;
    viewport->demension.y = window->height;

    glViewport(0, 0, window->width, window->height);

    return viewport;
}

void CGSetClearScreenColor(const CGColor color)
{
    if (cg_is_glad_initialized)
        glClearColor(color.r, color.g, color.b, color.alpha);
}

void CGTickRenderStart()
{
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT);
    //check OpenGL error
    int gl_error_code = glGetError();
    if (gl_error_code != GL_NO_ERROR)
    {
        CG_ERROR("OpenGL Error: Error code: 0x%x.", gl_error_code);
        exit(-1);
    }
}

void CGTickRenderEnd(CGWindow* window)
{
    glfwSwapBuffers((GLFWwindow*)window->glfw_window_instance);
}

CG_BOOL CGShouldWindowClose(CGWindow* window)
{
    return (CG_BOOL)glfwWindowShouldClose((GLFWwindow*)window->glfw_window_instance);
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
        return NULL;
    }
    result->fragment = CGLoadFile(fragment_path);
    if (result->fragment == NULL)
    {
        CG_ERROR("Load fragment shader source failed.");
        return NULL;
    }
    if (use_geometry)
    {
        result->geometry = CGLoadFile(geometry_path);
        if (result->geometry == NULL)
        {
            CG_ERROR("Load geometry shader source failed.");
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
    if (shader_source == NULL)
    {
        CG_ERROR("Attempting to compile a shader with a NULL source.");
        return CG_FALSE;
    }
    glShaderSource(shader_id, 1, &shader_source, NULL);
    glCompileShader(shader_id);

    int success;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);

    char info_log[CG_INFO_LOG_SIZE];

    if (!success)
    {
        glGetShaderInfoLog(shader_id, CG_INFO_LOG_SIZE, NULL, info_log);
        CG_ERROR("Failed to compile shader with id: %d. \nOutput log: %s", shader_id, info_log);
        return CG_FALSE;
    }
    return CG_TRUE;
}

CGShader* CGCreateShader(CGShaderSource* shader_source)
{
    if (shader_source == NULL)
    {
        CG_ERROR("Attempting to compile a NULL shader source.");
        return NULL;
    }
    CGShader* shader = (CGShader*)malloc(sizeof(CGShader));
    if (shader == NULL)
    {
        CG_ERROR("Construct shader failed");
        return NULL;
    }
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

void CGSetShaderUniform4f(
    CGShaderProgram shader_program, const char* uniform_name,
    float val_1, float val_2, float val_3, float val_4)
{
    CGGladInitializeCheck();
    if (uniform_name == NULL)
    {
        CG_ERROR("Attempting to set a uniform with name \"NULL\"");
        return;
    }
    GLint uniform_location = glGetUniformLocation(shader_program, uniform_name);
    glUniform4f(uniform_location, val_1, val_2, val_3, val_4);
}

CGTriangle CGConstructTriangle(CGVector2 vert_1, CGVector2 vert_2, CGVector2 vert_3)
{
    CGTriangle result;
    result.vert_1 = vert_1;
    result.vert_2 = vert_2;
    result.vert_3 = vert_3;
    result.z = 1;
    result.property = NULL;
    return result;
}

CGTriangle* CGCreateTriangle(CGVector2 vert_1, CGVector2 vert_2, CGVector2 vert_3)
{
    CGTriangle* result = (CGTriangle*)malloc(sizeof(CGTriangle));
    if (result == NULL)
    {
        CG_ERROR("Failed to allocate memory for triangle");
        return NULL;
    }
    result->vert_1 = vert_1;
    result->vert_2 = vert_2;
    result->vert_3 = vert_3;
    result->z = 0;
    result->property = NULL;
    return result;
}

void CGSetTriangleProperty(CGTriangle* triangle, CGGeometryProperty* property)
{
    if (triangle == NULL)
    {
        CG_ERROR("Attempting to set a property to a NULL triangle object");
        return;
    }
    triangle->property = property;
}

float* CGMakeTriangleVertices(CGTriangle* triangle)
{
    float* result = (float*)malloc(sizeof(float) * 9);
    if (result == NULL)
    {
        CG_ERROR("Failed to allocate memory for triangle vertexes.");
        return NULL;
    }
    if (triangle == NULL)
    {
        CG_ERROR("Attempting to set a property to a NULL triangle object");
        return NULL;
    }
    result[0] = triangle->vert_1.x;
    result[1] = triangle->vert_1.y;
    result[2] = triangle->z;
    result[3] = triangle->vert_2.x;
    result[4] = triangle->vert_2.y;
    result[5] = triangle->z;
    result[6] = triangle->vert_3.x;
    result[7] = triangle->vert_3.y;
    result[8] = triangle->z;
    return result;
}

void CGSetVBOValue(unsigned int* vbo, unsigned int vertices_size, float* vertices, unsigned int usage, CG_BOOL donot_unbind)
{
    CGGladInitializeCheck();
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, usage);
    if (!donot_unbind)
        glBindBuffer(GL_ARRAY_BUFFER, 0);
}

unsigned int CGCreateGeoVBO(unsigned int vertices_size, float* vertices, unsigned int usage, CG_BOOL donot_unbind)
{
    unsigned int result;
    glGenBuffers(1, &result);
    CGSetVBOValue(&result, vertices_size, vertices, usage, donot_unbind);
    return result;
}

void CGDrawTrangle(CGTriangle* triangle)
{
    if (triangle == NULL)
    {
        CG_ERROR("Attempting to draw a NULL triangle object");
        return;
    }
    CGGladInitializeCheck();
    // color
    static CGColor color;
    if (triangle->property != NULL)
        color = triangle->property->color;
    else
        color = CGConstructColor(1.0f, 0.5f, 0.0f, 1.0f);
    
    float* triangle_vertices = CGMakeTriangleVertices(triangle);
    if (triangle_vertices == NULL)
    {
        CG_ERROR("Failed to draw triangle.");
        return;
    }

    // draw
    CGSetVBOValue(&cg_vbo, 9 * sizeof(float), triangle_vertices, GL_DYNAMIC_DRAW, GL_TRUE);
    glUseProgram(cg_geo_shader_program);
    glBindVertexArray(cg_default_geo_shader_vao);
    CGSetShaderUniform4f(cg_geo_shader_program, "color", color.r, color.g, color.b, color.alpha);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
    glDeleteBuffers(1, &cg_vbo);
    free(triangle_vertices);
}