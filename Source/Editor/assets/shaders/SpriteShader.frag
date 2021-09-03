#version 450

layout (location = 0) out vec4 v_color;

struct VertexOutput
{
    vec4 color;
};

layout (location = 0) in VertexOutput v_input;

void main()
{
    v_color = v_input.color;
}
