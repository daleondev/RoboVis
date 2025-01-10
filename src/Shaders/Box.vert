#version 300 es

#ifdef GL_ES
precision mediump int;
precision mediump float;
#endif

uniform mat4 u_mvp;

in vec4 a_position;

void main()
{
    gl_Position = u_mvp * a_position;
}