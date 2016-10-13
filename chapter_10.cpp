#include "catch.hpp"

#define BOOST_PP_VARIADICS 1
#include <boost/preprocessor/seq/size.hpp>


#define SEQ_size(prefix, seq) BOOST_PP_SEQ_SIZE(seq)

TEST_CASE("sequence size", "[Boost.Preprocessor]")
{
    auto size = SEQ_size("seq", (a)(b)(c));
    REQUIRE(3 == size);

    auto size1 = SEQ_size(
                    "seq",
                    (a)(b)(c)
                 );
    REQUIRE(3 == size1);

    auto size2 = SEQ_size(
                    "seq",
                    (a)
                    (b)
                    (c)
                 );
    REQUIRE(3 == size2);

    // NOTE: these are expected errors.
    /*
    auto size3 = SEQ_size(
                    "seq",
                    (a, b, c)
                 );
    REQUIRE(1 == size3);

    auto size4 = SEQ_size(
                    "seq",
                    (a)
                    (b, c)
                 );
    REQUIRE(2 == size4);
    */

    auto size5 = SEQ_size(
                    "seq",
                    ((a, b, c))
                 );
    REQUIRE(1 == size5);

    auto size6 = SEQ_size(
                    "seq",
                    ((a, b, c))
                    ((d, e))
                 );
    REQUIRE(2 == size6);
}

#undef SEQ_size


TEST_CASE("10-3", "[tmp]")
{

}
