#ifndef BOOST_PP_IS_ITERATING

#   ifndef TINY_H_INCLUDED
#       define TINY_H_INCLUDED

#       include <boost/mpl/int.hpp>
#       include <boost/preprocessor/iteration/iterate.hpp>


#define TINY_MAX_SIZE 3


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

template <typename Tiny>
struct tiny_at<Tiny, 0>
{
    using type = typename Tiny::t0;
};

template <typename Tiny>
struct tiny_at<Tiny, 1>
{
    using type = typename Tiny::t1;
};

template <typename Tiny>
struct tiny_at<Tiny, 2>
{
    using type = typename Tiny::t2;
};


template <typename T0, typename T1, typename T2>
struct tiny_size : boost::mpl::int_<3>
{ };

template <typename T0, typename T1>
struct tiny_size<T0, T1, none> : boost::mpl::int_<2>
{ };

template <typename T0>
struct tiny_size<T0, none, none> : boost::mpl::int_<1>
{ };

template <>
struct tiny_size<none, none, none> : boost::mpl::int_<0>
{ };


template <typename Tiny, typename Pos>
struct tiny_iterator
{
    using category = boost::mpl::random_access_iterator_tag;
};


template <typename Tiny, typename Iterator, typename T>
struct tiny_insert;


template <typename Tiny, typename T>
struct tiny_insert<Tiny, tiny_iterator<Tiny, boost::mpl::int_<0>>, T>
        : tiny<T, typename Tiny::t0, typename Tiny::t1>
{ };

template <typename Tiny, typename T>
struct tiny_insert<Tiny, tiny_iterator<Tiny, boost::mpl::int_<1>>, T>
        : tiny<typename Tiny::t0, T, typename Tiny::t1>
{ };

template <typename Tiny, typename T>
struct tiny_insert<Tiny, tiny_iterator<Tiny, boost::mpl::int_<2>>, T>
        : tiny<typename Tiny::t0, typename Tiny::t1, T>
{ };


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

#   endif // TINY_H_INCLUDED

#else // BOOST_PP_IS_ITERATING

#   define n BOOST_PP_ITERATION()

#   undef n

#endif // BOOST_PP_IS_ITERATING
