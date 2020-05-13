#version 450

layout (location = 0) in vec3 i_pos;

uniform mat4 u_projection;
uniform mat4 u_view;

void main()
{
    gl_PointSize = 4.0;
    gl_Position = u_projection * u_view * vec4(i_pos, 1.0);
}
