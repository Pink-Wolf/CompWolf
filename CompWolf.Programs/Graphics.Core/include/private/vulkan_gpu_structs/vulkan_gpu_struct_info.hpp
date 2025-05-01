#ifndef COMPWOLF_GRAPHICS_VULKAN_GPU_STRUCT_INFO
#define COMPWOLF_GRAPHICS_VULKAN_GPU_STRUCT_INFO

#include <vulkan_graphics_environments>
#include <gpu_structs>

namespace compwolf::vulkan
{
	/** Gets the [[vulkan_handle::format]] for the given type. */
	template <typename T>
	struct get_vulkan_format
	{
		static_assert(dependent_false<T>, "Could not get the vulkan type of the given primitive type; was the given type actually a primitive?");

		/** The [[vulkan_handle::format]]. */
		static constexpr vulkan_handle::format value{};
	};

	/** Defines [[get_vulkan_format]] for the given type, with the given value. */
#define COMPWOLF_GRAPHICS_VULKAN_DEFINE_SIMPLE_GPU_PRIMITIVE(type, format_value)\
	template <>																	\
	struct get_vulkan_format<type>												\
	{																			\
		static constexpr vulkan_handle::format value = format_value;			\
	}																			\


	/** @hidden */ COMPWOLF_GRAPHICS_VULKAN_DEFINE_SIMPLE_GPU_PRIMITIVE(float, 100);
	/** @hidden */ COMPWOLF_GRAPHICS_VULKAN_DEFINE_SIMPLE_GPU_PRIMITIVE(float2, 103);
	/** @hidden */ COMPWOLF_GRAPHICS_VULKAN_DEFINE_SIMPLE_GPU_PRIMITIVE(float3, 106);
	/** @hidden */ COMPWOLF_GRAPHICS_VULKAN_DEFINE_SIMPLE_GPU_PRIMITIVE(float4, 109);

	/** @hidden */ COMPWOLF_GRAPHICS_VULKAN_DEFINE_SIMPLE_GPU_PRIMITIVE(double, 112);
	/** @hidden */ COMPWOLF_GRAPHICS_VULKAN_DEFINE_SIMPLE_GPU_PRIMITIVE(double2, 115);
	/** @hidden */ COMPWOLF_GRAPHICS_VULKAN_DEFINE_SIMPLE_GPU_PRIMITIVE(double3, 118);
	/** @hidden */ COMPWOLF_GRAPHICS_VULKAN_DEFINE_SIMPLE_GPU_PRIMITIVE(double4, 121);

	/** @hidden */ COMPWOLF_GRAPHICS_VULKAN_DEFINE_SIMPLE_GPU_PRIMITIVE(shader_int, 99);
	/** @hidden */ COMPWOLF_GRAPHICS_VULKAN_DEFINE_SIMPLE_GPU_PRIMITIVE(shader_int2, 102);
	/** @hidden */ COMPWOLF_GRAPHICS_VULKAN_DEFINE_SIMPLE_GPU_PRIMITIVE(shader_int3, 105);
	/** @hidden */ COMPWOLF_GRAPHICS_VULKAN_DEFINE_SIMPLE_GPU_PRIMITIVE(shader_int4, 108);
}

#endif // ! COMPWOLF_GRAPHICS_VULKAN_GPU_STRUCT_INFO
