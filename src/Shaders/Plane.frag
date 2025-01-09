#version 300 es

precision mediump int;
precision mediump float;

out vec4 color;

in vec2 v_texCoord;

uniform sampler2D u_texture;
uniform vec4 u_color;
uniform int u_useTex;

void main()
{
    if (u_useTex == 1)
        color = texture(u_texture, v_texCoord); 
    else
        color = u_color;
}