#include "graphics.h"
#include "log/log.h"
#include "file_manager/file_manager.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string.h>
#include <stdlib.h>

CG_BOOL cg_is_glfw_initialized = CG_FALSE;
CG_BOOL cg_is_glad_initialized = CG_FALSE;



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
    GLFW_KEY_0;
    if (cg_is_glfw_initialized)
    {
        glfwTerminate();
        cg_is_glfw_initialized = CG_FALSE;
    }
}

void CGInitGLAD()
{
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        CG_ERROR("GLAD setup OpenGL loader failed");
        return;
    }

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
    if(!cg_is_glad_initialized)
        CGInitGLAD();
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
    {
        glClearColor(color.r, color.g, color.b, color.alpha);
        glClear(GL_COLOR_BUFFER_BIT);
    }
}

void CGTickRenderStart()
{
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
        CG_ERROR("Construct shader source failed");
        return NULL;
    }
    char* vert_source = CGLoadFile(vertex_path);
    CGMakeStr(result->vertex, vert_source, "Construct vertex shader source failed.");
    free(vert_source);
    char* frag_source = CGLoadFile(fragment_path);
    CGMakeStr(result->fragment, frag_source, "Construct fragment shader source failed.");
    free(frag_source);
    if (use_geometry)
    {
        char* geo_source = CGLoadFile(geometry_path);
        CGMakeStr(result->geometry, geo_source, "Construct geometry shader source failed.");
        free(geo_source);
    }
    result->use_geometry = use_geometry;
}

void CGFreeShaderSource(CGShaderSource* shader_source)
{
    if (shader_source != NULL)
    {
        free(shader_source->vertex);
        free(shader_source->fragment);
        free(shader_source->geometry);
        free(shader_source);
    }
}

// compile one specific shader from source
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
        CG_ERROR("Failed to render shader with id: %d. Output log: %s", shader_id, info_log);
        return CG_FALSE;
    }
    return CG_TRUE;
}

CGShader* CGCreateShader(CGShaderSource* shader_source)
{
    CGShader* shader = (CGShader*)malloc(sizeof(CGShader));
    if (shader_source == NULL)
    {
        CG_ERROR("Attempting to compile a NULL shader source.");
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
        return shader;
    shader->geometry = glCreateShader(GL_GEOMETRY_SHADER);
    if (!CGCompileShader(shader->geometry, shader_source->geometry))
    {
        CG_ERROR("Failed to compile geometry shader.");
        return NULL;
    }
    return shader;
}

CGShaderProgram CGCreateShaderProgram(CGShader* shader)
{
    CGShaderProgram result;
    if (shader != NULL)
    {
        glAttachShader(result, shader->vertex);
        glAttachShader(result, shader->fragment);
        if (shader->use_geometry)
            glAttachShader(result, shader->geometry);
    }
    return result;
}

void CGSetShaderUniform4f(
    CGShaderProgram shader_program, const char* uniform_name,
    float val_1, float val_2, float val_3, float val_4)
{
    if (!cg_is_glad_initialized)
        CGInitGLAD();
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
    result->z = 1;
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

// make the triangle vertices array from an triangle
float* CGMakeTriangleVertices(CGTriangle* triangle)
{
    float* result = (float*)malloc(sizeof(float) * 9);
    if (result == NULL)
    {
        CG_ERROR("Failed to create triangle");
        return NULL;
    }
    result[0] = triangle->vert_1.x;
    result[1] = triangle->vert_1.y;
    result[2] = 0;
    result[3] = triangle->vert_2.x;
    result[4] = triangle->vert_2.y;
    result[5] = 0;
    result[6] = triangle->vert_3.x;
    result[7] = triangle->vert_3.y;
    result[8] = 0;
    return result;
}

void CGDrawTrangle(CGTriangle* triangle)
{
    if (triangle == NULL)
    {
        CG_ERROR("Attempting to draw a NULL triangle object");
        return;
    }
    if (!cg_is_glad_initialized)
        CGInitGLAD();
    // draw color
    CGColor color;
    if (triangle->property != NULL)
        color = triangle->property->color;
    else
        color = CGConstructColor(0.5f, 0.5f, 0.0f, 1.0f);
    
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

}