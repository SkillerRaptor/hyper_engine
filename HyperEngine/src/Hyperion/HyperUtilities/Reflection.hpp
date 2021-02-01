#pragma once

#include <rttr/registration.h>

namespace Hyperion
{
	#define HP_REFLECT_TYPE RTTR_REGISTRATION

	struct MetaInfo
	{
		constexpr static const char* FLAGS = "Flags";
		constexpr static const char* COPY_FUNCTION = "Copy";

		enum Flags : uint8_t
		{
			SERIALIZABLE = 1 << 0,
			EDITABLE = 1 << 1
		};
	};

	struct EditorInfo
	{
		constexpr static const char* EDIT_PRECISION = "Edit Precision";
		constexpr static const char* EDIT_RANGE = "Edit Name";
		constexpr static const char* INTERPRET_AS = "Interpret As";
		constexpr static const char* CUSTOM_VIEW = "Custom View";
		constexpr static const char* VIEW_CONDITION = "View Condition";
	};

	using ViewConditionFunction = bool(*)(const rttr::instance&);
	using CustomViewFunction = void(*)(rttr::instance&);
	using InstanceToVariantFunction = rttr::variant(*)(rttr::instance&);

	enum class InterpretAsInfo
	{
		DEFAULT,
		COLOR
	};

	struct Range
	{
		float Min = 0.0f;
		float Max = 0.0f;
	};

	class ReflectionMeta
	{
	public:
		template<typename T>
		ReflectionMeta(const T& obj)
		{
			rttr::variant flags = obj.get_metadata(MetaInfo::FLAGS);
			this->Flags = flags.is_valid() ? flags.to_uint32() : uint32_t{ 0 };

			rttr::variant copyFunction = obj.get_metadata(MetaInfo::COPY_FUNCTION);
			this->CopyFunction = copyFunction.is_valid() ? copyFunction.get_value<InstanceToVariantFunction>() : nullptr;

			rttr::variant precision = obj.get_metadata(EditorInfo::EDIT_PRECISION);
			this->Editor.EditPrecision = precision.is_valid() ? precision.get_value<float>() : 1.0f;

			rttr::variant editRange = obj.get_metadata(EditorInfo::EDIT_RANGE);
			this->Editor.EditRange = editRange.is_valid() ? editRange.get_value<Range>() : Range{ 0.0f, 0.0f };

			rttr::variant interpretAs = obj.get_metadata(EditorInfo::INTERPRET_AS);
			this->Editor.InterpretAs = interpretAs.is_valid() ? interpretAs.get_value<InterpretAsInfo>() : InterpretAsInfo::DEFAULT;

			rttr::variant customView = obj.get_metadata(EditorInfo::CUSTOM_VIEW);
			this->Editor.CustomView = customView.is_valid() ? customView.get_value<CustomViewFunction>() : nullptr;

			rttr::variant viewCondition = obj.get_metadata(EditorInfo::VIEW_CONDITION);
			this->Editor.ViewCondition = viewCondition.is_valid() ? viewCondition.get_value<ViewConditionFunction>() : nullptr;
		}

		uint32_t Flags = 0;
		InstanceToVariantFunction CopyFunction = nullptr;

		struct
		{
			CustomViewFunction CustomView = nullptr;
			ViewConditionFunction ViewCondition = nullptr;
			Range EditRange{ 0.0f, 0.0f };
			float EditPrecision = 1.0f;
			InterpretAsInfo InterpretAs = InterpretAsInfo::DEFAULT;
		} Editor;
	};

	template<typename T>
	rttr::variant Copy(rttr::instance& i)
	{
		return rttr::variant{ *i.try_convert<T>() };
	}
}

#define HP_FORCE_REFLECTION(TYPE) namespace Hyperion::Detail { void DynamicInitDummy##TYPE(); } \
		namespace {  struct DynamicInit##TYPE { DynamicInit##TYPE() { ::Hyperion::Detail::DynamicInitDummy##TYPE(); } } dynamicInitInstance##TYPE; }
#define HP_FORCE_REFLECTION_IMPLEMENTATION(TYPE) namespace Hyperion::Detail { void DynamicInitDummy##TYPE() { } }

HP_FORCE_REFLECTION(Utilities);
