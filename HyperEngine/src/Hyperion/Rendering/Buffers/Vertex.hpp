#pragma once

#include "Maths/Vector.hpp"

namespace Hyperion 
{
	struct Vertex
	{};

	struct Vertex2D : public Vertex
	{
		Vector3<> Position;
		Vector4<> Color;
		Vector2<> TexCoords;
		unsigned int TexId;
	};

	struct Vertex3D : public Vertex
	{
		Vector3<> Position;
		Vector4<> Color;
		Vector3<> Normals;
		Vector2<> TexCoords;
		Vector3<> Tangent;
		Vector3<> Bitangent;
		unsigned int TexId;
	};

	enum struct VertexLayout 
	{
		Vertex2D,
		Vertex3D
	};
}
