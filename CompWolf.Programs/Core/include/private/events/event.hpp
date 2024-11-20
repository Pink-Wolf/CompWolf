#ifndef COMPWOLF_EVENT
#define COMPWOLF_EVENT

#include <utility>
#include <vector>
#include <functional>
#include <type_traits>

namespace compwolf
{
	/** An implementation of the observer pattern, with functors as the observers and this as the subject.
	 * Specifically, allows functions and functors to "subscribe" to this; this can then be invoked to invoke all subscribers.
	 * @typeparam ParameterType The type of object to pass to subscribers; subscribers must have this as their only parameter.
	 * If void, passes nothing to subscribers; subscribers must then have no parameters.
	 * Consider making this a reference type to minimize data copying.
	 */
	template <typename ParameterType = void>
	class event
	{
	public:
		/** The parameter type of the event's subscribers. */
		using parameter_type = ParameterType;
		/** The type of functor that can subscribe to the event. */
		using value_type = std::function<void(parameter_type)>;
		/** The type of object used to represent a subscriber. */
		using key_type = std::vector<value_type>::size_type;

	private:
		std::vector<value_type> _observers;

	public: // modifiers
		/** Subscribes the given functor to the event.
		 * @returns a key to identify the functor, if you later wishes to unsubscribe it from the event.
		 */
		key_type subscribe(value_type observer) noexcept
		{
			auto key = _observers.size();
			_observers.emplace_back(std::move(observer));
			return key;
		}
		/** Unsubscribes the functor represented by the given key from the event.
		 * The key was generated when the functor subscribed to the event.
		 */
		void unsubscribe(key_type observer_key) noexcept
		{
			_observers[observer_key] = value_type();
		}

	public: // operators
		/** Invokes all subscribed functions/functors, passing along the given value to them. */
		void invoke(parameter_type parameter) const
		{
			for (auto& observer : std::vector<value_type>(_observers)) // copy vector so it cannot be modified during loop
			{
				if (observer == nullptr) continue;
				observer(static_cast<std::add_rvalue_reference_t<parameter_type>>(parameter)); // turn non-reference to xvalue
			}
		}
		/** Invokes all subscribed functions/functors, passing along the given value to them. */
		void operator()(parameter_type parameter) const
		{
			invoke(static_cast<std::add_rvalue_reference_t<parameter_type>>(parameter)); // turn non-reference to xvalue
		}
	};

	/** An implementation of the observer pattern, with functors as the observers and this as the subject.
	 * Specifically, allows functions and functors to "subscribe" to this; this can then be invoked to invoke all subscribers.
	 * @hidden
	 */
	template <>
	class event<void>
	{
	public:
		/** Void, to signal that this event passes no data long to its subscribers when invoked. */
		using parameter_type = void;
		/** The type of functor that can subscribe to the event. */
		using value_type = std::function<void(parameter_type)>;
		/** The type of object used to represent a subscriber. */
		using key_type = std::vector<value_type>::size_type;

	private:
		std::vector<value_type> _observers;

	public: // modifiers
		/** Subscribes the given functor to the event.
		 * @returns a key to identify the functor, if you later wishes to unsubscribe it from the event.
		 */
		key_type subscribe(value_type observer) noexcept
		{
			auto key = _observers.size();
			_observers.emplace_back(std::move(observer));
			return key;
		}
		/** Unsubscribes the functor represented by the given key from the event.
		 * The key was generated when the functor subscribed to the event.
		 */
		void unsubscribe(key_type observer_key) noexcept
		{
			_observers[observer_key] = value_type();
		}

	public: // operators
		/** Invokes all subscribed functions/functors, passing along the given value to them. */
		void invoke() const
		{
			for (auto& observer : std::vector<value_type>(_observers)) // copy vector so it cannot be modified during loop
			{
				if (observer == nullptr) continue;
				observer();
			}
		}
		/** Invokes all subscribed functions/functors, passing along the given value to them. */
		void operator()() const
		{
			invoke();
		}
	};
}

#endif // ! COMPWOLF_EVENT
