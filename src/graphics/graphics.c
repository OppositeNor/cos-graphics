#include "cos_graphics/graphics.h"
#include "cos_graphics/log.h"
#include "cos_graphics/resource.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define CG_MAX_OPENGL_BUFFER_SIZE 256

#define CGGladInitializeCheck()                                 \
    if (!cg_is_glad_initialized) {                              \
        CGInitGLAD();                                           \
    }((void)0)

CG_BOOL cg_is_glfw_initialized = CG_FALSE;
CG_BOOL cg_is_glad_initialized = CG_FALSE;

CGGeometryProperty* cg_default_geo_property;
CGSpriteProperty* cg_default_sprite_property;

#define CG_BUFFERS_TRIANGLE_VBO 0
#define CG_BUFFERS_QUADRANGLE_VBO 1
#define CG_BUFFERS_QUADRANGLE_EBO 2
#define CG_BUFFERS_SPRITE_VBO 3
#define CG_BUFFERS_SPRITE_EBO 4

unsigned int cg_buffers[CG_MAX_OPENGL_BUFFER_SIZE] = {0};
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
 * @brief default shader for geometry
 */
CGShaderProgram cg_default_geo_shader_program;

/**
 * @brief shader program for drawing geometry
 */
CGShaderProgram cg_geo_shader_program;

const float cg_normal_matrix[16] = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};

// compile one specific shader from source
CG_BOOL CGCompileShader(unsigned int shader_id, const char* shader_source);

// make the triangle vertices array from an triangle
float* CGMakeTriangleVertices(CGTriangle* triangle);

// make the quadrangle vertices array from an quadrangle
float* CGGetQuadrangleVertices(CGQuadrangle* quadrangle);

// set buffer value
void CGBindBuffer(GLenum buffer_type, unsigned int buffer, unsigned int buffer_size, void* buffer_data, unsigned int usage);

// create transform matrix
float* CGCreateTransformMatrix(CGVector2 transform);

// create scale matrix
float* CGCreateScaleMatrix(CGVector2 scale);

// create rotation matrix
float* CGCreateRotateMatrix(float rotate);

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

void CGInitGLFW()
{
    CG_ERROR_EXP_EXIT(glfwInit() != GLFW_TRUE, -1, "GLFW initialization failed");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
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
        free(cg_default_geo_property);
        cg_default_geo_property = NULL;
        free(cg_default_sprite_property);
        cg_default_sprite_property = NULL;
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
    CG_ERROR_EXP_EXIT(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), -1, "GLAD setup OpenGL loader failed");

    //initialize default shader for geometries
    CGShaderSource* shader_source = 
        CGCreateShaderSourceFromPath(
            cg_default_geo_vshader_path,
            cg_default_geo_fshader_path, NULL, CG_FALSE
        );
    CG_ERROR_COND_EXIT(shader_source == NULL, -1, "Failed to create default shader source.");
    CGShader* shader = CGCreateShader(shader_source);
    CG_ERROR_COND_EXIT(shader == NULL, -1, "Failed to create default shader.");
    cg_default_geo_shader_program = CGCreateShaderProgram(shader);
    cg_geo_shader_program = cg_default_geo_shader_program;
    CGDeleteShaderSource(shader_source);
    CGDeleteShader(shader);
    cg_default_geo_property = CGCreateGeometryProperty(
        CGConstructColor(0.8f, 0.8f, 0.8f, 1.0f), 
        CGConstructVector2(0.0f, 0.0f),
        CGConstructVector2(1.0f, 1.0f),
        0.0f);
    cg_default_sprite_property = CGCreateSpriteProperty(
        (CGVector2){0.0f, 0.0f},
        (CGVector2){1.0f, 1.0f},
        0.0f);
    cg_is_glad_initialized = CG_TRUE;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glGenBuffers(5, cg_buffers);
    cg_buffer_count = 5;
}

CGWindow* CGCreateWindow(int width, int height, const char* title, CG_BOOL use_full_screen)
{
    if (!cg_is_glfw_initialized)
        CGInitGLFW();

    CGWindow* window = (CGWindow*)malloc(sizeof(CGWindow));
    CG_ERROR_COND_RETURN(window == NULL, NULL, "Failed to allocate memory for window.");
    window->width = width;
    window->height = height;
    strcpy(window->title, title);
    window->use_full_screen = use_full_screen;
    window->glfw_window_instance = glfwCreateWindow(width, height, title, 
        use_full_screen ? glfwGetPrimaryMonitor() : NULL, NULL);
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
    glDeleteVertexArrays(1, &window->sprite_vao);
    glfwDestroyWindow((GLFWwindow*)window->glfw_window_instance);
    free(window);
}

void CGCreateViewport(CGWindow* window)
{
    CG_ERROR_CONDITION(window == NULL || window->glfw_window_instance == NULL, "Attempting to create a viewport on a NULL window.");
    if (glfwGetCurrentContext() != window->glfw_window_instance)
        glfwMakeContextCurrent((GLFWwindow*)(window->glfw_window_instance));
    CGGladInitializeCheck();

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.2, 0.2, 0.2, 1.0);
    glViewport(0, 0, window->width, window->height);
    float temp_vertices[12] = 
    {0, 0, 0,       //3
     0, 0, 0,       //6
     0, 0, 0,       //9
     0, 0, 0};

    unsigned int cg_quadrangle_indices[6] = {
        0, 1, 2,
        0, 2, 3
    };
    
    float sprite_temp_vertices[20] = {
        0, 0, 0, -1,  1,
        0, 0, 0,  1,  1,
        0, 0, 0,  1, -1,
        0, 0, 0, -1, -1,
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

    // set sprite vao properties
    glGenVertexArrays(1, &window->sprite_vao);
    glBindVertexArray(window->sprite_vao);
    CGBindBuffer(GL_ARRAY_BUFFER, cg_buffers[CG_BUFFERS_SPRITE_VBO], 20 * sizeof(float), sprite_temp_vertices, GL_DYNAMIC_DRAW);
    CGBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cg_buffers[CG_BUFFERS_SPRITE_EBO], 6 * sizeof(unsigned int), cg_quadrangle_indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(3 * sizeof(float)));
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

CGGeometryProperty* CGCreateGeometryProperty(CGColor color, CGVector2 transform, CGVector2 scale, float rotation)
{
    CGGeometryProperty* property = (CGGeometryProperty*)malloc(sizeof(CGGeometryProperty));
    CG_ERROR_COND_RETURN(property == NULL, NULL, "Failed to allocate memory for CGGeometryProperty.");
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

void CGSetMatrixesUniforms(CGGeometryProperty* property)
{
    CG_ERROR_CONDITION(property == NULL, "Attempting to set uniforms out of a NULL property");
    CGSetShaderUniformVec4f(cg_geo_shader_program, "color", 
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
    CGSetShaderUniformMat4f(cg_geo_shader_program, "model_mat", result);
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
    CG_ERROR_COND_RETURN(result == NULL, NULL, "Failed to allocate memory for triangle.");
    result->vert_1 = vert_1;
    result->vert_2 = vert_2;
    result->vert_3 = vert_3;
    result->z = 0;
    result->property = NULL;
    return result;
}

void CGSetTriangleProperty(CGTriangle* triangle, CGGeometryProperty* property)
{
    CG_ERROR_CONDITION(triangle == NULL, "Attempting to set a property to a NULL triangle object.");
    triangle->property = property;
}

float* CGMakeTriangleVertices(CGTriangle* triangle)
{
    CG_ERROR_COND_RETURN(triangle == NULL, NULL, "Attempting to make vertices from a NULL triangle object.");
    float* result = (float*)malloc(sizeof(float) * 9);
    CG_ERROR_COND_RETURN(result == NULL, NULL, "Failed to allocate memory for triangle vertexes.");
    static const double denom = (CG_RENDER_FAR - CG_RENDER_NEAR);
    float depth = (triangle->z - CG_RENDER_NEAR) / denom;
    result[0] = triangle->vert_1.x;
    result[1] = triangle->vert_1.y;
    result[2] = depth;
    result[3] = triangle->vert_2.x;
    result[4] = triangle->vert_2.y;
    result[5] = depth;
    result[6] = triangle->vert_3.x;
    result[7] = triangle->vert_3.y;
    result[8] = depth;
    return result;
}

void CGBindBuffer(GLenum buffer_type, unsigned int buffer, unsigned int buffer_size, void* buffer_data, unsigned int usage)
{
    CGGladInitializeCheck();
    glBindBuffer(buffer_type, buffer);
    glBufferData(buffer_type, buffer_size, buffer_data, usage);
}

void CGDrawTriangle(CGTriangle* triangle, CGWindow* window)
{
    CG_ERROR_CONDITION(window == NULL || window->glfw_window_instance == NULL, "Attempting to draw triangle on a NULL window.");
    CG_ERROR_CONDITION(triangle == NULL, "Attempting to draw a NULL triangle object.");
    float* triangle_vertices = CGMakeTriangleVertices(triangle);
    CG_ERROR_CONDITION(triangle_vertices == NULL, "Failed to draw triangle.");
    CGGladInitializeCheck();
    if (glfwGetCurrentContext() != window->glfw_window_instance)
        glfwMakeContextCurrent(window->glfw_window_instance);
    CGGeometryProperty* property;
    if (triangle->property != NULL)
        property = triangle->property;
    else
        property = cg_default_geo_property;

    //draw
    glBindVertexArray(window->triangle_vao);
    glUseProgram(cg_geo_shader_program);
    glBindBuffer(GL_ARRAY_BUFFER, cg_buffers[CG_BUFFERS_TRIANGLE_VBO]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 9 * sizeof(float), triangle_vertices);
    free(triangle_vertices);

    CGSetMatrixesUniforms(property);
    CGSetShaderUniform1f(cg_geo_shader_program, "render_width", (float)window->width);
    CGSetShaderUniform1f(cg_geo_shader_program, "render_height", (float)window->height);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

float* CGGetQuadrangleVertices(CGQuadrangle* quadrangle)
{
    CG_ERROR_COND_RETURN(quadrangle == NULL, NULL, "Attempting to get quadrangle vertices from a NULL CGQuadrangle.");
    float* vertices = (float*)malloc(sizeof(float) * 12);
    CG_ERROR_COND_RETURN(vertices == NULL, NULL, "Failed to allocate vertices memories.");
    static const double denom = (CG_RENDER_FAR - CG_RENDER_NEAR);
    float depth = (quadrangle->z - CG_RENDER_NEAR) / denom;
    vertices[0]  = quadrangle->vert_1.x;
    vertices[1]  = quadrangle->vert_1.y;
    vertices[2]  = depth;
    vertices[3]  = quadrangle->vert_2.x;
    vertices[4]  = quadrangle->vert_2.y;
    vertices[5]  = depth;
    vertices[6]  = quadrangle->vert_3.x;
    vertices[7]  = quadrangle->vert_3.y;
    vertices[8]  = depth;
    vertices[9]  = quadrangle->vert_4.x;
    vertices[10] = quadrangle->vert_4.y;
    vertices[11] = depth;
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
    CG_ERROR_COND_RETURN(result == NULL, NULL, "Failed to allocate memory for CGQuadrangle object");
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
    CG_ERROR_CONDITION(window == NULL || window->glfw_window_instance == NULL, "Cannot draw quadrangle on a NULL window.");
    CG_ERROR_CONDITION(quadrangle == NULL, "Attempting to draw a NULL quadrangle.");
    float* vertices = CGGetQuadrangleVertices(quadrangle);
    CG_ERROR_CONDITION(vertices == NULL, "Failed to draw quadrangle.");
    CGGladInitializeCheck();
    if (glfwGetCurrentContext() != window->glfw_window_instance)
        glfwMakeContextCurrent(window->glfw_window_instance);
    CGGeometryProperty* property;
    if (quadrangle->property != NULL)
        property = quadrangle->property;
    else
        property = cg_default_geo_property;
    
    //draw
    glBindVertexArray(window->quadrangle_vao);
    glUseProgram(cg_default_geo_shader_program);
    glBindBuffer(GL_ARRAY_BUFFER, cg_buffers[CG_BUFFERS_QUADRANGLE_VBO]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 12, vertices);
    free(vertices);
    
    CGSetMatrixesUniforms(property);
    CGSetShaderUniform1f(cg_geo_shader_program, "render_width", (float)window->width);
    CGSetShaderUniform1f(cg_geo_shader_program, "render_height", (float)window->height);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

CGSpriteProperty* CGCreateSpriteProperty(CGVector2 transform, CGVector2 scale, float rotation)
{
    CGSpriteProperty* property = (CGSpriteProperty*)malloc(sizeof(CGSpriteProperty));
    CG_ERROR_COND_RETURN(property == NULL, NULL, "Failed to allocate memory for sprite property.");
    property->transform = transform;
    property->scale = scale;
    property->rotation = rotation;
    return property;
}

CGSprite* CGCreateSprite(const char* img_path, CGSpriteProperty* property, CGWindow* window)
{
    if (glfwGetCurrentContext() != window->glfw_window_instance)
        glfwMakeContextCurrent(window->glfw_window_instance);
    CG_ERROR_COND_RETURN(img_path == NULL, NULL, "Cannot create image with NULL texture path.");
    CG_ERROR_COND_RETURN(window == NULL, NULL, "Cannot create image with NULL window.");
    CGSprite* sprite = (CGSprite*)malloc(sizeof(CGSprite));
    CG_ERROR_COND_RETURN(sprite == NULL, NULL, "Failed to allocate memory for sprite.");
    sprite->in_window = window;
    CGImage* image = CGLoadImage(img_path);
    sprite->demention.x = image->width;
    sprite->demention.y = image->height;
    if (image == NULL)
    {
        free(sprite);
        CG_ERROR_COND_RETURN(CG_TRUE, NULL, "Failed to allocate memory for sprite texture.");
    }
    glGenBuffers(1, &sprite->texture_id);
    glBindVertexArray(window->sprite_vao);
    glBindBuffer(GL_TEXTURE_2D, sprite->texture_id);
    switch(image->channels)
    {
    case 3:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);
        break;
    case 4:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->data);
        break;
    default:
        CGDeleteImage(image);
        glBindVertexArray(0);
        glBindBuffer(GL_TEXTURE_2D, 0);
        CG_ERROR_COND_RETURN(CG_TRUE, NULL, "Invalid image channel count. CosGraphics currently only supports images with 3 or 4 channels.");
        break;
    }
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindVertexArray(0);
    glBindBuffer(GL_TEXTURE_2D, 0);
    CGDeleteImage(image);
}