#include <type_traits>

#ifndef EnumUtils_hpp
#define EnumUtils_hpp

namespace Utils
{
using EnumFlags = size_t;

template <typename Enum>
constexpr size_t makeFlagForEnum(Enum e)
{
    return 1 << std::underlying_type_t<Enum>(e);
}

template <typename T, typename U>
constexpr bool hasFlags(T value, U flags)
{
    return (value & flags) == flags;
}
} // namespace Utils

#endif /* EnumUtils_hpp */
