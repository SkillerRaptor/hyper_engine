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

		static int16_t Int16();
		static int16_t Int16(int16_t start, int16_t end);

		static int32_t Int32();
		static int32_t Int32(int32_t start, int32_t end);

		static int64_t Int64();
		static int64_t Int64(int64_t start, int64_t end);

		static uint16_t UInt16();
		static uint16_t UInt16(uint16_t start, uint16_t end);

		static uint32_t UInt32();
		static uint32_t UInt32(uint32_t start, uint32_t end);

		static uint64_t UInt64();
		static uint64_t UInt64(uint64_t start, uint64_t end);

		static float Float();
		static float Float(float start, float end);

		static double Double();
		static double Double(double start, double end);

		static bool Bool();
	};
}
