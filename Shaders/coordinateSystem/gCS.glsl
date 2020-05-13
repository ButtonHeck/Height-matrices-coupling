#version 450

layout (points) in;
layout (line_strip, max_vertices = 2) out;

out vec4 v_fColor;

in vec4 v_gColor[];
in vec3 v_gDirection[];

uniform mat4 u_projection;
uniform mat4 u_view;

void main()
{
    v_fColor = v_gColor[0];
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + u_projection * u_view * vec4(v_gDirection[0], 0.0);
    EmitVertex();
    EndPrimitive();
}
