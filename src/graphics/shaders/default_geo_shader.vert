#version 330 core

layout(location = 0) in vec3 vert_pos;

void main()
{
    gl_Position = vec4(vert_pos.x, vert_pos.y, vert_pos.z, 1.0);
}