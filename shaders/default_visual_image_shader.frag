#version 330 core

in vec2 tex_coord;

out vec4 FragColor;

uniform vec4 color;
uniform sampler2D frag_texture;
uniform bool is_clamped;
uniform vec2 clamp_top_left;
uniform vec2 clamp_bottom_right;
uniform vec2 image_dimension;

vec4 GetTextureColor(vec4 pixel_color, vec4 color_modifier);

void main()
{
    if (is_clamped)
    {
        vec2 dtex_coord = tex_coord * image_dimension;
            FragColor = (dtex_coord.x < clamp_top_left.x || dtex_coord.y < clamp_top_left.y
            || dtex_coord.x > clamp_bottom_right.x || dtex_coord.y > clamp_bottom_right.y) ? 
            vec4(0.0, 0.0, 0.0, 0.0) : texture(frag_texture, tex_coord) * color;
    }
    else
    {
        FragColor = texture(frag_texture, tex_coord) * color;
    }
}