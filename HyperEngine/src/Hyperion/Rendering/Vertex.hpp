#pragma once

#include "HyperMath/HyperMath.hpp"

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
		Vec3 Normals;
		Vec2 TextureCoords;
		Vec3 Tangent;
		Vec3 Bitangent;
		unsigned int TextureId;
	};

	enum struct VertexLayout 
	{
		Vertex2D,
		Vertex3D
	};
}
