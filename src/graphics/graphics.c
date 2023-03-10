#include "graphics.h"
#include "log/log.h"
#include "file_manager/file_manager.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define CGGladInitializeCheck() \
    if (!cg_is_glad_initialized)\
        CGInitGLAD();           \
    ((void)0)

CG_BOOL cg_is_glfw_initialized = CG_FALSE;
CG_BOOL cg_is_glad_initialized = CG_FALSE;

CGGeometryProperty* cg_default_geo_property;

unsigned int cg_vbo;
unsigned int cg_ebo;

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

const float cg_normal_matrix[16] = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};

unsigned int cg_quadrangle_indices[6] = {
    0, 1, 2,
    0, 2, 3
};

// compile one specific shader from source
CG_BOOL CGCompileShader(unsigned int shader_id, const char* shader_source);

// make the triangle vertices array from an triangle
float* CGMakeTriangleVertices(CGTriangle* triangle);

// make the quadrangle vertices array from an quadrangle
float* CGGetQuadrangleVertices(CGQuadrangle* quadrangle);

// set buffer value
void CGSetBufferValue(GLenum buffer_type, unsigned int* buffer, unsigned int buffer_size, void* buffer_data, unsigned int usage, CG_BOOL donot_unbind);

// create transform matrix
float* CGCreateTransformMatrix(CGVector2 transform);

// create scale matrix
float* CGCreateScaleMatrix(CGVector2 scale);

// create rotation matrix
float* CGCreateRotateMatrix(float rotate);

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
    glViewport(0, 0, width, height);
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
    if (cg_is_glad_initialized)
    {
        glDeleteVertexArrays(1, &cg_default_geo_shader_vao);
        glDeleteBuffers(1, &cg_vbo);
        glDeleteBuffers(1, &cg_ebo);
        glDeleteProgram(cg_default_geo_shader_program);
        CGDeleteGeometryProperty(cg_default_geo_property);
        CGDeleteShaderProgram(cg_default_geo_shader_program);
        cg_is_glad_initialized = CG_FALSE;
    }
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
        exit(-1);
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.2, 0.2, 0.2, 1.0);

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
    glEnableVertexAttribArray(0);
    glGenBuffers(1, &cg_vbo);
    glGenBuffers(1, &cg_ebo);
    glBindVertexArray(0);

    cg_default_geo_property = CGCreateGeometryProperty(
        CGConstructColor(0.8f, 0.8f, 0.8f, 1.0f), 
        CGConstructVector2(0.0f, 0.0f),
        CGConstructVector2(1.0f, 1.0f),
        0.0f);
    
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
    CGViewport* viewport = (CGViewport*)malloc(sizeof(CGViewport));
    if (viewport == NULL)
    {
        CG_ERROR("Failed to allocate memory for viewport.");
        return NULL;
    }
    
    viewport->demension.x = window->width;
    viewport->demension.y = window->height;

    glViewport(0, 0, window->width, window->height);
    glfwSetFramebufferSizeCallback(window->glfw_window_instance, CGFrameBufferSizeCallback);
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
        CG_ERROR("Failed to compile shader with id: %d. \nOutput log: %s.", shader_id, info_log);
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
        CG_ERROR("Construct shader failed.");
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
    CGGladInitializeCheck();
    if (uniform_name == NULL)
    {
        CG_ERROR("Attempting to set a uniform with a NULL name.");
        return;
    }
    GLint uniform_location = glGetUniformLocation(shader_program, uniform_name);
    glUniform1f(uniform_location, value);
}

void CGSetShaderUniformVec4f(
    CGShaderProgram shader_program, const char* uniform_name,
    float val_1, float val_2, float val_3, float val_4)
{
    CGGladInitializeCheck();
    if (uniform_name == NULL)
    {
        CG_ERROR("Attempting to set a uniform with a NULL name.");
        return;
    }
    GLint uniform_location = glGetUniformLocation(shader_program, uniform_name);
    glUniform4f(uniform_location, val_1, val_2, val_3, val_4);
}

void CGSetShaderUniformMat4f(CGShaderProgram shader_program, const char* uniform_name, const float* data)
{
    CGGladInitializeCheck();
    if (uniform_name == NULL)
    {
        CG_ERROR("Attempting to set a uniform with a NULL name.");
        return;
    }
    GLint uniform_location = glGetUniformLocation(shader_program, uniform_name);
    glUniformMatrix4fv(uniform_location, 1, GL_FALSE, data);
}

CGGeometryProperty* CGCreateGeometryProperty(CGColor color, CGVector2 transform, CGVector2 scale, float rotation)
{
    CGGeometryProperty* property = (CGGeometryProperty*)malloc(sizeof(CGGeometryProperty));
    if (property == NULL)
    {
        CG_ERROR("Failed to allocate memory for CGGeometryProperty.");
        return NULL;
    }
    property->color = color;
    property->transform = transform;
    property->scale = scale;
    property->rotation = rotation;
    return property;
}

void CGDeleteGeometryProperty(CGGeometryProperty* property)
{
    free(property);
    property = NULL;
}

float* CGCreateTransformMatrix(CGVector2 transform)
{
    float* result = (float*)malloc(sizeof(float) * 16);
    if (result == NULL)
    {
        CG_ERROR("failed to allocate memory for transform matrix.");
        return NULL;
    }
    memcpy(result, cg_normal_matrix, sizeof(float) * 16);
    result[12] = transform.x;
    result[13] = transform.y;
    
    return result;
}

float* CGCreateScaleMatrix(CGVector2 scale)
{
    float* result = (float*)malloc(sizeof(float) * 16);
    if (result == NULL)
    {
        CG_ERROR("failed to allocate memory for scale matrix.");
        return NULL;
    }
    memcpy(result, cg_normal_matrix, sizeof(float) * 16);
    result[0] = scale.x;
    result[5] = scale.y;
    return result;
}

float* CGCreateRotateMatrix(float rotate)
{
    float* result = (float*)malloc(sizeof(float) * 16);
    if (result == NULL)
    {
        CG_ERROR("Failed to allocate memory for rotation matrix.");
        return NULL;
    }
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

void CGSetMatrixesUniforms(CGGeometryProperty* property)
{
    float* tmp_mat = CGCreateTransformMatrix(property->transform);
    if (tmp_mat != NULL)
    {
        CGSetShaderUniformMat4f(cg_geo_shader_program, "transform_mat", tmp_mat);
        free(tmp_mat);
    }
    else
        CGSetShaderUniformMat4f(cg_geo_shader_program, "transform_mat", cg_normal_matrix);

    tmp_mat = CGCreateScaleMatrix(property->scale);
    if (tmp_mat != NULL)
    {
        CGSetShaderUniformMat4f(cg_geo_shader_program, "scale_mat", tmp_mat);
        free(tmp_mat);
    }
    else
        CGSetShaderUniformMat4f(cg_geo_shader_program, "scale_mat", cg_normal_matrix);

    tmp_mat = CGCreateRotateMatrix(property->rotation);
    if (tmp_mat != NULL)
    {
        CGSetShaderUniformMat4f(cg_geo_shader_program, "rotate_mat", tmp_mat);
        free(tmp_mat);
    }
    else
        CGSetShaderUniformMat4f(cg_geo_shader_program, "rotate_mat", cg_normal_matrix);
    
    tmp_mat = NULL;
}

CGTriangle CGConstructTriangle(CGVector2 vert_1, CGVector2 vert_2, CGVector2 vert_3)
{
    CGTriangle result;
    result.vert_1 = vert_1;
    result.vert_2 = vert_2;
    result.vert_3 = vert_3;
    result.z = 0;
    result.property = NULL;
    return result;
}

CGTriangle* CGCreateTriangle(CGVector2 vert_1, CGVector2 vert_2, CGVector2 vert_3)
{
    CGTriangle* result = (CGTriangle*)malloc(sizeof(CGTriangle));
    if (result == NULL)
    {
        CG_ERROR("Failed to allocate memory for triangle.");
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
        CG_ERROR("Attempting to set a property to a NULL triangle object.");
        return;
    }
    triangle->property = property;
}

float* CGMakeTriangleVertices(CGTriangle* triangle)
{
    if (triangle == NULL)
    {
        CG_ERROR("Attempting to set a property to a NULL triangle object.");
        return NULL;
    }
    float* result = (float*)malloc(sizeof(float) * 9);
    if (result == NULL)
    {
        CG_ERROR("Failed to allocate memory for triangle vertexes.");
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

void CGSetBufferValue(GLenum buffer_type, unsigned int* buffer, unsigned int buffer_size, void* buffer_data, unsigned int usage, CG_BOOL donot_unbind)
{
    CGGladInitializeCheck();
    glBindBuffer(buffer_type, *buffer);
    glBufferData(buffer_type, buffer_size, buffer_data, usage);
    if (!donot_unbind)
        glBindBuffer(buffer_type, 0);
}

void CGDrawTriangle(CGTriangle* triangle, CGWindow* window)
{
    if (window == NULL || window->glfw_window_instance == NULL)
    {
        CG_ERROR("Cannot draw triangle on a NULL window.");
        return;
    }
    if (triangle == NULL)
    {
        CG_ERROR("Attempting to draw a NULL triangle object.");
        return;
    }
    float* triangle_vertices = CGMakeTriangleVertices(triangle);
    if (triangle_vertices == NULL)
    {
        CG_ERROR("Failed to draw triangle.");
        return;
    }
    CGGladInitializeCheck();
    if (glfwGetCurrentContext() != window->glfw_window_instance)
        glfwMakeContextCurrent(window->glfw_window_instance);
    static CGGeometryProperty* property;
    if (triangle->property != NULL)
        property = triangle->property;
    else
        property = cg_default_geo_property;
    
    //draw
    CGSetBufferValue(GL_ARRAY_BUFFER, &cg_vbo, 9 * sizeof(float), triangle_vertices, GL_DYNAMIC_DRAW, GL_TRUE);
    free(triangle_vertices);
    glUseProgram(cg_geo_shader_program);
    glBindVertexArray(cg_default_geo_shader_vao);

    //set uniforms
    CGSetShaderUniformVec4f(cg_geo_shader_program, "color", 
        property->color.r, property->color.g, property->color.b, property->color.alpha);
    CGSetMatrixesUniforms(property);
    CGSetShaderUniform1f(cg_geo_shader_program, "render_width", (float)window->width);
    CGSetShaderUniform1f(cg_geo_shader_program, "render_height", (float)window->height);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
    
}

float* CGGetQuadrangleVertices(CGQuadrangle* quadrangle)
{
    if (quadrangle == NULL)
    {
        CG_ERROR("Attempting to get quadrangle vertices from a NULL CGQuadrangle");
        return NULL;
    }
    float* vertices = (float*)malloc(sizeof(float) * 12);
    if (vertices == NULL)
    {
        CG_ERROR("Failed to allocate vertices memories.");
        return NULL;
    }
    vertices[0]  = quadrangle->vert_1.x;
    vertices[1]  = quadrangle->vert_1.y;
    vertices[2]  = quadrangle->z;
    vertices[3]  = quadrangle->vert_2.x;
    vertices[4]  = quadrangle->vert_2.y;
    vertices[5]  = quadrangle->z;
    vertices[6]  = quadrangle->vert_3.x;
    vertices[7]  = quadrangle->vert_3.y;
    vertices[8]  = quadrangle->z;
    vertices[9]  = quadrangle->vert_4.x;
    vertices[10] = quadrangle->vert_4.y;
    vertices[11] = quadrangle->z;
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
    result.property = NULL;
    return result;
}

CGQuadrangle* CGCreateQuadrangle(CGVector2 vert_1, CGVector2 vert_2, CGVector2 vert_3, CGVector2 vert_4)
{
    CGQuadrangle* result = (CGQuadrangle*)malloc(sizeof(CGQuadrangle));
    if (result == NULL)
    {
        CG_ERROR("Failed to allocate memory for CGQuadrangle object");
        return NULL;
    }
    result->vert_1 = vert_1;
    result->vert_2 = vert_2;
    result->vert_3 = vert_3;
    result->vert_4 = vert_4;
    result->z = 0.0f;
    result->property = NULL;
    return result;
}

void CGDrawQuadrangle(CGQuadrangle* quadrangle, CGWindow* window)
{
    if (window == NULL || window->glfw_window_instance == NULL)
    {
        CG_ERROR("Cannot draw quadrangle on a NULL window.");
        return;
    }
    if (quadrangle == NULL)
    {
        CG_ERROR("Attempting to draw a NULL quadrangle.");
        return;
    }
    float* vertices = CGGetQuadrangleVertices(quadrangle);
    if (vertices == NULL)
    {
        CG_ERROR("Failed to draw quadrangle.");
        return;
    }
    CGGladInitializeCheck();
    if (glfwGetCurrentContext() != window->glfw_window_instance)
        glfwMakeContextCurrent(window->glfw_window_instance);
    static CGGeometryProperty* property;
    if (quadrangle->property != NULL)
        property = quadrangle->property;
    else
        property = cg_default_geo_property;
    
    CGSetBufferValue(GL_ARRAY_BUFFER, &cg_vbo, sizeof(float) * 12, vertices, GL_DYNAMIC_DRAW, CG_TRUE);
    free(vertices);

    glBindVertexArray(cg_default_geo_shader_vao);
    CGSetBufferValue(GL_ELEMENT_ARRAY_BUFFER, &cg_ebo, sizeof(unsigned int) * 6, cg_quadrangle_indices, GL_DYNAMIC_DRAW, CG_TRUE);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glUseProgram(cg_geo_shader_program);

    //set uniforms
    CGSetShaderUniformVec4f(cg_geo_shader_program, "color", 
        property->color.r, property->color.g, property->color.b, property->color.alpha);
    CGSetMatrixesUniforms(property);
    CGSetShaderUniform1f(cg_geo_shader_program, "render_width", (float)window->width);
    CGSetShaderUniform1f(cg_geo_shader_program, "render_height", (float)window->height);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}