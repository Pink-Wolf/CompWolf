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
		requires std::constructible_from<ShaderType, typename ShaderType::environment_type::gpu_type&, std::vector<uint32_t>&>
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
		static inline std::map<typename environment_type::gpu_type*, shader_type> _shaders;
		static inline std::vector<uint32_t> _code = shader_code_from_file(CodePath);

	public: // accessors
		/** Returns the path to the shader file. */
		static constexpr auto path() noexcept -> const char* { return CodePath; }

		/** Returns the shader instance for the given gpu. */
		static auto get(typename environment_type::gpu_type& gpu) noexcept -> shader_type&
		{
			auto key = &gpu;
			auto i = _shaders.find(key);
			if (i != _shaders.end()) return i->second;
			return _shaders.emplace(key
				, shader_type(gpu, _code)
			).first->second;
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_STATIC_SHADER
