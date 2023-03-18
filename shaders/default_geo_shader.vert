#version 330 core

layout(location = 0) in vec3 vert_pos;

uniform mat4 model_mat;

uniform float render_width;
uniform float render_height;

void main()
{
    vec4 global_pos = model_mat * vec4(vert_pos.x, vert_pos.y, vert_pos.z, 1.0);
    gl_Position = vec4(global_pos.x / render_width, global_pos.y / render_height, global_pos.z, 1.0); 
}