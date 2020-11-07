#pragma once

#include "HyperMath/Vector.hpp"

namespace Hyperion 
{
	struct Vertex {};

	struct Vertex2D : public Vertex
	{
		Vec3 Position;
		Vec4 Color;
		Vec2 TextureCoords;
		unsigned int TextureId;
	};

	struct Vertex3D : public Vertex
	{
		Vec3 Position;
		Vec4 Color;
		unsigned int TextureId;
		Vec2 TextureCoords;
		Vec3 Normals;
		Vec3 Tangent;
		Vec3 Bitangent;
	};

	enum struct VertexLayout 
	{
		Vertex2D,
		Vertex3D
	};
}
