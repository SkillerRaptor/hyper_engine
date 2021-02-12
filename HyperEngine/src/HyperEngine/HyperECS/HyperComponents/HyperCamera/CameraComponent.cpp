#include "HyperECS/HyperComponents/HyperCamera/CameraComponent.hpp"

#include "HyperUtilities/Reflection.hpp"

namespace HyperECS
{
	CameraComponent::CameraComponent(const glm::vec4& backgroundColor, ProjectionType projectionType, float fieldOfView, const glm::vec2& clippingPlanes, const glm::vec2& viewportRect, bool primary)
		: m_BackgroundColor{ backgroundColor }, m_Projection{ projectionType }, m_FieldOfView{ fieldOfView }, m_ClippingPlanes{ clippingPlanes }, m_ViewportRect{ viewportRect }, m_Primary{ primary }
	{
	}

	CameraComponent::CameraComponent(const CameraComponent& other)
		: m_BackgroundColor{ other.m_BackgroundColor }, m_Projection{ other.m_Projection }, m_FieldOfView{ other.m_FieldOfView }, m_ClippingPlanes{ other.m_ClippingPlanes }, m_ViewportRect{ other.m_ViewportRect }, m_Primary{ other.m_Primary }
	{
	}

	CameraComponent& CameraComponent::operator=(const CameraComponent& other)
	{
		m_BackgroundColor = other.m_BackgroundColor;
		m_Projection = other.m_Projection;
		m_FieldOfView = other.m_FieldOfView;
		m_ClippingPlanes = other.m_ClippingPlanes;
		m_ViewportRect = other.m_ViewportRect;
		m_Primary = other.m_Primary;
		return *this;
	}

	CameraComponent::CameraComponent(CameraComponent&& other) noexcept
		: m_BackgroundColor{ std::move(other.m_BackgroundColor) }, m_Projection{ std::move(other.m_Projection) }, m_FieldOfView{ std::move(other.m_FieldOfView) }, m_ClippingPlanes{ std::move(other.m_ClippingPlanes) }, m_ViewportRect{ std::move(other.m_ViewportRect) }, m_Primary{ std::move(other.m_Primary) }
	{
	}

	CameraComponent& CameraComponent::operator=(CameraComponent&& other) noexcept
	{
		m_BackgroundColor = std::move(other.m_BackgroundColor);
		m_Projection = std::move(other.m_Projection);
		m_FieldOfView = std::move(other.m_FieldOfView);
		m_ClippingPlanes = std::move(other.m_ClippingPlanes);
		m_ViewportRect = std::move(other.m_ViewportRect);
		m_Primary = std::move(other.m_Primary);
		return *this;
	}

	bool CameraComponent::operator==(const CameraComponent& other) const
	{
		return (m_BackgroundColor == other.m_BackgroundColor && m_Projection == other.m_Projection && m_FieldOfView == other.m_FieldOfView && m_ClippingPlanes == other.m_ClippingPlanes && m_ViewportRect == other.m_ViewportRect && m_Primary == other.m_Primary);
	}

	bool CameraComponent::operator!=(const CameraComponent& other) const
	{
		return !(*this == other);
	}

	HP_REFLECT_TYPE
	{
		using namespace HyperUtilities;

		rttr::registration::enumeration<CameraComponent::ProjectionType>("Projection Type")
			(
				rttr::value("Orthographic", CameraComponent::ProjectionType::ORTHOGRAPHIC),
				rttr::value("Perspective", CameraComponent::ProjectionType::PERSPECTIVE)
			);

		rttr::registration::class_<CameraComponent>("Camera Component")
			(
				rttr::metadata(MetaInfo::COPY_FUNCTION, Copy<CameraComponent>),
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
			)
			.constructor<>()
			.constructor<const glm::vec4&, CameraComponent::ProjectionType, float, const glm::vec2&, const glm::vec2&, bool>()
			.property("Background Color", &CameraComponent::GetBackgroundColor, &CameraComponent::SetBackgroundColor)
			(
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
				rttr::metadata(EditorInfo::INTERPRET_AS, InterpretAsInfo::COLOR)
			)
			.property("Projection", &CameraComponent::GetProjection, &CameraComponent::SetProjection)
			(
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
			)
			.property("Field of View", &CameraComponent::GetFieldOfView, &CameraComponent::SetFieldOfView)
			(
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
				rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.1f, 179.9f })
			)
			.property("Clipping Planes", &CameraComponent::GetClippingPlanes, &CameraComponent::SetClippingPlanes)
			(
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
				rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.1f, 10000.0f })
			)
			.property("Viewport Rect", &CameraComponent::GetViewportRect, &CameraComponent::SetViewportRect)
			(
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE),
				rttr::metadata(EditorInfo::EDIT_RANGE, Range { 0.0f, 1.0f }),
				rttr::metadata(EditorInfo::EDIT_PRECISION, 0.01f)
			)
			.property("Primary", &CameraComponent::IsPrimary, &CameraComponent::SetPrimary)
			(
				rttr::metadata(MetaInfo::FLAGS, MetaInfo::SERIALIZABLE | MetaInfo::EDITABLE)
			);
	}
}
