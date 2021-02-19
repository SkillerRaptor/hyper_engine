#include "HyperPanels/AssetsPanel.hpp"

#include <HyperEngine.hpp>

#include "HyperUtilities/AssetsManager.hpp"

namespace HyperEditor
{
	void AssetsPanel::OnUpdate()
	{
		AssetsManager::CheckAssets();
	}

	void AssetsPanel::OnRender()
	{
	}
}
