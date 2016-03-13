#include "catch.hpp"

#include <type_traits>
#include <algorithm>

#include <boost/mpl/or.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/mpl/next.hpp>
#include <boost/mpl/not_equal_to.hpp>
#include <boost/mpl/greater.hpp>
#include <boost/mpl/minus.hpp>
#include <boost/mpl/plus.hpp>
#include <boost/mpl/multiplies.hpp>
#include <boost/mpl/int.hpp>

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

////////////////////////////////////////////////////////////////////////////////
template <typename N, typename Predicate>
struct next_if
        : mpl::if_<
                typename mpl::apply<Predicate, N>::type,
                typename mpl::next<N>::type,
                N
            >
{ };

template <typename N1, typename N2>
struct formula
        : mpl::if_<
                mpl::not_equal_to<N1, N2>,
                typename mpl::if_<
                                mpl::greater<N1, N2>,
                                typename mpl::minus<N1, N2>::type,
                                N1
                            >::type,
                typename mpl::plus<
                                N1,
                                typename mpl::multiplies<N1, mpl::int_<2>>
                            >::type
            >::type
{ };


TEST_CASE("4-3", "[tmp]")
{

}


////////////////////////////////////////////////////////////////////////////////
template <typename T>
struct is_data_member_pointer;

template <typename T>
struct is_pointer_to_function;

template <typename T>
struct is_reference_to_function_pointer;

template <typename T>
struct is_reference_to_non_const;


TEST_CASE("4-4", "[tmp]")
{

}


////////////////////////////////////////////////////////////////////////////////
template <typename Container, typename Value>
typename Container::iterator
container_find(Container & c, Value const& v)
{
    return std::find(c.begin(), c.end(), v);
}


TEST_CASE("4-5", "[tmp]")
{

}

