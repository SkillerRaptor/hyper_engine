#pragma once

#include "HyperUtilities/AssetsManager.hpp"

namespace HyperEditor
{
	class AssetsPanel
	{
	public:
		AssetsPanel() = default;
		~AssetsPanel() = default;

		void OnUpdate();
		void OnRender();
	};
}
