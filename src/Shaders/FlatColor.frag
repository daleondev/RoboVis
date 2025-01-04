#version 300 es

#ifdef GL_ES
precision lowp int;
precision lowp float;
#endif

uniform vec4 u_color;

out vec4 color;

void main()
{
    color = u_color;
}