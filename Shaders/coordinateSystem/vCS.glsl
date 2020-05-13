#version 450

layout (location = 0) in vec3 i_pos;
layout (location = 1) in vec3 i_color;

uniform mat4 u_projection;
uniform mat4 u_view;

out vec4 v_gColor;
out vec3 v_gDirection;

void main()
{
    gl_Position = u_projection * u_view * vec4(i_pos, 1.0);
    v_gColor = vec4(i_color, 1.0);

    //make direction line 50 units length
    v_gDirection = i_color * 50;
}
