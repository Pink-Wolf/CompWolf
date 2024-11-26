#ifndef COMPWOLF_VULKAN_GRAPHICS_ENVIRONMENT
#define COMPWOLF_VULKAN_GRAPHICS_ENVIRONMENT

#include <graphics_environments>
#include "vulkan_gpu_connection.hpp"
#include "environment/glfw_environment.hpp"
#include "environment/vulkan_environment.hpp"
#include "environment/vulkan_debug_environment.hpp"

#include <events>

namespace compwolf::vulkan
{
	class vulkan_graphics_environment;

	/** A Vulkan implementation of [[graphics_environment]].
	 * @warning It is undefined behaviour to construct or destruct a [[vulkan_graphics_environment]] on a thread other than the one that started the program.
	 */
	class vulkan_graphics_environment : public graphics_environment<vulkan_gpu_connection>
	{
		static vulkan_graphics_environment_settings _settings;
		static glfw_environment _glfw_environment;
		static vulkan_environment _vulkan_environment;
		static vulkan_debug_environment _vulkan_debug_environment;

		static std::vector<vulkan_gpu_connection> _gpus;

	public: // constructors
		/** @overload Constructs a freed [[vulkan_graphics_environment]].
		 * See [[freeable]] for more information.
		 * @see freeable
		 */
		vulkan_graphics_environment() noexcept = default;
		vulkan_graphics_environment(vulkan_graphics_environment&&) = default;
		auto operator=(vulkan_graphics_environment&&) -> vulkan_graphics_environment& = default;
		~vulkan_graphics_environment() noexcept { free(); }

		/**
		 * @throws std::runtime_error if there was an error during setup due to causes outside of the program.
		 * @warning It is undefined behaviour to construct or destruct a [[vulkan_graphics_environment]] on a thread other than the one that started the program.
		 * @see graphics_environment_settings
		 */
		vulkan_graphics_environment(vulkan_graphics_environment_settings settings);

	public: // vulkan-specific
		/** Returns the environment's [[vulkan_handle::instance]]. */
		auto vulkan_instance() const noexcept -> vulkan_handle::instance
		{
			return _vulkan_environment.vulkan_instance();
		}

	public: // compwolf::graphics_environment
		/** @see compwolf::graphics_environment */
		auto gpus() const noexcept -> const std::vector<vulkan_gpu_connection>& final
		{
			return _gpus;
		}

		/** @see compwolf::graphics_environment */
		void update() final;

	protected:
		/** @see freeable */
		void on_free() noexcept final;
	};
}

#endif // ! COMPWOLF_VULKAN_GRAPHICS_ENVIRONMENT