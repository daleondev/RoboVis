#version 300 es

#ifdef GL_ES
precision mediump int;
precision mediump float;
#endif

in vec4 v_color;

out vec4 fragColor;

void main()
{
    fragColor = v_color;
}