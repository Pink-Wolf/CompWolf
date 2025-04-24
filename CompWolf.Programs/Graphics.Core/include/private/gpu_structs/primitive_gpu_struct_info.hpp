#ifndef COMPWOLF_GRAPHICS_PRIMITIVE_GPU_STRUCT_INFO
#define COMPWOLF_GRAPHICS_PRIMITIVE_GPU_STRUCT_INFO

#include "gpu_struct_info.hpp"
#include <cstdint>

namespace compwolf
{
	/** Defines the given type as a primitive type for the GPU. */
#define COMPWOLF_GRAPHICS_DEFINE_SIMPLE_GPU_PRIMITIVE(type)						\
	struct gpu_struct_info<type>												\
	{																			\
		using primitives = type_list<type>;										\
		static consteval std::array<std::size_t, 1> primitive_offsets{ 0 };		\
	}																			\

	/** @hidden */
	COMPWOLF_GRAPHICS_DEFINE_SIMPLE_GPU_PRIMITIVE(float);
	/** @hidden */
	COMPWOLF_GRAPHICS_DEFINE_SIMPLE_GPU_PRIMITIVE(float2);
	/** @hidden */
	COMPWOLF_GRAPHICS_DEFINE_SIMPLE_GPU_PRIMITIVE(float3);
	/** @hidden */
	COMPWOLF_GRAPHICS_DEFINE_SIMPLE_GPU_PRIMITIVE(float4);

	/** @hidden */
	COMPWOLF_GRAPHICS_DEFINE_SIMPLE_GPU_PRIMITIVE(double);
	/** @hidden */
	COMPWOLF_GRAPHICS_DEFINE_SIMPLE_GPU_PRIMITIVE(double2);
	/** @hidden */
	COMPWOLF_GRAPHICS_DEFINE_SIMPLE_GPU_PRIMITIVE(double3);
	/** @hidden */
	COMPWOLF_GRAPHICS_DEFINE_SIMPLE_GPU_PRIMITIVE(double4);

	/** The type of integer that can be used on the GPU. */
	using shader_int = uint32_t;
	/** @hidden */
	COMPWOLF_DEFINE_DIMENSIONAL_ARRAY_TYPES(shader_int);
	/** @hidden */
	COMPWOLF_GRAPHICS_DEFINE_SIMPLE_GPU_PRIMITIVE(shader_int);
	/** @hidden */
	COMPWOLF_GRAPHICS_DEFINE_SIMPLE_GPU_PRIMITIVE(shader_int2);
	/** @hidden */
	COMPWOLF_GRAPHICS_DEFINE_SIMPLE_GPU_PRIMITIVE(shader_int3);
	/** @hidden */
	COMPWOLF_GRAPHICS_DEFINE_SIMPLE_GPU_PRIMITIVE(shader_int4);
}

#endif // ! COMPWOLF_GRAPHICS_PRIMITIVE_GPU_STRUCT_INFO
