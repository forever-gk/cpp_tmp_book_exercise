#ifndef BOOST_PP_IS_ITERATING

#   ifndef TINY_H_INCLUDED
#       define TINY_H_INCLUDED

#       include <boost/mpl/iterator_tags.hpp>
#       include <boost/mpl/int.hpp>
#       include <boost/mpl/plus.hpp>
#       include <boost/mpl/minus.hpp>
#       include <boost/mpl/at.hpp>
#       include <boost/mpl/erase.hpp>
#       include <boost/mpl/pop_back.hpp>
#       include <boost/mpl/pop_front.hpp>
#       include <boost/mpl/push_back.hpp>
#       include <boost/mpl/insert.hpp>
#       include <boost/mpl/size.hpp>

#       include <boost/preprocessor/cat.hpp>
#       include <boost/preprocessor/iteration/iterate.hpp>
#       include <boost/preprocessor/repetition/enum.hpp>
#       include <boost/preprocessor/repetition/enum_params.hpp>
#       include <boost/preprocessor/repetition/enum_trailing.hpp>
#       include <boost/preprocessor/repetition/repeat.hpp>
#       include <boost/preprocessor/arithmetic/add.hpp>
#       include <boost/preprocessor/arithmetic/sub.hpp>
#       include <boost/preprocessor/punctuation/comma.hpp>
#       include <boost/preprocessor/punctuation/comma_if.hpp>
#       include <boost/preprocessor/comparison/not_equal.hpp>
#       include <boost/preprocessor/control/expr_if.hpp>
#       include <boost/preprocessor/seq/enum.hpp>

#       ifndef TINY_MAX_SIZE
#           define TINY_MAX_SIZE 3
#       endif // TINY_MAX_SIZE


struct none
{ };


struct tiny_tag
{ };


#   define TINY_tiny_param(z, n, data) typename BOOST_PP_CAT(T, n) = none
#   define TINY_tiny_using(z, n, data) using BOOST_PP_CAT(t, n) = BOOST_PP_CAT(T, n);

template <BOOST_PP_ENUM(TINY_MAX_SIZE, TINY_tiny_param, ~)>
struct tiny
{
    using type = tiny;
    using tag = tiny_tag;

    BOOST_PP_REPEAT(TINY_MAX_SIZE, TINY_tiny_using, ~)
};

#   undef TINY_tiny_using
#   undef TINY_tiny_param


template <typename Tiny, int N>
struct tiny_at;


template <BOOST_PP_ENUM_PARAMS(TINY_MAX_SIZE, typename T)>
struct tiny_size : boost::mpl::int_<TINY_MAX_SIZE>
{ };


template <typename Tiny, typename Pos>
struct tiny_iterator
{
    using category = boost::mpl::random_access_iterator_tag;
};


template <typename Tiny, typename Iterator, typename T>
struct tiny_insert;


template <typename Tiny, typename Iterator>
struct tiny_erase;


#       define BOOST_PP_ITERATION_LIMITS (0, TINY_MAX_SIZE - 1)
#       define BOOST_PP_FILENAME_1 "tiny.h"   // this header file
#       include BOOST_PP_ITERATE()

////////////////////////////////////////////////////////////////////////////////
namespace boost { namespace mpl {
        template <typename Tiny, typename Pos>
        struct next<tiny_iterator<Tiny, Pos>>
        {
            using type = tiny_iterator<
                                Tiny,
                                typename mpl::next<Pos>::type
                            >;
        };

        template <typename Tiny, typename Pos>
        struct prior<tiny_iterator<Tiny, Pos>>
        {
            using type = tiny_iterator<
                                Tiny,
                                typename mpl::prior<Pos>::type
                            >;
        };

        template <>
        struct at_impl<tiny_tag>
        {
            template <typename Tiny, typename N>
            struct apply : tiny_at<Tiny, N::value>
            { };
        };

        template <typename Tiny, typename Pos>
        struct deref<tiny_iterator<Tiny, Pos>>
                : at<Tiny, Pos>
        { };

        template <typename Tiny, typename Pos, typename N>
        struct advance<tiny_iterator<Tiny, Pos>, N>
        {
            using type = tiny_iterator<
                                Tiny,
                                typename mpl::plus<Pos, N>::type
                            >;
        };

        template <typename Tiny, typename Pos1, typename Pos2>
        struct distance<tiny_iterator<Tiny, Pos1>, tiny_iterator<Tiny, Pos2>>
                : mpl::minus<Pos2, Pos1>
        { };

        template <>
        struct begin_impl<tiny_tag>
        {
            template <typename Tiny>
            struct apply
            {
                using type = tiny_iterator<Tiny, int_<0>>;
            };
        };

        template <>
        struct end_impl<tiny_tag>
        {
            template <typename Tiny>
            struct apply
            {
                using type = tiny_iterator<
                                    Tiny,
                                    typename tiny_size<
                                                    typename Tiny::t0,
                                                    typename Tiny::t1,
                                                    typename Tiny::t2
                                                >::type
                                >;
            };
        };

        template <>
        struct size_impl<tiny_tag>
        {
            template <typename Tiny>
            struct apply
                    : tiny_size<
                            typename Tiny::t0,
                            typename Tiny::t1,
                            typename Tiny::t2
                        >
            { };
        };

        template <>
        struct clear_impl<tiny_tag>
        {
            template <typename Tiny>
            struct apply : tiny<>
            { };
        };

        template <>
        struct push_front_impl<tiny_tag>
        {
            template <typename Tiny, typename T>
            struct apply
            {
                static_assert(size<Tiny>() < 3, "tiny is full.");
                using type = typename tiny<T, typename Tiny::t0, typename Tiny::t1>::type;
            };
        };

        template <>
        struct push_back_impl<tiny_tag>
        {
            template <typename Tiny, typename T>
            struct apply : insert<Tiny, typename end<Tiny>::type, T>
            { };
        };

        template <>
        struct insert_impl<tiny_tag>
        {
            template <typename Tiny, typename Pos, typename T>
            struct apply
            {
                static_assert(size<Tiny>() < 3, "tiny is full.");
                using type = typename tiny_insert<Tiny, Pos, T>::type;
            };
        };

        // NOTE: not implemented the range version of erase here.
        template <>
        struct erase_impl<tiny_tag>
        {
            template <typename Tiny, typename Pos, typename>
            struct apply : tiny_erase<Tiny, Pos>
            { };
        };

        template <>
        struct pop_back_impl<tiny_tag>
        {
            template <typename Tiny>
            struct apply
            {
                static_assert(size<Tiny>() > 0, "tiny is empty.");
                using type = typename erase<Tiny, typename prior<typename end<Tiny>::type>::type>::type;
            };
        };

        template <>
        struct pop_front_impl<tiny_tag>
        {
            template <typename Tiny>
            struct apply
            {
                static_assert(size<Tiny>() > 0, "tiny is empty.");
                using type = typename erase<Tiny, typename begin<Tiny>::type>::type;
            };
        };
}} // namespace boost::mpl
////////////////////////////////////////////////////////////////////////////////

#   endif // TINY_H_INCLUDED

#else // BOOST_PP_IS_ITERATING

#   define n BOOST_PP_ITERATION()


template <typename Tiny>
struct tiny_at<Tiny, n>
{
    using type = typename Tiny::BOOST_PP_CAT(t, n);
};


#   define TINY_size(z, n, data) data

template <BOOST_PP_ENUM_PARAMS(n, typename T)>
struct tiny_size
<
    BOOST_PP_ENUM_PARAMS(n, T)
    BOOST_PP_COMMA_IF(n)
    BOOST_PP_ENUM(BOOST_PP_SUB(TINY_MAX_SIZE, n), TINY_size, none)
>
    : boost::mpl::int_<n>
{ };

#   undef TINY_size


#   define TINY_insert_before(z, n, data) typename Tiny::BOOST_PP_CAT(t, n)
#   define TINY_insert_after(z, n, data) typename Tiny::BOOST_PP_CAT(t, BOOST_PP_ADD(n, data))

template <typename Tiny, typename T>
struct tiny_insert<Tiny, tiny_iterator<Tiny, boost::mpl::int_<n>>, T>
        : tiny<
                BOOST_PP_ENUM(n, TINY_insert_before, ~)
                BOOST_PP_COMMA_IF(n)
                T
                BOOST_PP_ENUM_TRAILING(
                    BOOST_PP_SUB(TINY_MAX_SIZE, BOOST_PP_ADD(n, 1)),
                    TINY_insert_after,
                    n
                )
            >
{ };

#   undef TINY_insert_after
#   undef TINY_insert_before


#   define TINY_erase(z, n, data)                           \
                BOOST_PP_EXPR_IF(                           \
                    BOOST_PP_NOT_EQUAL(n, data),            \
                    (typename Tiny::BOOST_PP_CAT(t, n))     \
                )

template <typename Tiny>
struct tiny_erase<Tiny, tiny_iterator<Tiny, boost::mpl::int_<n>>>
        : tiny<
                BOOST_PP_SEQ_ENUM(
                    BOOST_PP_REPEAT(TINY_MAX_SIZE, TINY_erase, n)
                )
                BOOST_PP_COMMA()
                none
            >
{ };

#   undef TINY_erase


#   undef n

#endif // BOOST_PP_IS_ITERATING
