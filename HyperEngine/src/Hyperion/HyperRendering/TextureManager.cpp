#include "TextureManager.hpp"

#include <stb_image.h>

#include "HyperCore/Core.hpp"

namespace Hyperion
{
	unsigned char* TextureManager::LoadImage(const std::string& path, int32_t& width, int32_t& height, int32_t& channels)
	{
		HP_ASSERT(!path.empty(), "Texture Manager can't load an empty path!");
		stbi_set_flip_vertically_on_load(true);
		return stbi_load(path.c_str(), &width, &height, &channels, 0);
	}

	void TextureManager::FreeImage(unsigned char* pixels)
	{
		HP_ASSERT(pixels, "Texture Manager can't free invalid pixels!");
		stbi_image_free(pixels);
	}
}
