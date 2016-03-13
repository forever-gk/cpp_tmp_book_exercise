#include "catch.hpp"

#include <type_traits>

#include <boost/mpl/or.hpp>
#include <boost/mpl/and.hpp>

namespace mpl = boost::mpl;


TEST_CASE("4-0", "[tmp]")
{
    using type_t = int;

    static_assert(mpl::or_<std::is_integral<type_t>, std::is_pointer<type_t>>(), "");
    static_assert(mpl::or_<std::is_pointer<type_t>, std::is_integral<type_t>>(), "");

    static_assert(!mpl::and_<std::is_integral<type_t>, std::is_pointer<type_t>>(), "");
    static_assert(!mpl::and_<std::is_pointer<type_t>, std::is_integral<type_t>>(), "");
}

////////////////////////////////////////////////////////////////////////////////
template <typename... Condition>
struct logical_or;

template <>
struct logical_or<> : std::false_type
{ };

template <typename x, typename... xs>
struct logical_or<x, xs...>
        : std::conditional_t<
                x::value,
                std::true_type,
                logical_or<xs...>
            >
{ };


template <typename... Condition>
struct logical_and;

template <>
struct logical_and<> : std::true_type
{ };

template <typename x, typename... xs>
struct logical_and<x, xs...>
        : std::conditional_t<
                x::value,
                logical_and<xs...>,
                std::false_type
            >
{ };


TEST_CASE("4-1", "[tmp]")
{
    using type_t = int;

    static_assert(logical_or<std::is_integral<type_t>, std::is_pointer<type_t>>(), "");
    static_assert(logical_or<std::is_pointer<type_t>, std::is_integral<type_t>>(), "");

    static_assert(!logical_and<std::is_integral<type_t>, std::is_pointer<type_t>>(), "");
    static_assert(!logical_and<std::is_pointer<type_t>, std::is_integral<type_t>>(), "");
}

TEST_CASE("4-2", "[tmp]")
{
    using type_t = int;

    static_assert(
            logical_or<
                    std::is_integral<type_t>,
                    std::is_pointer<type_t>,
                    std::is_function<type_t>
            >(),
            ""
    );
    static_assert(
            logical_or<
                    std::is_pointer<type_t>,
                    std::is_integral<type_t>,
                    std::is_function<type_t>
            >(),
            ""
    );

    static_assert(
            !logical_and<
                    std::is_integral<type_t>,
                    std::is_pointer<type_t>,
                    std::is_function<type_t>
            >(),
            ""
    );
    static_assert(
            !logical_and<
                    std::is_pointer<type_t>,
                    std::is_integral<type_t>,
                    std::is_function<type_t>
            >(),
            ""
    );
}

