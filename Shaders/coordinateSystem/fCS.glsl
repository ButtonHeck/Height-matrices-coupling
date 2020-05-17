#version 450

in vec4 v_fColor;

out vec4 o_FragColor;

void main()
{
    o_FragColor = v_fColor;
    o_FragColor.a = 0.5;
}
