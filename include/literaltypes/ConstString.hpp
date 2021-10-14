#pragma once

#include <algorithm>
#include <array>
#include <iosfwd>
#include <limits>
#include <stdexcept>
#include <string_view>
#include <tuple>

namespace literaltypes
{

template <size_t N>
using string_array = std::array<char, N>;
template <size_t N>
using string_iterator = typename string_array<N>::const_iterator;

namespace impl
{
    template <size_t N, typename Iterator>
    static constexpr auto init(Iterator begin, Iterator end) noexcept
    {
        string_array<N> os;
        std::copy(begin, end, os.begin());
        return os;
    }
    template <size_t M>
    static constexpr auto init(const char (&c)[M]) noexcept
    {
        // Exclude null terminator
        return init<M - 1>(c, c + M - 1);
    }
} // namespace impl

template <size_t N>
struct ConstString
{
    static constexpr auto size = N;

    const string_array<N> chars = {};

    template <size_t M>
    constexpr ConstString(const char (&string)[M]) noexcept
        : chars(impl::init(string))
    {
        static_assert(M - 1 == N);
    }

    template <typename Iterator>
    constexpr ConstString(Iterator begin, Iterator end) noexcept
        : chars(impl::init<N>(begin, end))
    {
    }

    operator std::string_view() const { return std::string_view(chars.data(), N); }

    constexpr char operator[](size_t index) const noexcept { return chars[index]; }

    template <size_t size>
    constexpr auto substr(size_t pos) const
    {
        if (pos >= N || pos + size > N)
        {
            throw std::runtime_error("Invalid start position.");
        }

        return ConstString<size>(chars.begin() + pos, chars.begin() + pos + size);
    }

    template <size_t pos = 0, size_t size = std::min(std::numeric_limits<size_t>::max(), N - pos)>
    constexpr auto substr() const
    {
        static_assert(pos < N, "Invalid start position.");
        return substr<size>(pos);
    }
};
template <size_t N>
ConstString(const char (&string)[N]) -> ConstString<N - 1>;

template <ConstString s>
static constexpr decltype(auto) operator"" _cs()
{
    return s;
}

template <size_t N>
static constexpr bool operator==(const ConstString<N>& a, const ConstString<N>& b)
{
    return std::equal(a.chars.begin(), a.chars.end(), b.chars.begin());
}
template <size_t N, size_t M>
static constexpr bool operator==(const ConstString<N>&, const ConstString<M>&)
{
    return false;
}

template <ConstString a, ConstString b>
static constexpr bool equal()
{
    return a == b;
}

template <ConstString s, char c, size_t offset = 0>
static constexpr auto split()
{
    constexpr auto section_end = std::find(s.chars.begin() + offset, s.chars.end(), c);
    constexpr auto size = section_end - (s.chars.begin() + offset);
    constexpr auto new_start = offset + size + (section_end == s.chars.end() ? 0 : 1);
    if constexpr (size > 0)
    {
        return std::tuple_cat(
            std::make_tuple(s.template substr<size>(offset)), split<s, c, new_start>());
    }
    else
    {
        return std::tuple<>();
    }
}

template <size_t N>
std::ostream& operator<<(std::ostream& os, const ConstString<N>& s)
{
    os << std::string_view(s);
    return os;
}

} // namespace literaltypes
