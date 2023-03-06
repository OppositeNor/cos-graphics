#version 330 core

layout(location = 0) in vec3 vert_pos;

uniform mat4 transform_mat;
uniform mat4 scale_mat;
uniform mat4 rotate_mat;

uniform float render_width;
uniform float render_height;

void main()
{
    gl_Position = scale_mat * rotate_mat * transform_mat * vec4(vert_pos.x / render_width, vert_pos.y / render_height, vert_pos.z, 1.0); 
}