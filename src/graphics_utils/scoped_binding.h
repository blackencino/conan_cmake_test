#pragma once

#include <tuple>
#include <utility>

namespace graphics_utils {

namespace detail {

template <typename Bindable, typename Arg_tuple, std::size_t... I>
void apply_bind_impl(Bindable& bindable,
                     Arg_tuple const& arg_tuple,
                     std::index_sequence<I...>) {
    bind(bindable, std::get<I>(arg_tuple)...);
}

template <typename Bindable, typename Arg_tuple>
void apply_bind(Bindable& bindable, Arg_tuple const& arg_tuple) {
    apply_bind_impl(bindable,
                    arg_tuple,
                    std::make_index_sequence<
                      std::tuple_size<std::decay_t<Arg_tuple>>::value>{});
}

template <typename Bindable, typename Arg_tuple, std::size_t... I>
void apply_unbind_impl(Bindable& bindable,
                       Arg_tuple const& arg_tuple,
                       std::index_sequence<I...>) {
    unbind(bindable, std::get<I>(arg_tuple)...);
}

template <typename Bindable, typename Arg_tuple>
void apply_unbind(Bindable& bindable, Arg_tuple const& arg_tuple) {
    apply_unbind_impl(bindable,
                      arg_tuple,
                      std::make_index_sequence<
                        std::tuple_size<std::decay_t<Arg_tuple>>::value>{});
}

}  // namespace detail

template <typename Bindable, typename... Arg_types>
class Scoped_binding {
    Scoped_binding() = delete;
    Scoped_binding(Scoped_binding const&) = delete;
    Scoped_binding& operator=(Scoped_binding const&) = delete;

public:
    Scoped_binding(Bindable& bindable, std::tuple<Arg_types...> arg_tuple)
      : m_bindable(&bindable)
      , m_arg_tuple(arg_tuple) {
        apply_bind(*m_bindable, m_arg_tuple);
    }
    Scoped_binding(Scoped_binding&& other)
      : m_bindable(other.m_bindable)
      , m_arg_tuple(other.m_arg_tuple) {
        // Make sure other is marked as moved so we don't double-unbind
        other.m_bindable = nullptr;
    }
    Scoped_binding& operator=(Scoped_binding&& other) {
        if (m_bindable) { 
            apply_unbind(*m_bindable, m_arg_tuple);
            m_bindable = nullptr;
        }
        m_bindable = other.m_bindable;
        m_arg_tuple = other.m_arg_tuple;
        // Make sure other is marked as moved so we don't double-unbind
        other.m_bindable = nullptr;
        return *this;
    }

    ~Scoped_binding() {
        if (m_bindable) {
            apply_unbind(*m_bindable, m_arg_tuple);
            m_bindable = nullptr;
        }
    }

private:
    Bindable* m_bindable = nullptr;
    std::tuple<Arg_types...> m_arg_tuple;
};

template <typename Bindable, typename Arg>
class Scoped_binding<Bindable, Arg> {
    Scoped_binding() = delete;
    Scoped_binding(const Scoped_binding&) = delete;
    Scoped_binding& operator=(const Scoped_binding&) = delete;

public:
    Scoped_binding(Bindable& bindable, Arg arg)
      : m_bindable(&bindable)
      , m_arg(arg) {
        bind(*m_bindable, m_arg);
    }
    Scoped_binding(Scoped_binding&& other)
      : m_bindable(other.m_bindable)
      , m_arg(other.m_arg) {
        // Make sure other is marked as moved so we don't double-unbind
        other.m_bindable = nullptr;
    }

    Scoped_binding& operator=(Scoped_binding&& other) {
        if (m_bindable) { 
            unbind(*m_bindable, m_arg);
            m_bindable = nullptr;
        }
        m_bindable = other.m_bindable;
        m_arg = other.m_arg;
        // Make sure other is marked as moved so we don't double-unbind
        other.m_bindable = nullptr;
        return *this;
    }

    ~Scoped_binding() {
        if (m_bindable) {
            unbind(*m_bindable, m_arg);
            m_bindable = nullptr;
        }
    }

private:
    Bindable* m_bindable = nullptr;
    Arg m_arg;
};

template <typename Bindable>
class Scoped_binding<Bindable> {
    Scoped_binding() = delete;
    Scoped_binding(const Scoped_binding&) = delete;
    Scoped_binding& operator=(const Scoped_binding&) = delete;

public:
    explicit Scoped_binding(Bindable& bindable)
      : m_bindable(&bindable) {
        bind(*m_bindable);
    }
    Scoped_binding(Scoped_binding&& other)
      : m_bindable(other.m_bindable) {
        // Make sure other is marked as moved so we don't double-unbind
        other.m_bindable = nullptr;
    }
    Scoped_binding& operator=(Scoped_binding&& other) {
        if (m_bindable) { 
            unbind(*m_bindable);
            m_bindable = nullptr;
        }
        m_bindable = other.m_bindable;
        // Make sure other is marked as moved so we don't double-unbind
        other.m_bindable = nullptr;
        return *this;
    }

    ~Scoped_binding() {
        if (m_bindable) {
            unbind(*m_bindable);
            m_bindable = nullptr;
        }
    }

private:
    Bindable* m_bindable = nullptr;
};

template <typename Bindable>
Scoped_binding<Bindable> make_scoped_binding(Bindable& bindable) {
    return Scoped_binding<Bindable>{bindable};
}

template <typename Bindable, typename Arg>
Scoped_binding<Bindable, Arg> make_scoped_binding(Bindable& bindable, Arg arg) {
    return Scoped_binding<Bindable, Arg>{bindable, arg};
}

template <typename Bindable, typename... Arg_types>
Scoped_binding<Bindable, Arg_types...> make_scoped_binding(
  Bindable& bindable, std::tuple<Arg_types...> arg_tuple) {
    return Scoped_binding<Bindable, Arg_types...>{bindable, arg_tuple};
}

}  // namespace graphics_utils
