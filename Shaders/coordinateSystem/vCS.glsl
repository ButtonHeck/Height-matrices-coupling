#version 450

layout (location = 1) in vec3 i_color;

uniform mat4 u_projection;
uniform mat4 u_view;

out vec4 v_gColor;
out vec3 v_gDirection;

void main()
{
    //a little offset from Y=0 applied to prevent z-fighting with flat grid
    gl_Position = u_projection * u_view * vec4(0.0, 0.01, 0.0, 1.0);
    v_gColor = vec4(i_color, 1.0);

    //make direction line 40 units length
    v_gDirection = i_color * 40;
}
