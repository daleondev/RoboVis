#version 300 es

#ifdef GL_ES
precision mediump int;
precision mediump float;
#endif

uniform mat4 u_mvp;

attribute vec4 a_position;
attribute vec4 a_color;

varying vec4 v_color;

void main()
{
    gl_Position = u_mvp * a_position;

    v_color = a_color;
}