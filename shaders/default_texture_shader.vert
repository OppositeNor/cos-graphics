#version 330 core

layout(location = 0) in vec3 vert_pos;
layout(location = 1) in vec2 vtex_coord;

uniform mat4 model_mat;

uniform float render_width;
uniform float render_height;

out tex_coord;

void main()
{
    tex_coord = vtex_coord;
    vec4 global_pos = model_mat * vec4(vert_pos.x, vert_pos.y, vert_pos.z, 1.0);
    gl_Position = vec4(global_pos.x / render_width, global_pos.y / render_height, global_pos.z, 1.0);
}