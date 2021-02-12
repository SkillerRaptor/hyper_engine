#include "HyperECS/HyperComponents/TagComponent.hpp"

#include "HyperUtilities/Reflection.hpp"

namespace HyperECS
{
	TagComponent::TagComponent(const std::string& tag)
		: m_Tag{ tag }
	{
	}

	TagComponent::TagComponent(const TagComponent& other)
		: m_Tag{ other.m_Tag }
	{
	}

	TagComponent& TagComponent::operator=(const TagComponent& other)
	{
		m_Tag = other.m_Tag;
		return *this;
	}

	TagComponent::TagComponent(TagComponent&& other) noexcept
		: m_Tag{ std::move(other.m_Tag) }
	{
	}

	TagComponent& TagComponent::operator=(TagComponent&& other) noexcept
	{
		m_Tag = std::move(other.m_Tag);
		return *this;
	}

	bool TagComponent::operator==(const TagComponent& other) const
	{
		return (m_Tag == other.m_Tag);
	}

	bool TagComponent::operator!=(const TagComponent& other) const
	{
		return !(*this == other);
	}

	HP_REFLECT_TYPE
	{
		using namespace HyperUtilities;
		
		rttr::registration::class_<TagComponent>("Tag Component")
			(
				rttr::metadata(MetaInfo::COPY_FUNCTION, Copy<TagComponent>),
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
			)
			.constructor<>()
			.constructor<const std::string&>()
			.property("Tag", &TagComponent::GetTag, &TagComponent::SetTag)
			(
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
			);
	}
}
