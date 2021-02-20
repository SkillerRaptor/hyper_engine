#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_SpriteColor;
layout (location = 2) in vec2 a_TextureCoords;
layout (location = 3) in float a_TextureId;

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrix;

out vec4 v_SpriteColor;

void main()
{
    gl_Position = u_ProjectionMatrix * u_ViewMatrix * vec4(a_Position, 1.0);
    v_SpriteColor = a_SpriteColor;
}
