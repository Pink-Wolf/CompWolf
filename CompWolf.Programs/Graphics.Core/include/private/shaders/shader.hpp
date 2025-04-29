#ifndef COMPWOLF_GRAPHICS_SHADER
#define COMPWOLF_GRAPHICS_SHADER

#include <type_value_pairs>
#include <type_lists>
#include <utility>

namespace compwolf
{
	struct pixel_output_type;

	/** Represents some code on a gpu.
	 * @typeparam GraphicsEnvironmentType The type of [[graphics_environment]] that this shader works with.
	 * @typeparam InputType The type of element that the shader takes as input.
	 * @typeparam OutputType The type of element that the shader outputs.
	 * @typeparam FieldTypes The fields that the shader has.
	 * These must be [[type_value_pair]]s, denoting the type and position of the fields.
	 * These must be sorted by position.
	 * @warning It is undefined behaviour if the given FieldTypes are not sorted by position.
	 */
	template <typename GraphicsEnvironmentType, typename InputType, typename OutputType, typename... FieldTypes>
	class shader
	{
	public:
		/** The type of [[graphics_environment]] that this shader works with. */
		using environment_type = GraphicsEnvironmentType;
		/** The type of element that the shader takes as input. */
		using input_Type = InputType;
		/** The type of element that the shader outputs. */
		using output_Type = OutputType;
		/** The type and position of the shader's fields, in a [[type_list]]. */
		using field_types = type_list<FieldTypes...>;

	private:
		environment_type::gpu_type* _gpu{};

	public: // accessors
		/** Returns the gpu that the buffer is on.
		 * @customoverload
		 */
		auto gpu() noexcept -> typename environment_type::gpu_type& { return *_gpu; }
		/** Returns the gpu that the buffer is on. */
		auto gpu() const noexcept -> const typename environment_type::gpu_type& { return *_gpu; }

	public: // constructors
		/** Constructs an invalid [[shader]].
		 * Using this shader is undefined behaviour.
		 * @overload
		 */
		shader() = default;
		shader(shader&&) = default;
		auto operator=(shader&&) -> shader& = default;

		/** Creates a shader on the given gpu. */
		shader(environment_type::gpu_type& gpu) : _gpu(&gpu)
		{
		}
	};
}

#endif // ! COMPWOLF_GRAPHICS_SHADER
