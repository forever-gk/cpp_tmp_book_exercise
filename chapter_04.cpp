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


