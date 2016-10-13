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
{
    auto operator = (typename T::value_type const& rhs) const
    {
        wrap<T> t;
        t.value = rhs;
        return t;
    }
};

template <typename Tag, typename T, typename U>
auto & get(wrap<Tag>, boost::mpl::inherit2<T, U> & t)
{
    return static_cast<wrap<Tag> &>(t).value;
}

template <typename Tag, typename T, typename U>
auto get(wrap<Tag>, boost::mpl::inherit2<T, U> && t)
{
    return std::move(static_cast<wrap<Tag> &>(t).value);
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
            using value_type = NAMED_PARAM_value_type(elem);            \
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
            template <typename T, typename U>                               \
            auto operator , (wrap<T> const& lhs, wrap<U> const& rhs)        \
            {                                                               \
                tuple_t t;                                                  \
                get(lhs, t) = lhs.value;                                    \
                get(rhs, t) = rhs.value;                                    \
                return t;                                                   \
            }                                                               \
            template <typename T>                                           \
            auto operator , (tuple_t const& lhs, wrap<T> const& rhs)        \
            {                                                               \
                tuple_t t(lhs);                                             \
                get(rhs, t) = rhs.value;                                    \
                return t;                                                   \
            }                                                               \
            template <typename T>                                           \
            auto operator , (tuple_t && lhs, wrap<T> const& rhs)            \
            {                                                               \
                get(rhs, lhs) = rhs.value;                                  \
                return std::move(lhs);                                      \
            }                                                               \
        }


NAMED_PARAM(
    person,
    (id,    int,               0)
    (name,  std::string,  "none")
    (age,   int,               0)
    (code,  int,            1024)
)

TEST_CASE("10-3", "[tmp]")
{
    using namespace person;

    auto f = [](auto && params) {
        REQUIRE(get(id, params) == 0);
        REQUIRE(get(name, params) == "ghjang");
        REQUIRE(get(age, params) == 0);
        REQUIRE(get(code, params) == 97);
    };

    f((code = 97, name = "ghjang"));
}

TEST_CASE("person params", "[tmp]")
{
    using namespace person;

    auto p = (id = 10);
    REQUIRE(10 == p.value);
    auto p1 = (age = 200);
    REQUIRE(200 == p1.value);

    auto p2 = (p, p1);
    REQUIRE(get(id, p2) == 10);
    REQUIRE(get(name, p2) == "none");
    REQUIRE(get(age, p2) == 200);
    REQUIRE(get(code, p2) == 1024);

    auto p3 = (p2, name = "ghjang");
    REQUIRE(get(id, p3) == 10);
    REQUIRE(get(name, p3) == "ghjang");
    REQUIRE(get(age, p3) == 200);
    REQUIRE(get(code, p3) == 1024);

    auto p4 = (code = 97, name = "ghjang");
    REQUIRE(get(id, p4) == 0);
    REQUIRE(get(name, p4) == "ghjang");
    REQUIRE(get(age, p4) == 0);
    REQUIRE(get(code, p4) == 97);       
}

TEST_CASE("person get", "[tmp]")
{
    using namespace person;
    tuple_t t;
    REQUIRE(get(id, t) == 0);
    REQUIRE(get(name, t) == "none");
    REQUIRE(get(age, t) == 0);
    REQUIRE(get(code, t) == 1024);
}


#undef NAMED_PARAM
#undef NAMED_PARAM_get_name_field
#undef NAMED_PARAM_to_tuple_seq
#undef NAMED_PARAM_declare_tag_type
#undef NAMED_PARAM_default_value
#undef NAMED_PARAM_value_type
#undef NAMED_PARAM_tag_type
#undef NAMED_PARAM_name

#undef CREATE_PLACEHOLDER_FILLER_1_END
#undef CREATE_PLACEHOLDER_FILLER_0_END
#undef CREATE_PLACEHOLDER_FILLER_1
#undef CREATE_PLACEHOLDER_FILLER_0
