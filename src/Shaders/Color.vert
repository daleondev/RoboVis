#version 300 es

#ifdef GL_ES
precision mediump int;
precision mediump float;
#endif

uniform mat4 u_mvp;

in vec4 a_position;
in vec4 a_color;

out vec4 v_color;

void main()
{
    gl_Position = u_mvp * a_position;

    v_color = a_color;
}