#pragma once

#include "literaltypes/ConstString.hpp"
#include "literaltypes/Types.hpp"

#include <tuple>
#include <type_traits>
#include <utility>

namespace literaltypes
{

template <typename T, ConstString s>
struct ValuePair
{
    static constexpr ConstString name = s;
    using type = T;
    T value = T{};
};

template <typename T, ConstString s, typename F>
constexpr void apply(F&& f, ValuePair<T, s>& vp)
{
    std::forward<F>(f)(vp.value, vp.name);
}

namespace impl
{
    template <typename T>
    struct DependentFalse : public std::false_type
    {
    };

    template <class... ValuePairs>
    struct ValueContainer
    {
        using tuple = std::tuple<ValuePairs...>;

        tuple values;
        constexpr static auto size = std::tuple_size_v<tuple>;

        template <typename T, ConstString s>
        constexpr static decltype(auto) prepend(ValuePair<T, s>)
        {
            return ValueContainer<ValuePair<T, s>, ValuePairs...>{};
        }

        template <size_t index, typename F>
        constexpr void apply(F&& f)
        {
            literaltypes::apply(std::forward<F>(f), std::get<index>(values));
        }

        template <size_t index, typename F>
        constexpr void apply_from(F&& f)
        {
            if constexpr (index + 1 == size)
            {
                apply<index>(std::forward<F>(f));
            }
            else if constexpr (index < size)
            {
                apply<index>(f);
                apply_from<index + 1>(std::forward<F>(f));
            }
        }

        template <typename F>
        constexpr void apply_all(F&& f)
        {
            apply_from<0>(std::forward<F>(f));
        }

        template <ConstString s, size_t index>
        constexpr auto& get()
        {
            static_assert(index < size, "Invalid index.");

            auto& elem = std::get<index>(values);

            if constexpr (equal<s, elem.name>())
            {
                return elem.value;
            }
            else if constexpr (index > 0)
            {
                return get<s, index - 1>();
            }
            else
            {
                static_assert(DependentFalse<tuple>::value, "Invalid index.");
                return values;
            }
        }

        template <ConstString s>
        constexpr auto& get()
        {
            return get<s, size - 1>();
        }

        template <ConstString s>
        constexpr const auto& get() const
        {
            auto& mutable_ref = const_cast<std::add_lvalue_reference_t<
                std::remove_const_t<std::remove_reference_t<decltype(*this)>>>>(*this);
            return mutable_ref.template get<s>();
        }
    };

    template <ConstString s>
    static constexpr auto make_pair()
    {
        constexpr auto parts = split<s, ' '>();
        return ValuePair<get_type_registry_t::type<std::get<0>(parts)>, std::get<1>(parts)>{};
    }

    template <ConstString s>
    using pair_t = decltype(make_pair<s>());
} // namespace impl

template <ConstString s, size_t offset = 0>
static constexpr auto make_container()
{
    constexpr auto value_pairs = split<s, ';'>();

    if constexpr (std::tuple_size_v<decltype(value_pairs)> == offset)
    {
        return impl::ValueContainer<>{};
    }
    else
    {
        return make_container<s, offset + 1>().prepend(
            impl::make_pair<std::get<offset>(value_pairs)>());
    }
}

template <ConstString s>
using Container_t = decltype(make_container<s>());

template <ConstString s>
static constexpr decltype(auto) operator"" _c()
{
    return make_container<s>();
}

} // namespace literaltypes
