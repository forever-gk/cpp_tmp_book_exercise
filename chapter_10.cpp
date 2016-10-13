#include "catch.hpp"

#define BOOST_PP_VARIADICS 1
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/transform.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>

#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/inherit.hpp>
#include <boost/mpl/inherit_linearly.hpp>


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

    auto size2 = BOOST_PP_SEQ_SIZE(
                    SEQ_of_tuple(
                        person,
                        (id,    int,               0)
                        (name,  std::string,      "")
                        (age,   int,               0)
                        (code,  int,            1024)
                    )
                 );
    REQUIRE(4 == size2);
}

#undef SEQ_of_tuple

#undef CREATE_MY_MACRO_PLACEHOLDER_FILLER_1_END
#undef CREATE_MY_MACRO_PLACEHOLDER_FILLER_0_END
#undef CREATE_MY_MACRO_PLACEHOLDER_FILLER_1
#undef CREATE_MY_MACRO_PLACEHOLDER_FILLER_0


////////////////////////////////////////////////////////////////////////////////
template <typename T>
struct wrap : T
{ };

template <typename Tag, typename T, typename U>
auto & get(wrap<Tag>, boost::mpl::inherit2<T, U> & t)
{
    return static_cast<wrap<Tag> &>(t).value;
}


#define CREATE_PLACEHOLDER_FILLER_0(...)  \
            ((__VA_ARGS__)) CREATE_PLACEHOLDER_FILLER_1
#define CREATE_PLACEHOLDER_FILLER_1(...)  \
            ((__VA_ARGS__)) CREATE_PLACEHOLDER_FILLER_0
#define CREATE_PLACEHOLDER_FILLER_0_END
#define CREATE_PLACEHOLDER_FILLER_1_END

#define NAMED_PARAM_name(elem) BOOST_PP_TUPLE_ELEM(0, elem)
#define NAMED_PARAM_tag_type(elem) BOOST_PP_CAT(BOOST_PP_TUPLE_ELEM(0, elem), _tag)
#define NAMED_PARAM_value_type(elem) BOOST_PP_TUPLE_ELEM(1, elem)
#define NAMED_PARAM_default_value(elem) BOOST_PP_TUPLE_ELEM(2, elem)

#define NAMED_PARAM_declare_tag_type(r, data, elem)                     \
        struct NAMED_PARAM_tag_type(elem) {                             \
            static NAMED_PARAM_value_type(elem) const default_value;    \
            NAMED_PARAM_value_type(elem) value { default_value };       \
        };                                                              \
        NAMED_PARAM_value_type(elem) const                              \
            NAMED_PARAM_tag_type(elem)::default_value                   \
                { NAMED_PARAM_default_value(elem) };                    \
        static wrap<NAMED_PARAM_tag_type(elem)> const                   \
                NAMED_PARAM_name(elem) { };

#define NAMED_PARAM_to_tuple_seq(param) \
        BOOST_PP_CAT(CREATE_PLACEHOLDER_FILLER_0 param, _END)

#define NAMED_PARAM_get_name_field(s, data, elem) \
        BOOST_PP_CAT(BOOST_PP_TUPLE_ELEM(0, elem), _tag)

#define NAMED_PARAM(ns, param)                                              \
        namespace ns                                                        \
        {                                                                   \
            BOOST_PP_SEQ_FOR_EACH(                                          \
                NAMED_PARAM_declare_tag_type,                               \
                ~,                                                          \
                NAMED_PARAM_to_tuple_seq(param)                             \
            )                                                               \
            using tags_t                                                    \
                = boost::mpl::vector<                                       \
                    BOOST_PP_SEQ_ENUM(                                      \
                        BOOST_PP_SEQ_TRANSFORM(                             \
                            NAMED_PARAM_get_name_field,                     \
                            ~,                                              \
                            NAMED_PARAM_to_tuple_seq(param)                 \
                        )                                                   \
                    )                                                       \
                  >;                                                        \
            using tuple_t                                                   \
                = typename boost::mpl::inherit_linearly<                    \
                                tags_t,                                     \
                                boost::mpl::inherit<                        \
                                    wrap<boost::mpl::placeholders::_2>,     \
                                    boost::mpl::placeholders::_1            \
                                >                                           \
                            >::type;                                        \
        }


NAMED_PARAM(
    person,
    (id,    int,               0)
    (name,  std::string,      "")
    (age,   int,               0)
    (code,  int,            1024)
)

TEST_CASE("10-3", "[tmp]")
{
}

#undef CREATE_PLACEHOLDER_FILLER_1_END
#undef CREATE_PLACEHOLDER_FILLER_0_END
#undef CREATE_PLACEHOLDER_FILLER_1
#undef CREATE_PLACEHOLDER_FILLER_0

#undef NAMED_PARAM
#undef NAMED_PARAM_name
#undef NAMED_PARAM_tag