#ifndef COMPWOLF_GRAPHICS_BRUSH
#define COMPWOLF_GRAPHICS_BRUSH

#include <graphics_environments>
#include <shaders>
#include <type_value_pair_lists>
#include <utility>
#include <concepts>
#include <stdexcept>

namespace compwolf
{
	/** Represents some code that can be used to draw objects on a window.
	 * This code is a pipeline made out of shaders.
	 *
	 * This class is abstract; a derived type should generally implement/override:
	 * * [[brush::draw]]
	 *
	 * @typeparam GraphicsEnvironmentType The type of [[graphics_environment]] that this buffer works with.
	 * @typeparam InputShaderType The type of vertex shader used by the brush.
	 * That is, when drawing an object, the shader used to modify the object's elements/vertices.
	 * @typeparam PixelShaderType The type of fragment shader used by the brush.
	 * That is, when drawing an object, the shader used to determine the individual pixel colors of the drawing.
	 */
	template <typename GraphicsEnvironmentType, typename InputShaderType, typename PixelShaderType>
		requires (std::same_as<typename InputShaderType::output_Type, typename PixelShaderType::input_type>
			&& std::same_as<typename PixelShaderType::output_type, pixel_output_type>)
	class brush
	{
	public:
		/** The type of [[graphics_environment]] that this brush works with. */
		using environment_type = GraphicsEnvironmentType;
		/** The type of vertex shader used by the brush.
		 * That is, when drawing an object, the shader used to modify the object's elements/vertices.
		 */
		using input_shader_type = InputShaderType;
		/** The type of fragment shader used by the brush.
		 * That is, when drawing an object, the shader used to determine the individual pixel colors of the drawing.
		 */
		using pixel_shader_type = PixelShaderType;

		/** The type of element that an object must be made out of to be drawn by this brush. */
		using input_type = typename input_shader_type::input_type;

		/** The type and position of the brush's shaders' fields, in a [[type_list]]. */
		using field_types = merge_type_value_pairs_by_value<
			typename input_shader_type::field_types,
			typename pixel_shader_type::field_types
		>;
		

	private:
		input_shader_type* _input_shader{};
		pixel_shader_type* _pixel_shader{};

	public: // accessors
		/** Returns the gpu that the brush is on.
		 * @customoverload
		 */
		auto gpu() noexcept -> typename environment_type::gpu_type& { return _input_shader->gpu(); }
		/** Returns the gpu that the brush is on. */
		auto gpu() const noexcept -> const typename environment_type::gpu_type& { return _input_shader->gpu(); }
		
		/** Returns the input [[shader]] that the brush uses.
		 * @customoverload
		 */
		auto input_shader() noexcept -> typename input_shader_type& { return *_input_shader; }
		/** Returns the input [[shader]] that the brush uses. */
		auto input_shader() const noexcept -> const typename input_shader_type& { return *_input_shader; }
		
		/** Returns the pixel [[shader]] that the brush uses.
		 * @customoverload
		 */
		auto pixel_shader() noexcept -> typename pixel_shader_type& { return *_pixel_shader; }
		/** Returns the pixel [[shader]] that the brush uses. */
		auto pixel_shader() const noexcept -> const typename pixel_shader_type& { return *_pixel_shader; }

	public: // constructors
		/** Constructs an invalid [[brush]].
		 * Using this brush is undefined behaviour.
		 * @overload
		 */
		brush() = default;
		brush(brush&&) = default;
		auto operator=(brush&&) -> brush& = default;

		/** Creates a brush on the given gpu. */
		brush(input_shader_type& input_shader, pixel_shader_type& pixel_shader)
			: _input_shader(&input_shader), _pixel_shader(&pixel_shader)
		{
			if (&input_shader.gpu() != pixel_shader.gpu())
				throw std::invalid_argument("Brush was given shaders for different GPUs");
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_BRUSH
