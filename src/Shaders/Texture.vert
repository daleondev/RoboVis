#version 300 es

precision mediump int;
precision mediump float;

uniform mat4 u_mvp;

in vec4 a_position;
in vec2 a_texCoord;

out vec2 v_texCoord;

void main()
{
    v_texCoord = a_texCoord;
    gl_Position = u_mvp * a_position;           
}