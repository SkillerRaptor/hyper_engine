#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 0) in vec4 a_color;

layout (std140, binding = 0) uniform Camera
{
    mat4 u_view_projection;
};

struct VertexOutput
{
    vec4 color;
};

layout (location = 0) out VertexOutput v_output;

void main()
{
    v_output.color = a_color;

    gl_Position = u_view_projection * vec4(a_position, 1.0);
}
