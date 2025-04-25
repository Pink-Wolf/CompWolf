#ifndef COMPWOLF_GRAPHICS_VULKAN_SHADER_INTERNAL
#define COMPWOLF_GRAPHICS_VULKAN_SHADER_INTERNAL

#include <vulkan_graphics_environments>
#include <unique_deleter_ptr>
#include <utility>
#include <cstdint>

namespace compwolf::vulkan::internal
{
	/** @hidden */
	class vulkan_shader_internal
	{
	public:
		unique_deleter_ptr<vulkan_handle::shader_t> vulkan_shader{};

	public: // constructors
		/** Constructs an invalid [[vulkan_shader_internal]].
		 * Using this shader is undefined behaviour.
		 * @overload
		 */
		vulkan_shader_internal() = default;
		vulkan_shader_internal(vulkan_shader_internal&&) = default;
		auto operator=(vulkan_shader_internal&&) -> vulkan_shader_internal& = default;

		/** Creates a shader on the given gpu. */
		vulkan_shader_internal(vulkan_gpu_connection& gpu
			, const std::vector<uint32_t>& spirvCode
		);
	};
}

#endif // ! COMPWOLF_GRAPHICS_VULKAN_SHADER_INTERNAL
