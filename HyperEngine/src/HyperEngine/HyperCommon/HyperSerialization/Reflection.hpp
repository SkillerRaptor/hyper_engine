#pragma once

#include <HyperCore/HyperPrerequisites.hpp>

#include <rttr/registration.h>

#include <cstdint>

namespace HyperEngine
{
	#define HP_REFLECT_TYPE RTTR_REGISTRATION
	
	#define HP_FORCE_REFLECTION(type)                                   \
        namespace HyperEngine::Detail                                   \
        {                                                               \
            void DynamicInitDummy##type();                              \
        }                                                               \
                                                                        \
        namespace                                                       \
        {                                                               \
            struct DynamicInit##type                                    \
            {                                                           \
                DynamicInit##type()                                     \
                {                                                       \
                    ::HyperEngine::Detail::DynamicInitDummy##type();    \
                }                                                       \
            }                                                           \
                                                                        \
            DynamicInit##type dynamicInitInstance##type;                \
        }
	
	#define HP_FORCE_REFLECTION_IMPLEMENTATION(type)    \
        namespace HyperEngine::Detail                   \
        {                                               \
            void DynamicInitDummy##type()               \
            {                                           \
            }                                           \
        }
	
	struct MetaInfo
	{
		static constexpr const char* FLAGS = "Flags";
		static constexpr const char* COPY_FUNCTION = "Copy";
		
		enum Flags : uint8_t
		{
			SERIALIZABLE = HYPERENGINE_BIT8(0),
			EDITABLE = HYPERENGINE_BIT8(1)
		};
	};
	
	struct EditorInfo
	{
		static constexpr const char* EDIT_PRECISION = "Edit Precision";
		static constexpr const char* EDIT_RANGE = "Edit Name";
		static constexpr const char* INTERPRET_AS = "Interpret As";
		static constexpr const char* CUSTOM_VIEW = "Custom View";
		static constexpr const char* VIEW_CONDITION = "View Condition";
	};
	
	enum class InterpretAsInfo
	{
		DEFAULT,
		COLOR
	};
	
	struct Range
	{
		float min{ 0.0f };
		float max{ 0.0f };
	};
	
	using ViewConditionFunction = bool(*)(const rttr::instance&);
	using CustomViewFunction = void(*)(rttr::instance&);
	using InstanceToVariantFunction = rttr::variant(*)(rttr::instance&);
	
	class ReflectionMeta
	{
	public:
		struct Editor
		{
			CustomViewFunction customView{ nullptr };
			ViewConditionFunction viewCondition{ nullptr };
			Range editRange{ 0.0f, 0.0f };
			float editPrecision{ 1.0f };
			InterpretAsInfo interpretAs{ InterpretAsInfo::DEFAULT };
		};
	
	public:
		template <typename T>
		explicit ReflectionMeta(const T& object)
		{
			rttr::variant flags = object.get_metadata(MetaInfo::FLAGS);
			m_flags =
				flags.is_valid()
				? flags.to_uint32()
				: uint32_t{ 0 };
			
			rttr::variant copyFunction = object.get_metadata(MetaInfo::COPY_FUNCTION);
			m_copyFunction =
				copyFunction.is_valid()
				? copyFunction.get_value<InstanceToVariantFunction>()
				: nullptr;
			
			rttr::variant precision = object.get_metadata(EditorInfo::EDIT_PRECISION);
			m_editor.editPrecision =
				precision.is_valid()
				? precision.get_value<float>()
				: 1.0f;
			
			rttr::variant editRange = object.get_metadata(EditorInfo::EDIT_RANGE);
			m_editor.editRange =
				editRange.is_valid()
				? editRange.get_value<Range>()
				: Range{ 0.0f, 0.0f };
			
			rttr::variant interpretAs = object.get_metadata(EditorInfo::INTERPRET_AS);
			m_editor.interpretAs =
				interpretAs.is_valid()
				? interpretAs.get_value<InterpretAsInfo>()
			    : InterpretAsInfo::DEFAULT;
			
			rttr::variant customView = object.get_metadata(EditorInfo::CUSTOM_VIEW);
			m_editor.customView =
				customView.is_valid()
				? customView.get_value<CustomViewFunction>()
				: nullptr;
			
			rttr::variant viewCondition = object.get_metadata(EditorInfo::VIEW_CONDITION);
			m_editor.viewCondition =
				viewCondition.is_valid()
				? viewCondition.get_value<ViewConditionFunction>()
			    : nullptr;
		}
		
		inline uint32_t GetFlags() const { return m_flags; }
		inline Editor GetEditor() const { return m_editor; }
		inline InstanceToVariantFunction GetCopyFunction() const { return m_copyFunction; }
	
	public:
		uint32_t m_flags{ 0 };
		Editor m_editor{};
		InstanceToVariantFunction m_copyFunction{ nullptr };
	};
	
	template <typename T>
	inline rttr::variant Copy(rttr::instance& i)
	{
		return rttr::variant{ *i.try_convert<T>() };
	}
}