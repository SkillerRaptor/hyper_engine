#version 330 core

out vec4 a_FragColor;

in vec4 v_SpriteColor;
in vec2 v_TextureCoords;
in float v_TextureId;

uniform sampler2D u_Textures[32];

void main()
{
	vec4 textureColor = vec4(1.0);

	switch(int(v_TextureId))
	{
		case 0: textureColor = texture(u_Textures[0], v_TextureCoords); break;
		case 1: textureColor = texture(u_Textures[1], v_TextureCoords); break;
		case 2: textureColor = texture(u_Textures[2], v_TextureCoords); break;
		case 3: textureColor = texture(u_Textures[3], v_TextureCoords); break;
		case 4: textureColor = texture(u_Textures[4], v_TextureCoords); break;
		case 5: textureColor = texture(u_Textures[5], v_TextureCoords); break;
		case 6: textureColor = texture(u_Textures[6], v_TextureCoords); break;
		case 7: textureColor = texture(u_Textures[7], v_TextureCoords); break;
		case 8: textureColor = texture(u_Textures[8], v_TextureCoords); break;
		case 9: textureColor = texture(u_Textures[9], v_TextureCoords); break;
		case 10: textureColor = texture(u_Textures[10], v_TextureCoords); break;
		case 11: textureColor = texture(u_Textures[11], v_TextureCoords); break;
		case 12: textureColor = texture(u_Textures[12], v_TextureCoords); break;
		case 13: textureColor = texture(u_Textures[13], v_TextureCoords); break;
		case 14: textureColor = texture(u_Textures[14], v_TextureCoords); break;
		case 15: textureColor = texture(u_Textures[15], v_TextureCoords); break;
		case 16: textureColor = texture(u_Textures[16], v_TextureCoords); break;
		case 17: textureColor = texture(u_Textures[17], v_TextureCoords); break;
		case 18: textureColor = texture(u_Textures[18], v_TextureCoords); break;
		case 19: textureColor = texture(u_Textures[19], v_TextureCoords); break;
		case 20: textureColor = texture(u_Textures[20], v_TextureCoords); break;
		case 21: textureColor = texture(u_Textures[21], v_TextureCoords); break;
		case 22: textureColor = texture(u_Textures[22], v_TextureCoords); break;
		case 23: textureColor = texture(u_Textures[23], v_TextureCoords); break;
		case 24: textureColor = texture(u_Textures[24], v_TextureCoords); break;
		case 25: textureColor = texture(u_Textures[25], v_TextureCoords); break;
		case 26: textureColor = texture(u_Textures[26], v_TextureCoords); break;
		case 27: textureColor = texture(u_Textures[27], v_TextureCoords); break;
		case 28: textureColor = texture(u_Textures[28], v_TextureCoords); break;
		case 29: textureColor = texture(u_Textures[29], v_TextureCoords); break;
		case 30: textureColor = texture(u_Textures[30], v_TextureCoords); break;
		case 31: textureColor = texture(u_Textures[31], v_TextureCoords); break;
	}

	if (textureColor.a < 0.1)
		discard;

    a_FragColor = textureColor * v_SpriteColor;
}
