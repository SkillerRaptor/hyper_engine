#version 330 core

out vec4 a_FragColor;

in vec4 v_SpriteColor;
in vec2 v_TextureCoords;
in float v_TextureId;

uniform sampler2D u_Textures[32];

void main()
{
	vec4 textureColor = texture(u_Textures[int(v_TextureId)], v_TextureCoords);

	if (textureColor.a < 0.1)
		discard;

    a_FragColor = textureColor * v_SpriteColor;
}
