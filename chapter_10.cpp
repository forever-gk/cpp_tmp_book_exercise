#include "catch.hpp"

#define BOOST_PP_VARIADICS 1
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>


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


// refer to the following link to implement BOOST_FUSION_ADAPT_STRUCT-like macros.
// http://stackoverflow.com/questions/24309309/how-to-use-boost-preprocessor-to-generate-accessors/24312646#24312646

#define CREATE_MY_MACRO_PLACEHOLDER_FILLER_0(...)  \
            ((__VA_ARGS__)) CREATE_MY_MACRO_PLACEHOLDER_FILLER_1
#define CREATE_MY_MACRO_PLACEHOLDER_FILLER_1(...)  \
            ((__VA_ARGS__)) CREATE_MY_MACRO_PLACEHOLDER_FILLER_0
#define CREATE_MY_MACRO_PLACEHOLDER_FILLER_0_END
#define CREATE_MY_MACRO_PLACEHOLDER_FILLER_1_END

#define SEQ_of_tuple(prefix, seqOfTupleExpr)   \
            BOOST_PP_CAT(CREATE_MY_MACRO_PLACEHOLDER_FILLER_0 seqOfTupleExpr, _END)


TEST_CASE("tuple sequence", "[Boost.Preprocessor]")
{
    auto size = BOOST_PP_SEQ_SIZE(
                    SEQ_of_tuple(
                        "seq",
                        (a, b, c)
                        (d, e, f)
                        (name, std::string, "")
                    )
                );
    REQUIRE(3 == size);

    auto size1 = BOOST_PP_SEQ_SIZE(
                    SEQ_of_tuple(
                        "seq",
                        (a)
                        (d, e)
                        (name, std::string, "")
                    )
                 );
    REQUIRE(3 == size1);
}

#undef SEQ_of_tuple

#undef CREATE_MY_MACRO_PLACEHOLDER_FILLER_1_END
#undef CREATE_MY_MACRO_PLACEHOLDER_FILLER_0_END
#undef CREATE_MY_MACRO_PLACEHOLDER_FILLER_1
#undef CREATE_MY_MACRO_PLACEHOLDER_FILLER_0


TEST_CASE("10-3", "[tmp]")
{

}
