#version 450

out vec4 o_FragColor;

uniform vec4 u_color;

void main()
{
    o_FragColor = u_color;
}
