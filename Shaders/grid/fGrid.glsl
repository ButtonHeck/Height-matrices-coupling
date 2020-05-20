#version 450

out vec4 o_FragColor;
in float v_heightAbs;

uniform vec4 u_color;
uniform bool u_applyHeightColoring;

void main()
{
    if (u_applyHeightColoring)
    {
        o_FragColor = vec4( vec3(u_color) * v_heightAbs, u_color.a );
    }
    else
    {
        o_FragColor = u_color;
    }
}
