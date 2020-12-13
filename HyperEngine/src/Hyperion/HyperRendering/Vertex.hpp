#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Hyperion 
{
	struct Vertex {};

	struct Vertex2D : public Vertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TextureCoords;
		unsigned int TextureId;
	};

	struct Vertex3D : public Vertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		unsigned int TextureId;
		glm::vec2 TextureCoords;
		glm::vec3 Normals;
		glm::vec3 Tangent;
		glm::vec3 Bitangent;
	};

	enum struct VertexLayout 
	{
		Vertex2D,
		Vertex3D
	};
}
