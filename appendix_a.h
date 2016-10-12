#ifndef BOOST_PP_IS_ITERATING

#   ifndef TINY_H_INCLUDED
#       define TINY_H_INCLUDED

#       include <boost/mpl/int.hpp>
#       include <boost/mpl/iterator_tags.hpp>

#       include <boost/preprocessor/cat.hpp>
#       include <boost/preprocessor/iteration/iterate.hpp>
#       include <boost/preprocessor/repetition/enum.hpp>
#       include <boost/preprocessor/repetition/enum_params.hpp>
#       include <boost/preprocessor/repetition/enum_trailing.hpp>
#       include <boost/preprocessor/arithmetic/add.hpp>
#       include <boost/preprocessor/arithmetic/sub.hpp>
#       include <boost/preprocessor/arithmetic/dec.hpp>
#       include <boost/preprocessor/punctuation/comma_if.hpp>
#       include <boost/preprocessor/comparison/greater.hpp>

#       ifndef TINY_MAX_SIZE
#           define TINY_MAX_SIZE 3
#       endif // TINY_MAX_SIZE


struct none
{ };


struct tiny_tag
{ };


template <typename T0 = none, typename T1 = none, typename T2 = none>
struct tiny
{
    using type = tiny;
    using tag = tiny_tag;

    using t0 = T0;
    using t1 = T1;
    using t2 = T2;
};


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

template <typename Tiny>
struct tiny_erase<Tiny, tiny_iterator<Tiny, boost::mpl::int_<0>>>
        : tiny<typename Tiny::t1, typename Tiny::t2, none>
{ };

template <typename Tiny>
struct tiny_erase<Tiny, tiny_iterator<Tiny, boost::mpl::int_<1>>>
        : tiny<typename Tiny::t0, typename Tiny::t2, none>
{ };

template <typename Tiny>
struct tiny_erase<Tiny, tiny_iterator<Tiny, boost::mpl::int_<2>>>
        : tiny<typename Tiny::t0, typename Tiny::t1, none>
{ };

#       define BOOST_PP_ITERATION_LIMITS (0, TINY_MAX_SIZE - 1)
#       define BOOST_PP_FILENAME_1 "appendix_a.h"   // this header file
#       include BOOST_PP_ITERATE()

#   endif // TINY_H_INCLUDED

#else // BOOST_PP_IS_ITERATING

#   define n BOOST_PP_ITERATION()

#   define TINY_size(z, n, data) data

#   define TINY_insert_0(z, n, data) typename Tiny::BOOST_PP_CAT(t, n)
#   define TINY_insert_1(z, n, data) typename Tiny::BOOST_PP_CAT(t, BOOST_PP_ADD(n, data))


template <typename Tiny>
struct tiny_at<Tiny, n>
{
    using type = typename Tiny::BOOST_PP_CAT(t, n);
};


template <BOOST_PP_ENUM_PARAMS(n, typename T)>
struct tiny_size
<
    BOOST_PP_ENUM_PARAMS(n, T)
    BOOST_PP_COMMA_IF(n)
    BOOST_PP_ENUM(BOOST_PP_SUB(TINY_MAX_SIZE, n), TINY_size, none)
>
    : boost::mpl::int_<n>
{ };


template <typename Tiny, typename T>
struct tiny_insert<Tiny, tiny_iterator<Tiny, boost::mpl::int_<n>>, T>
        : tiny<
                BOOST_PP_ENUM(n, TINY_insert_0, ~)
                BOOST_PP_COMMA_IF(n)
                T
                BOOST_PP_ENUM_TRAILING(
                    BOOST_PP_SUB(TINY_MAX_SIZE, BOOST_PP_ADD(n, 1)),
                    TINY_insert_1,
                    n
                )
            >
{ };


#   undef TINY_insert_1
#   undef TINY_insert_0

#   undef TINY_print

#   undef n

#endif // BOOST_PP_IS_ITERATING
