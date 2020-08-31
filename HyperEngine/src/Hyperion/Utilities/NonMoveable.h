#pragma once

namespace Hyperion
{
	class NonMoveable
	{
	protected:
		NonMoveable() = default;
		~NonMoveable() = default;

		NonMoveable(const NonMoveable&&) = delete;
		NonMoveable& operator=(const NonMoveable&&) = delete;
	};
}