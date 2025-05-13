#ifndef COMPWOLF_GRAPHICS_STATIC_SHADER
#define COMPWOLF_GRAPHICS_STATIC_SHADER

#include <concepts>
#include <map>
#include "shader.hpp"

namespace compwolf
{
	/** A static container of a shader.
	 * @typeparam CodePath The path to the file containing the shader's SPIR-V code.
	 * @typeparam ShaderType The type of shader this contains.
	 * @see shader
	 */
	template <const char* const CodePath, typename ShaderType>
		requires (std::constructible_from<ShaderType, typename ShaderType::environment_type::gpu_type&, std::vector<uint32_t>&>)
	class static_shader
	{
	public:
		/** The type of shader this static shader represents. */
		using shader_type = ShaderType;

		/** The type of [[graphics_environment]] that this shader works with. */
		using environment_type = typename shader_type::environment_type;
		/** The type of element that the shader takes as input. */
		using input_type = typename shader_type::input_type;
		/** The type of element that the shader outputs. */
		using output_type = typename shader_type::output_type;
		/** The type and position of the shader's fields, in a [[type_list]]. */
		using field_types = typename shader_type::field_types;

	private:
		struct element_constructor
		{
			static auto func(void*, typename environment_type::gpu_type& gpu)
			{
				auto code = shader_code_from_file(CodePath);
				return shader_type(gpu, code);
			}
		};
		using map_type = gpu_map<ShaderType, element_constructor>;
		static inline map_type _shaders;

	public: // accessors
		/** Returns the path to the shader file. */
		static constexpr auto path() noexcept -> const char* { return CodePath; }

		/** Returns the shader instance for the given gpu. */
		static auto get(typename environment_type::gpu_type& gpu) noexcept -> shader_type&
		{
			if (!_shaders.is_valid())
			{
				auto code = shader_code_from_file(CodePath);
				// in-place construction so assignment does not break pointers
				_shaders.~map_type();
				new (&_shaders)map_type(gpu, code);
			}

			return _shaders.get(gpu);
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_STATIC_SHADER
