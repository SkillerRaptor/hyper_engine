#pragma once

#include "Maths/Vector.hpp"

namespace Hyperion 
{
	struct Vertex
	{};

	struct Vertex2D : public Vertex
	{
		Vector3<float> Position;
		Vector4<float> Color;
		Vector2<float> TexCoords;
		unsigned int TexId;
	};

	struct Vertex3D : public Vertex
	{
		Vector3<float> Position;
		Vector4<float> Color;
		Vector3<float> Normals;
		Vector2<float> TexCoords;
		Vector3<float> Tangent;
		Vector3<float> Bitangent;
		unsigned int TexId;
	};

	enum struct VertexLayout 
	{
		Vertex2D,
		Vertex3D
	};
}
