#include "HyperUtilities/Reflection.hpp"

#include <glm/glm.hpp>

#include "HyperRendering/ShaderManager.hpp"
#include "HyperRendering/TextureManager.hpp"

namespace HyperUtilities
{
	HP_REFLECT_TYPE
	{
		rttr::registration::class_<glm::vec2>("Vector2")
			.constructor<>()
			.constructor<float, float>()
			.property("X", &glm::vec2::x)
			(
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
			)
			.property("Y", &glm::vec2::y)
			(
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
			);

		rttr::registration::class_<glm::vec3>("Vector3")
			.constructor<>()
			.constructor<float, float, float>()
			.property("X", &glm::vec3::x)
			(
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
			)
			.property("Y", &glm::vec3::y)
			(
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
			)
			.property("Z", &glm::vec3::z)
			(
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
			);

		rttr::registration::class_<glm::vec4>("Vector4")
			.constructor<>()
			.constructor<float, float, float, float>()
			.property("X", &glm::vec4::x)
			(
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
			)
			.property("Y", &glm::vec4::y)
			(
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
			)
			.property("Z", &glm::vec4::z)
			(
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
			)
			.property("W", &glm::vec4::w)
			(
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
			);

		rttr::registration::class_<HyperRendering::TextureHandle>("Texture")
			.constructor<>()
			.property("Handle", &HyperRendering::TextureHandle::Handle)
			(
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::EDITABLE)
			);
	}
}

HP_FORCE_REFLECTION_IMPLEMENTATION(Utilities);
