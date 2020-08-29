#pragma once

namespace Hyperion 
{
	struct Vector2 
	{
		float X;
		float Y;
	};

	struct Vector3
	{
		float X;
		float Y;
		float Z;
	};

	struct Vector4
	{
		float X;
		float Y;
		float Z;
		float W;
	};

	struct Vertex
	{};

	struct Vertex2D : public Vertex
	{
		Vector2 Position;
		Vector4 Color;
		Vector2 TexCoords;
		unsigned int TexId;
	};

	struct Vertex3D : public Vertex
	{
		Vector3 Position;
		Vector4 Color;
		Vector3 Normals;
		Vector2 TexCoords;
		Vector3 Tangent;
		Vector3 Bitangent;
		unsigned int TexId;
	};

	enum struct VertexLayout 
	{
		Vertex2D,
		Vertex3D
	};
}