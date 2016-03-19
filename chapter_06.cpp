#include "catch.hpp"

#include <type_traits>

#include <boost/mpl/if.hpp>
#include <boost/mpl/less_equal.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/string.hpp>
#include <boost/mpl/copy.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/sizeof.hpp>

namespace mpl = boost::mpl;


////////////////////////////////////////////////////////////////////////////////
template <typename lhs, typename rhs>
struct min_type
        : mpl::if_<
                mpl::less_equal<mpl::sizeof_<lhs>, mpl::sizeof_<rhs>>,
                lhs,
                rhs
            >
{ };

template <typename Sequence>
struct smallest
        : mpl::copy<
                Sequence,
                mpl::inserter<
                        typename mpl::front<Sequence>::type,
                        min_type<mpl::placeholders::_, mpl::placeholders::_>
                >
          >
{ };


template <typename Sequence>
struct smallest_fold
        : mpl::fold<
                Sequence,
                typename mpl::front<Sequence>::type,
                min_type<mpl::placeholders::_, mpl::placeholders::_>
            >
{ };


TEST_CASE("6-0", "[tmp]")
{
    using std::is_same;

    static_assert(
            is_same<
                    char,
                    smallest<mpl::vector<int[2], char, double &>>::type
            >(),
            ""
    );

    static_assert(
            is_same<
                    char,
                    smallest_fold<mpl::vector<int[2], char, double &>>::type
            >(),
            ""
    );
}


////////////////////////////////////////////////////////////////////////////////
TEST_CASE("multi-character sequence", "[tmp]")
{
    constexpr auto cs = 'abcd';
    static_assert(std::is_same<int const, decltype(cs)>(), "");
    static_assert(4 == sizeof(cs), "");
    static_assert('d' == ((cs >> 0)  & 0xFF), "");
    static_assert('c' == ((cs >> 8)  & 0xFF), "");
    static_assert('b' == ((cs >> 16) & 0xFF), "");
    static_assert('a' == ((cs >> 24) & 0xFF), "");

    /*
    // NOTE: compile warning on clang: character constant too long for its type
    constexpr auto cs1 = 'abcde';
    static_assert(std::is_same<int const, decltype(cs1)>(), "");
    static_assert(4 == sizeof(cs1), "");
    static_assert('e' == ((cs1 >> 0)  & 0xFF), "");
    static_assert('d' == ((cs1 >> 8)  & 0xFF), "");
    static_assert('c' == ((cs1 >> 16) & 0xFF), "");
    static_assert('b' == ((cs1 >> 24) & 0xFF), "");

    // NOTE: 'a' was truncated. compile error on clang.
    //static_assert('a' != ((cs1 >> 32) & 0xFF), "");
     */
}

TEST_CASE("6-1", "[tmp]")
{
}

