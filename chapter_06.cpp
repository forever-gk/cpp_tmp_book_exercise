#include "catch.hpp"

#include <type_traits>

#include <boost/mpl/if.hpp>
#include <boost/mpl/comparison.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/copy.hpp>
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
}

