#version 300 es

#ifdef GL_ES
precision mediump int;
precision mediump float;
#endif

uniform vec4 u_color;

out vec4 color;

void main()
{
    color = u_color;
}