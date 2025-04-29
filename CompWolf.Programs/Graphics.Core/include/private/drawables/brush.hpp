#ifndef COMPWOLF_GRAPHICS_BRUSH
#define COMPWOLF_GRAPHICS_BRUSH

#include <graphics_environments>
#include <shaders>
#include <type_value_pair_lists>
#include <utility>
#include <concepts>

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
		environment_type::gpu_type* _gpu{};

	public: // accessors
		/** Returns the gpu that the brush is on.
		 * @customoverload
		 */
		auto gpu() noexcept -> typename environment_type::gpu_type& { return *_gpu; }
		/** Returns the gpu that the buffer is on. */
		auto gpu() const noexcept -> const typename environment_type::gpu_type& { return *_gpu; }

	public: // constructors
		/** Constructs an invalid [[brush]].
		 * Using this brush is undefined behaviour.
		 * @overload
		 */
		brush() = default;
		brush(brush&&) = default;
		auto operator=(brush&&) -> brush& = default;

		/** Creates a brush on the given gpu. */
		brush(environment_type::gpu_type& gpu) : _gpu(&gpu)
		{
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_BRUSH
