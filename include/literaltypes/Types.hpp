#pragma once

#include "literaltypes/ConstString.hpp"

#include <cstdint>
#include <tuple>

namespace literaltypes
{
template <typename T, ConstString s, ConstString a = s>
struct StringToType
{
    static constexpr ConstString string = s;
    static constexpr ConstString alias = a;
    using type = T;
};

template <class... Types>
struct TypeRegistry
{
    using tuple = std::tuple<Types...>;

    static constexpr tuple types = {};

    template <typename T, ConstString s, ConstString a = s>
    constexpr static decltype(auto) append()
    {
        return TypeRegistry<Types..., StringToType<T, s, a>>{};
    }

    template <ConstString s, size_t index>
    static constexpr auto get_type()
    {
        static_assert(index < std::tuple_size_v<tuple>, "Invalid index.");

        auto elem = std::get<index>(types);

        if constexpr (equal<s, elem.string>() || equal<s, elem.alias>())
        {
            return typename decltype(elem)::type();
        }
        else if constexpr (index > 0)
        {
            return get_type<s, index - 1>();
        }
        else
        {
            return void();
        }
    }

    template <ConstString s>
    static constexpr auto get_type()
    {
        return get_type<s, std::tuple_size_v<tuple> - 1>();
    }

    template <ConstString s>
    using type = decltype(get_type<s>());
};

static constexpr auto get_type_registry()
{
    return TypeRegistry<>()
        .append<double, "double"_cs, "f64"_cs>()
        .append<float, "float"_cs, "f32"_cs>()
        .append<std::int64_t, "int64"_cs, "i64"_cs>()
        .append<std::int32_t, "int"_cs, "i32"_cs>()
        .append<std::int16_t, "short"_cs, "i16"_cs>()
        .append<std::int8_t, "char"_cs, "i8"_cs>()
        .append<std::uint64_t, "uint64"_cs, "u64"_cs>()
        .append<std::uint32_t, "uint"_cs, "u32"_cs>()
        .append<std::uint16_t, "ushort"_cs, "u16"_cs>()
        .append<std::uint8_t, "uchar"_cs, "u8"_cs>();
}

using get_type_registry_t = decltype(get_type_registry());
} // namespace literaltypes
