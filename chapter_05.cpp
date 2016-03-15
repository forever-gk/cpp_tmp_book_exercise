#include "catch.hpp"

#include <boost/mpl/iterator_tags.hpp>
#include <boost/mpl/iterator_range.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/mpl/next.hpp>
#include <boost/mpl/prior.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/plus.hpp>
#include <boost/mpl/minus.hpp>
#include <boost/mpl/multiplies.hpp>
#include <boost/mpl/divides.hpp>
#include <boost/mpl/advance.hpp>
#include <boost/mpl/distance.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/clear.hpp>
#include <boost/mpl/push_front.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/equal.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/transform_view.hpp>
#include <boost/mpl/joint_view.hpp>


namespace mpl = boost::mpl;


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
struct tiny_size : mpl::int_<3>
{ };

template <typename T0, typename T1>
struct tiny_size<T0, T1, none> : mpl::int_<2>
{ };

template <typename T0>
struct tiny_size<T0, none, none> : mpl::int_<1>
{ };

template <>
struct tiny_size<none, none, none> : mpl::int_<0>
{ };


template <typename Tiny, typename T, int N>
struct tiny_push_back;

template <typename Tiny, typename T>
struct tiny_push_back<Tiny, T, 0>
        : tiny<T, none, none>
{ };

template <typename Tiny, typename T>
struct tiny_push_back<Tiny, T, 1>
        : tiny<typename Tiny::t0, T, none>
{ };

template <typename Tiny, typename T>
struct tiny_push_back<Tiny, T, 2>
        : tiny<typename Tiny::t0, typename Tiny::t1, T>
{ };


template <typename Tiny, typename Pos>
struct tiny_iterator
{
    using category = mpl::random_access_iterator_tag;
};


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
                    : tiny<T, typename Tiny::t0, typename Tiny::t1>
            { };
        };

        template <>
        struct push_back_impl<tiny_tag>
        {
            template <typename Tiny, typename T>
            struct apply
                    : tiny_push_back<Tiny, T, size<Tiny>::value>
            { };
        };
}} // namespace boost::mpl


TEST_CASE("5-0", "[tmp]")
{
    using std::is_same;

    using tiny_t = tiny<char, int, double>;

    using begin_t = typename mpl::begin<tiny_t>::type;
    using end_t = typename mpl::end<tiny_t>::type;

    static_assert(is_same<int, typename mpl::deref<typename mpl::next<begin_t>::type>::type>(), "");
    static_assert(is_same<double, typename mpl::deref<typename mpl::prior<end_t>::type>::type>(), "");

    static_assert(mpl::distance<begin_t, end_t>() == 3, "");
    static_assert(is_same<char, typename mpl::deref<begin_t>::type>(), "");
    static_assert(is_same<int, typename mpl::deref<typename mpl::advance<begin_t, mpl::int_<1>>::type>::type>(), "");
    static_assert(is_same<double, typename mpl::deref<typename mpl::advance<begin_t, mpl::int_<2>>::type>::type>(), "");

    using tiny_1_t = tiny<>;
    static_assert(mpl::size<tiny_1_t>() == 0, "");

    using tiny_2_t = typename mpl::push_back<tiny_1_t, double>::type;
    static_assert(mpl::size<tiny_2_t>() == 1, "");

    using tiny_3_t = typename mpl::push_front<tiny_2_t, char>::type;
    static_assert(mpl::size<tiny_3_t>() == 2, "");

    using tiny_4_t = typename mpl::push_back<tiny_3_t, int>::type;
    static_assert(mpl::size<tiny_4_t>() == 3, "");

    static_assert(is_same<char, typename mpl::at<tiny_4_t, mpl::int_<0>>::type>(), "");
    static_assert(is_same<double, typename mpl::at<tiny_4_t, mpl::int_<1>>::type>(), "");
    static_assert(is_same<int, typename mpl::at<tiny_4_t, mpl::int_<2>>::type>(), "");

    static_assert(is_same<tiny<>, typename mpl::clear<tiny_4_t>::type>(), "");
}

////////////////////////////////////////////////////////////////////////////////

template <typename S>
struct middle_pos
        : mpl::advance<
                typename mpl::begin<S>::type,
                mpl::divides<mpl::size<S>, mpl::int_<2>>
            >
{ };

template <typename S>
struct double_first_half
        : mpl::joint_view<
                mpl::transform_view<
                        mpl::iterator_range<
                                typename mpl::begin<S>::type,
                                typename middle_pos<S>::type
                        >,
                        mpl::multiplies<mpl::placeholders::_, mpl::int_<2>>
                >,
                mpl::iterator_range<
                        typename middle_pos<S>::type,
                        typename mpl::end<S>::type
                >
            >
{ };


TEST_CASE("5-1", "[tmp]")
{
    using std::is_same;

    using vector_c_t = mpl::vector_c<int, 1, 2, 3, 4>;

    static_assert(2 == mpl::divides<mpl::size<vector_c_t>, mpl::int_<2>>(), "");

    static_assert(
            mpl::equal_to<
                    typename mpl::at<
                                    vector_c_t,
                                    mpl::divides<mpl::size<vector_c_t>, mpl::int_<2>>
                                >::type,
                    mpl::int_<3>
            >(),
            ""
    );

    static_assert(
            mpl::equal_to<
                    typename mpl::deref<
                                    typename mpl::advance<
                                                    typename mpl::begin<vector_c_t>::type,
                                                    mpl::divides<mpl::size<vector_c_t>, mpl::int_<2>>
                                                >::type
                                >::type,
                    mpl::int_<3>
            >(),
            ""
    );

    static_assert(
            2 == mpl::size<
                    typename mpl::iterator_range<
                                    typename mpl::begin<vector_c_t>::type,
                                    typename mpl::advance<
                                                    typename mpl::begin<vector_c_t>::type,
                                                    mpl::divides<mpl::size<vector_c_t>, mpl::int_<2>>
                                                >::type
                                >::type
                    >(),
            ""
    );

    static_assert(
            mpl::equal<
                    mpl::vector_c<int, 2, 4, 6, 8>,
                    typename mpl::transform<
                                    vector_c_t,
                                    mpl::multiplies<mpl::placeholders::_, mpl::int_<2>>
                                >::type
            >(),
            ""
    );

    using double_first_half_t = typename double_first_half<mpl::vector_c<int, 1, 2, 3, 4>>::type;
    static_assert(mpl::equal_to<mpl::int_<4>, mpl::size<double_first_half_t>>(), "");
    static_assert(mpl::equal_to<mpl::int_<2>, typename mpl::at_c<double_first_half_t, 0>::type>(), "");
    static_assert(mpl::equal_to<mpl::int_<4>, typename mpl::at_c<double_first_half_t, 1>::type>(), "");
    static_assert(mpl::equal_to<mpl::int_<3>, typename mpl::at_c<double_first_half_t, 2>::type>(), "");
    static_assert(mpl::equal_to<mpl::int_<4>, typename mpl::at_c<double_first_half_t, 3>::type>(), "");

    static_assert(
            mpl::equal<
                    typename double_first_half<mpl::vector_c<int, 1, 2, 3, 4>>::type,
                    mpl::vector_c<int, 2, 4, 3, 4>
            >(),
            ""
    );
}

