#pragma once

#include <random>

namespace Hyperion
{
	class Random
	{
	private:
		static std::mt19937 s_RandomEngine;

	public:
		static void Init();

		static int Int(int start, int end);

		static float Float();
		static float Float(int start, int end);

		static double Double();
		static double Double(int start, int end);

		static bool Bool();
	};	
}