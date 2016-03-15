#include "catch.hpp"

#include <type_traits>
#include <algorithm>
#include <vector>

#include <boost/mpl/or.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/mpl/next.hpp>
#include <boost/mpl/equal_to.hpp>
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


template <typename T>
struct is_even : mpl::false_
{ };

template <int i>
struct is_even<mpl::int_<i>> : mpl::bool_<i % 2 == 0>
{ };


template <typename N, typename Predicate>
struct next_if_
        : mpl::eval_if<
                typename mpl::apply<Predicate, N>::type,
                mpl::next<N>,
                N
            >
{ };

template <typename N1, typename N2>
struct formula_
        : mpl::eval_if<
                mpl::not_equal_to<N1, N2>,
                mpl::eval_if<
                        mpl::greater<N1, N2>,
                        mpl::minus<N1, N2>,
                        N1
                >,
                mpl::plus<
                        N1,
                        mpl::multiplies<N1, mpl::int_<2>>
                >
            >::type
{ };


template <typename T>
struct TD;

TEST_CASE("4-3", "[tmp]")
{
    using std::is_same;
    using mpl::placeholders::_;
    using mpl::int_;
    using mpl::equal_to;

    static_assert(is_same<int_<1>, typename next_if<int_<1>, is_even<_>>::type>(), "");
    static_assert(is_same<int_<3>, typename next_if<int_<2>, is_even<_>>::type>(), "");

    static_assert(is_same<int_<1>, typename next_if_<int_<1>, is_even<_>>::type>(), "");
    static_assert(is_same<int_<3>, typename next_if_<int_<2>, is_even<_>>::type>(), "");

    static_assert(equal_to<int_<9>, formula<int_<3>, int_<3>>>(), "");
    static_assert(equal_to<int_<3>, formula<int_<3>, int_<4>>>(), "");
    static_assert(equal_to<int_<1>, formula<int_<4>, int_<3>>>(), "");

    static_assert(equal_to<int_<9>, formula_<int_<3>, int_<3>>>(), "");
    static_assert(equal_to<int_<3>, formula_<int_<3>, int_<4>>>(), "");
    static_assert(equal_to<int_<1>, formula_<int_<4>, int_<3>>>(), "");
}


////////////////////////////////////////////////////////////////////////////////
template <typename T>
using is_data_member_pointer = std::is_member_pointer<T>;

template <typename T>
struct is_pointer_to_function
        : mpl::and_<
                std::is_pointer<T>,
                std::is_function<std::remove_pointer_t<T>>
            >
{ };

template <typename T>
struct is_reference_to_function_pointer
        : mpl::and_<
                std::is_reference<T>,
                is_pointer_to_function<std::remove_reference_t<T>>
            >
{ };

template <typename T>
struct is_reference_to_non_const
        : mpl::and_<
                std::is_reference<T>,
                mpl::not_<std::is_const<std::remove_reference_t<T>>>
            >
{ };


TEST_CASE("4-4", "[tmp]")
{
    struct S { };

    static_assert(!is_data_member_pointer<S>(), "");
    static_assert(is_data_member_pointer<int(S:: *)>(), "");

    static_assert(!is_pointer_to_function<S>(), "");
    static_assert(!is_pointer_to_function<decltype(std::atoi)>(), "");
    static_assert(is_pointer_to_function<std::decay_t<decltype(std::atoi)>>(), "");

    static_assert(!is_reference_to_function_pointer<S>(), "");
    static_assert(!is_reference_to_function_pointer<decltype(std::atoi)>(), "");
    static_assert(is_reference_to_function_pointer<std::add_lvalue_reference_t<std::decay_t<decltype(std::atoi)>>>(), "");
    static_assert(is_reference_to_function_pointer<std::add_rvalue_reference_t<std::decay_t<decltype(std::atoi)>>>(), "");

    static_assert(!is_reference_to_non_const<S>(), "");
    static_assert(!is_reference_to_non_const<S const&>(), "");
    static_assert(is_reference_to_non_const<S &>(), "");
}


template <typename T>
struct is_variadic_function : std::false_type
{ };

template <typename R, typename... Args>
struct is_variadic_function<R(Args..., ...)> : std::true_type
{ };


TEST_CASE("variadic function match", "[tmp]")
{
    static_assert(std::is_function<decltype(std::printf)>(), "");
    static_assert(std::is_function<decltype(std::atoi)>(), "");

    static_assert(is_variadic_function<decltype(std::printf)>(), "");
    static_assert(!is_variadic_function<decltype(std::atoi)>(), "");
}


////////////////////////////////////////////////////////////////////////////////
template <typename Container, typename Value>
auto container_find(Container & c, Value const& v)
{
    return std::find(c.begin(), c.end(), v);
}


TEST_CASE("4-5", "[tmp]")
{
    std::vector<int> v{ 1, 2, 3, 4, 5 };
    std::vector<int> const cv{ 1, 2, 3, 4, 5 };

    REQUIRE(v.end() != container_find(v, 3));
    REQUIRE(cv.end() != container_find(cv, 3));
}

