#version 330 core

in vec2 tex_coord;

out vec4 FragColor;
uniform vec4 color;
uniform sampler2D frag_texture;

void main()
{
    FragColor = texture(frag_texture, tex_coord) * color;
}