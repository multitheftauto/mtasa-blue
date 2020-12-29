#pragma once

#include <functional>
#include <algorithm>
#include <vector>
#include <type_traits>
#include <utility>
#include <shared_mutex>

namespace ksignals
{
	template<int I> struct placeholder {};
} // ksignals::

namespace std {
	template<int I>
	struct is_placeholder< ::ksignals::placeholder<I>> : std::integral_constant<int, I> {};
} // std::

namespace ksignals {
	namespace detail {
		template<std::size_t... Is, class F, class... Args>
		auto easy_bind(std::integer_sequence<size_t, Is...>, F const& f, Args&&... args)
		{
			return std::bind(f, std::forward<Args>(args)..., placeholder<Is + 1>{}...);
		}
	} // detal::

	template<class R, class... FArgs, class... Args>
	auto easy_bind(std::function<R(FArgs...)> const& f, Args&&... args)
	{
		return detail::easy_bind(std::make_integer_sequence<size_t, sizeof...(FArgs)-sizeof...(Args)>{}, f, std::forward<Args>(args)...);
	}

	template<class R, typename T, class... FArgs, class... Args>
	auto easy_bind(R(T::*f)(FArgs...), T* meh, Args&&... args)
	{
		return detail::easy_bind(std::make_integer_sequence<size_t, sizeof...(FArgs)-sizeof...(Args)>{}, f, meh, std::forward<Args>(args)...);
	}

	template<class R, class... FArgs, class... Args>
	auto easy_bind(R(*f)(FArgs...), Args&&... args)
	{
		return detail::easy_bind(std::make_integer_sequence<size_t, sizeof...(FArgs)-sizeof...(Args)>{}, f, std::forward<Args>(args)...);
	}

	// We have to abstract member functions and non-member functions
	// Or do we not want non member functions
	class EventDelegateBase
	{
	public:
        virtual ~EventDelegateBase() = default;
	};

	template <typename _Rx = void, typename... Args>
	class EventBase;

	template<typename _Rx = void, typename... Args>
	class EventDelegate : public EventDelegateBase
	{
	private:
		std::vector<EventBase<_Rx, Args...>*> v;

	public:
		EventDelegate(EventBase<_Rx, Args...> &)
		{
			//e.connect(*this);
		}

		virtual ~EventDelegate()
		{
			for (auto e : v) {
				e->disconnect(*this);
			}

			v.clear();
		}

		void add(EventBase<_Rx, Args...> *e)
		{
			v.push_back(e);
		}

		void remove(EventBase<_Rx, Args...> *e)
		{
			if (v.empty()) {
				return;
			}
			v.erase(std::remove(std::begin(v), std::end(v), e), std::end(v));
		}


		virtual _Rx invoke(Args &&...) { throw std::runtime_error("Meow"); }
	};

	template<typename T, typename _Rx = void, typename... Args>
	class EventDelegateMemberFunction : public EventDelegate<_Rx, Args...>
	{
	private:
		T * t;
		_Rx(T::*f)(Args...);
	public:
		EventDelegateMemberFunction(EventBase<_Rx, Args...> &e, T *t, _Rx(T::*f)(Args...))
			: EventDelegate<_Rx, Args...>(e)
		{
			this->t = t;
			this->f = f;
		}

		virtual _Rx invoke(Args&&...args) override
		{
			if (t) {
				return (t->*f)(std::forward<Args>(args)...);
			}
			throw std::runtime_error("Something has gone wrong!");
		}
	};

	template<typename _Rx = void, typename... Args>
	class EventDelegateFunctionObject : public EventDelegate<_Rx, Args...>
	{
	private:
		std::function<_Rx(Args...)> fn;

	public:
		EventDelegateFunctionObject(EventBase<_Rx, Args...> &e, std::function<_Rx(Args...)> f)
			: EventDelegate<_Rx, Args...>(e),
			fn(f)
		{ }

		virtual _Rx invoke(Args&&... args) override
		{
			return fn(std::forward<Args>(args)...);
		}
	};


	template<typename T, typename _Rx = void, typename... Args>
	class EventDelegateFunctionPointer : public EventDelegate<_Rx, Args...>
	{
		T * _function = nullptr;
	public:
		EventDelegateFunctionPointer(EventBase<_Rx, Args...> &e, T * function)
			: EventDelegate<_Rx, Args...>(e),
			_function(function)
		{

		}

		virtual _Rx invoke(Args &&...args) override
		{
			if (_function) {
				return (*_function)(std::forward<Args>(args)...);
			}

			throw std::runtime_error("Something has gone wrong!");
		}
	};

	template <typename _Rx, typename... Args>
	class EventBase
	{
	private:
		std::vector<EventDelegate<_Rx, Args...>*> things;
        std::vector<EventDelegate<_Rx, Args...>*> _staging_connect_events;
        std::vector<EventDelegate<_Rx, Args...>*> _staging_disconnect_events;
        std::mutex _staging_mutex;
        std::shared_mutex _mutex;

		struct event_delegate_ptr_tag { };

	public:
		enum BoolCallResult : uint8_t
		{
			NO_EVENT = 0,
			ALL_FALSE,
			ALL_TRUE,
			BOTH,
			MAX_RESULT,
		};

        EventBase() = default;
		virtual ~EventBase()
		{
			for (auto i : things) {
				i->remove(this);
			}
		}

        EventBase& operator=(EventBase &&other) {
            for (auto i : other.things) {
                i->remove(&other);
            }
            for (auto i : other.things) {
                i->add(this);
            }
            this->things = std::move(other.things);
            this->_staging_connect_events = std::move(other._staging_connect_events);
            this->_staging_disconnect_events = std::move(other._staging_disconnect_events);

            return *this;
        }

        EventBase(EventBase &&other) {
            for (auto i : other.things) {
                i->remove(&other);
            }
            for (auto i : other.things) {
                i->add(this);
            }
            this->things = std::move(other.things);
            this->_staging_connect_events = std::move(other._staging_connect_events);
            this->_staging_disconnect_events = std::move(other._staging_disconnect_events);
        }

        EventBase& operator = (const EventBase&) = delete;
        EventBase(const EventBase&) = delete;

		auto operator()(Args... args)
		{
			return invoke(std::forward<Args>(args)...);
		}

		template<typename _Rx_ = _Rx, std::enable_if_t<std::is_same<_Rx_, void>::value>* = nullptr>
		void invoke(Args... args) {
            {
                std::shared_lock<std::shared_mutex> lock(_mutex);
                // Loop through all connected things and call
                for (auto s : things) {
                    s->invoke(std::forward<Args>(args)...);
                }
            }
            if (_mutex.try_lock()) {
                std::lock_guard<decltype(_staging_mutex)> lk{ _staging_mutex };
                for (auto &ed : _staging_disconnect_events) {
                    things.erase(std::remove(std::begin(things), std::end(things), ed), std::end(things));
                }

                for (auto &ed : _staging_connect_events) {
                    things.push_back(ed);
                }

                _staging_disconnect_events.clear();
                _staging_connect_events.clear();

                _mutex.unlock();
            }
		}

		template<typename _Rx_ = _Rx, std::enable_if_t<std::is_same<_Rx_, void>::value == false && std::is_same<_Rx_, bool>::value == false>* = nullptr>
		std::vector<_Rx_> invoke(Args... args) {
			std::vector<_Rx> return_values;
            {
                std::shared_lock<std::shared_mutex> lock(_mutex);
                return_values.resize(things.size());

                // Loop through all connected things and call
                size_t i = 0;
                for (auto s : things) {
                    return_values[i] = (s->invoke(std::forward<Args>(args)...));
                    ++i;
                }
            }

            if (_mutex.try_lock()) {
                std::lock_guard<decltype(_staging_mutex)> lk{ _staging_mutex };
                for (auto &ed : _staging_disconnect_events) {
                    things.erase(std::remove(std::begin(things), std::end(things), ed), std::end(things));
                }

                for (auto &ed : _staging_connect_events) {
                    things.push_back(ed);
                }

                _staging_disconnect_events.clear();
                _staging_connect_events.clear();

                _mutex.unlock();
            }

			return std::move(return_values);
		}


		template<typename _Rx_ = _Rx, std::enable_if_t<std::is_same<_Rx_, bool>::value == true>* = nullptr>
        BoolCallResult invoke(Args... args) {

            BoolCallResult result = MAX_RESULT;

            {
                std::shared_lock<std::shared_mutex> lock(_mutex);
                if (things.empty()) {
                    return BoolCallResult::NO_EVENT;
                }



                {
                    // Loop through all connected things and call
                    for (auto s : things) {
                        const bool meow_result = (s->invoke(std::forward<Args>(args)...));
                        if (meow_result) {
                            if (result == BoolCallResult::MAX_RESULT) {
                                result = BoolCallResult::ALL_TRUE;
                            }
                            else if (result == BoolCallResult::ALL_FALSE) {
                                result = BoolCallResult::BOTH;
                            }
                        }
                        else {
                            if (result == BoolCallResult::ALL_TRUE || result == BoolCallResult::BOTH) {
                                result = BoolCallResult::BOTH;
                            }
                            else {
                                result = BoolCallResult::ALL_FALSE;
                            }
                        }
                    }
                }
            }

            if (_mutex.try_lock()) {
                std::lock_guard<decltype(_staging_mutex)> lk{ _staging_mutex };
                for (auto &ed : _staging_disconnect_events) {
                    things.erase(std::remove(std::begin(things), std::end(things), ed), std::end(things));
                }

                for (auto &ed : _staging_connect_events) {
                    things.push_back(ed);
                }

                _staging_disconnect_events.clear();
                _staging_connect_events.clear();

                _mutex.unlock();
            }

			return result;
		}

		EventDelegate<_Rx, Args...> & connect(EventDelegate<_Rx, Args...> &ed)
		{
            if (_mutex.try_lock()) {
                ed.add(this);
                things.push_back(&ed);
                _mutex.unlock();
                return ed;
            }
            else {
                std::lock_guard<decltype(_staging_mutex)> lk{ _staging_mutex };
                ed.add(this);
                _staging_connect_events.emplace_back(&ed);
                return ed;
            }
		}


		template <typename T, typename F, typename... FArgs>
		typename std::enable_if<std::is_class<T>::value, EventDelegate<_Rx, Args...> &>::type
			connect(T* t, F &&fn, FArgs&&... args)
		{
			// T is class
			return connect(easy_bind(fn, t, std::forward<FArgs>(args)...));
		}

		template <typename T, typename F, typename... FArgs>
		typename std::enable_if<!std::is_class<T>::value, EventDelegate<_Rx, Args...> &>::type
			connect(T* t, F && arg0, FArgs&&... args)
		{
			return connect(easy_bind(t, arg0, std::forward<FArgs>(args)...));
		}

		template <typename T, typename F>
		typename std::enable_if<!std::is_class<T>::value, EventDelegate<_Rx, Args...> &>::type
			connect(T* t)
		{
			event_delegate_ptr_tag tag;
			auto f = new EventDelegateFunctionPointer<T, _Rx, Args...>(*this, t);
			return connect(static_cast<EventDelegate<_Rx, Args...> *>(f), tag);
		}

		template <typename T>
		EventDelegate<_Rx, Args...> & connect(T* t, _Rx(T::*fn)(Args...))
		{
			event_delegate_ptr_tag tag;
			auto f = new EventDelegateMemberFunction<T, _Rx, Args...>(*this, t, fn);
			return connect(static_cast<EventDelegate<_Rx, Args...> *>(f), tag);
		}

		template<typename F>
		EventDelegate<_Rx, Args...> & connect(F && fn)
		{
			// As this is either std::bind result
			// Or a lambda we can just pass it to the std::function overload
			//
			return connect(std::function<_Rx(Args...)>(std::forward<F>(fn)), *this);
		}

		template <typename T>
		struct identity
		{
			typedef T type;
		};

		EventDelegate<_Rx, Args...> & connect(std::function<_Rx(Args...)> fn, EventBase<_Rx, Args...> & /* Trick to avoid recursion */)
		{
			event_delegate_ptr_tag tag;
			auto f = new EventDelegateFunctionObject<_Rx, Args...>(*this, fn);
			return connect(static_cast<EventDelegate<_Rx, Args...> *>(f), tag);
		}

		EventDelegate<_Rx, Args...> & connect(EventDelegate<_Rx, Args...> *ed, event_delegate_ptr_tag)
		{
			return connect(*ed);
		}

		void disconnect(EventDelegate<_Rx, Args...> &ed)
		{
            if (_mutex.try_lock()) {
                things.erase(std::remove(std::begin(things), std::end(things), &ed), std::end(things));
                _mutex.unlock();
            }
            else {
                std::lock_guard<decltype(_staging_mutex)> lk{ _staging_mutex };
                _staging_connect_events.erase(std::remove(std::begin(_staging_connect_events), std::end(_staging_connect_events), &ed), std::end(_staging_connect_events));
                _staging_disconnect_events.emplace_back(&ed);
            }
		}
	};


    template<class T>
    struct EventBaseImpl;

    template<class _Ret,
    class... _Types>
    struct EventBaseImpl<_Ret(_Types...)>
    {	/* determine type from argument list */
        using type = EventBase<_Ret, _Types...>;
    };

    template<typename T>
    class Event : public EventBaseImpl<T>::type
    { };

	class Delegate
	{
	private:
		struct delegate_func_object_tag { };

	public:
		~Delegate()
		{
			v.clear();
		}

		template <typename T, typename E, typename F, typename... FArgs>
		typename std::enable_if<std::is_class<T>::value, void>::type
			connect(E &&e, T* t, F &&fn, FArgs&&... args)
		{
			// T is class
			connect(std::forward<E>(e), easy_bind(fn, t, std::forward<FArgs>(args)...));
		}

		template <typename T, typename E, typename F, typename... FArgs>
		typename std::enable_if<!std::is_class<T>::value, void>::type
			connect(E &&e, T* t, F && arg0, FArgs&&... args)
		{
			connect(std::forward<E>(e), easy_bind(t, arg0, std::forward<FArgs>(args)...));
		}

		template <typename T, typename E, typename F, typename _Rx = void, typename... Args>
		typename std::enable_if<!std::is_class<T>::value, void>::type
			connect(Event<_Rx(Args...)> &e, T* t)
		{
			auto f = new EventDelegateFunctionPointer<T, Args...>(e, t);
			connect<_Rx, Args...>(e, std::move(f));
		}

		template <typename T, typename _Rx = void, typename... Args>
		void connect(Event<_Rx(Args...)> &e, T* t, _Rx (T::*fn)(Args...))
		{
			auto f = new EventDelegateMemberFunction<T, _Rx, Args...>(e, t, fn);
			connect<_Rx, Args...>(e, std::move(f));
		}

		template<typename F, typename _Rx = void, typename... Args>
		std::enable_if_t<!std::is_pointer<std::decay_t<F>>::value>
			connect(Event<_Rx(Args...)> &e, F &&fn)
		{
			// As this is either std::bind result
			// Or a lambda we can just pass it to the std::function overload
			//
			std::function<_Rx(Args...)> f = fn;
			connect<_Rx, Args...>(e, f, delegate_func_object_tag{});
		}

		template<typename _Rx = void, typename... Args>
		void connect(Event<_Rx(Args...)> &e, std::function<_Rx(Args...)> fn, delegate_func_object_tag)
		{
			auto f = new EventDelegateFunctionObject<_Rx, Args...>(e, fn);
			connect<_Rx, Args...>(e, f);
		}

		template<typename _Rx = void, typename... Args>
		void connect(Event<_Rx(Args...)> &e, EventDelegate<_Rx, Args...> *ed)
		{
			e.connect(*ed);
			v.emplace_back(ed);
		}

	private:
		std::vector<std::unique_ptr<EventDelegateBase>> v;
	};
} // ksignals::
