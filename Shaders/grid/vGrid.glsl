#version 450

layout (location = 0) in vec3 i_pos;
out float v_heightAbs;

const float MAX_HEIGHT = 2.0;

uniform mat4 u_projection;
uniform mat4 u_view;

void main()
{
    gl_PointSize = 4.0;
    gl_Position = u_projection * u_view * vec4(i_pos, 1.0);
    v_heightAbs = (i_pos.y / MAX_HEIGHT) * 0.8 + 0.2;
}
