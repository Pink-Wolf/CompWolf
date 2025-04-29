#ifndef COMPWOLF_GRAPHICS_VULKAN_SHADER
#define COMPWOLF_GRAPHICS_VULKAN_SHADER

#include <vulkan_graphics_environments>
#include <shaders>
#include "vulkan_shader_internal.hpp"

namespace compwolf::vulkan
{
	/** A Vulkan-implementation of [[shader]].
	 * @typeparam InputType The type of element that the shader takes as input.
	 * @typeparam OutputType The type of element that the shader outputs.
	 * @typeparam FieldTypes The fields that the shader has.
	 * These must be [[type_value_pair]]s, denoting the type and position of the fields.
	 * @see shader
	 * @see vulkan_graphics_environment
	 */
	template <typename InputType, typename OutputType, typename... FieldTypes>
	class vulkan_shader
		: public shader<vulkan_graphics_environment, InputType, OutputType, FieldTypes...>
	{
		using super = shader<vulkan_graphics_environment, InputType, OutputType, FieldTypes...>;

	private:
		internal::vulkan_shader_internal _internal;

	public: // vulkan-specific
		/** Returns the [[vulkan_handle::shader]] that the shader represents. */
		auto vulkan_shader_module() const noexcept -> vulkan_handle::shader { return _internal.vulkan_shader.get(); }

	public: // constructors
		/** Constructs an invalid [[vulkan_shader]].
		 * Using this shader is undefined behaviour.
		 * @overload
		 */
		vulkan_shader() = default;
		vulkan_shader(vulkan_shader&&) = default;
		auto operator=(vulkan_shader&&) -> vulkan_shader& = default;

		/** Creates a shader on the given gpu. */
		vulkan_shader(vulkan_gpu_connection& gpu
			, const std::vector<uint32_t>& spirvCode)
			: super(gpu)
			, _internal(gpu, std::move(spirvCode))
		{
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_VULKAN_SHADER
