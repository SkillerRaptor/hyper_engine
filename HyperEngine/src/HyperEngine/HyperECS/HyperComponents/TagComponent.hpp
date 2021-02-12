#pragma once

#include <string>

namespace HyperECS
{
	class TagComponent
	{
	private:
		std::string m_Tag;

	public:
		TagComponent() = default;
		~TagComponent() = default;

		TagComponent(const std::string& tag);

		TagComponent(const TagComponent& other);
		TagComponent& operator=(const TagComponent& other);

		TagComponent(TagComponent&& other) noexcept;
		TagComponent& operator=(TagComponent&& other) noexcept;

		bool operator==(const TagComponent& other) const;
		bool operator!=(const TagComponent& other) const;

		inline TagComponent& SetTag(const std::string& tag)
		{
			m_Tag = tag;
			return *this;
		}

		inline const std::string& GetTag() const
		{
			return m_Tag;
		}
	};
}
