#ifndef COMPWOLF_DESTRUCT_EVENT
#define COMPWOLF_DESTRUCT_EVENT

#include "event.hpp"
#include <utility>

namespace compwolf
{
	/** An event which calls itself right before being destructed.
	 * This also calls itself when move- or copy-assigned.
	 */
	template <typename ParameterType = void>
	class destruct_event : public event<ParameterType>
	{
		ParameterType* _data;
	public:
		/** Returns the object that is passed to subscribers when destructed. */
		auto data() noexcept -> ParameterType& { return *_data; }
		/** Sets what object that is passed to subscribers when destructed.
		 * @return the data passed to this.
		 */
		auto set_data(ParameterType& data) noexcept -> ParameterType& { return *(_data = &data); }

	public: // constructors
		/** Constructs an invalid [[destruct_event]].
		 * Invoking this event is undefined behaviour.
		 * @overload
		 */
		destruct_event() noexcept = default;
		destruct_event(const destruct_event<ParameterType>& other) : event<>(other) {}
		auto operator=(const destruct_event<ParameterType>& other) -> destruct_event&
		{
			invoke(data());
			event<ParameterType>::operator=(other);
		}
		destruct_event(destruct_event<ParameterType>&& other) : event<>(std::move(other)) {}
		auto operator=(destruct_event<ParameterType>&& other) -> destruct_event&
		{
			invoke(data());
			event<ParameterType>::operator=(std::move(other));
		}
		~destruct_event() noexcept { invoke(data()); }

		destruct_event(ParameterType& data) : _data(data) {}
	};

	/** An event which calls itself right before being destructed.
	 * This also calls itself when move- or copy-assigned.
	 * @hidden
	 */
	template <>
	class destruct_event<void> : public event<>
	{
	public: // constructors
		destruct_event() noexcept = default;
		destruct_event(const destruct_event<>& other) : event<>(other) {}
		auto operator=(const destruct_event<>& other) -> destruct_event&
		{
			invoke();
			event<>::operator=(other);
			return *this;
		}
		destruct_event(destruct_event<>&& other) : event<>(std::move(other)) {}
		auto operator=(destruct_event<>&& other) -> destruct_event&
		{
			invoke();
			event<>::operator=(std::move(other));
			return *this;
		}
		~destruct_event() noexcept { invoke(); }
	};
}

#endif // ! COMPWOLF_DESTRUCT_EVENT
