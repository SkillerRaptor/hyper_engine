#pragma once

#include "Maths/Vector.hpp"

namespace Hyperion 
{
	struct Vertex
	{};

	struct Vertex2D : public Vertex
	{
		Vec3 Position;
		Vec4 Color;
		Vec2 TexCoords;
		unsigned int TexId;
	};

	struct Vertex3D : public Vertex
	{
		Vec3 Position;
		Vec4 Color;
		Vec3 Normals;
		Vec2 TexCoords;
		Vec3 Tangent;
		Vec3 Bitangent;
		unsigned int TexId;
	};

	enum struct VertexLayout 
	{
		Vertex2D,
		Vertex3D
	};
}
