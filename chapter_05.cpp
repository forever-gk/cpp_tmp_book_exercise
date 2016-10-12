#include "catch.hpp"

#include "tree.h"

// for tiny
#include "appendix_a.h"

#include <boost/mpl/arithmetic.hpp>
#include <boost/mpl/comparison.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/back.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/pop_back.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/erase.hpp>
#include <boost/mpl/equal.hpp>
#include <boost/mpl/copy.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/transform_view.hpp>


namespace mpl = boost::mpl;


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


TEST_CASE("5-0", "[tmp]")
{
    using std::is_same;

    using tiny_t = tiny<char, int, double>;

    using begin_t = mpl::begin<tiny_t>::type;
    using end_t = mpl::end<tiny_t>::type;

    static_assert(is_same<int, mpl::deref<mpl::next<begin_t>::type>::type>(), "");
    static_assert(is_same<double, mpl::deref<mpl::prior<end_t>::type>::type>(), "");

    static_assert(mpl::distance<begin_t, end_t>() == 3, "");
    static_assert(is_same<char, mpl::deref<begin_t>::type>(), "");
    static_assert(is_same<int, mpl::deref<mpl::advance_c<begin_t, 1>::type>::type>(), "");
    static_assert(is_same<double, mpl::deref<mpl::advance_c<begin_t, 2>::type>::type>(), "");

    using tiny_1_t = tiny<>;
    static_assert(mpl::size<tiny_1_t>() == 0, "");

    using tiny_2_t = mpl::push_back<tiny_1_t, double>::type;
    static_assert(mpl::size<tiny_2_t>() == 1, "");

    using tiny_3_t = mpl::push_front<tiny_2_t, char>::type;
    static_assert(mpl::size<tiny_3_t>() == 2, "");

    using tiny_4_t = mpl::push_back<tiny_3_t, int>::type;
    static_assert(mpl::size<tiny_4_t>() == 3, "");

    static_assert(is_same<char, mpl::at_c<tiny_4_t, 0>::type>(), "");
    static_assert(is_same<double, mpl::at_c<tiny_4_t, 1>::type>(), "");
    static_assert(is_same<int, mpl::at_c<tiny_4_t, 2>::type>(), "");

    static_assert(is_same<tiny<>, mpl::clear<tiny_4_t>::type>(), "");
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
                    mpl::at<
                            vector_c_t,
                            mpl::divides<mpl::size<vector_c_t>, mpl::int_<2>>
                        >::type,
                    mpl::int_<3>
            >(),
            ""
    );

    static_assert(
            mpl::equal_to<
                    mpl::deref<
                        mpl::advance<
                            mpl::begin<vector_c_t>::type,
                            mpl::divides<mpl::size<vector_c_t>, mpl::int_<2>>
                        >::type
                    >::type,
                    mpl::int_<3>
            >(),
            ""
    );

    static_assert(
            2 == mpl::size<
                        mpl::iterator_range<
                            mpl::begin<vector_c_t>::type,
                                mpl::advance<
                                        mpl::begin<vector_c_t>::type,
                                        mpl::divides<mpl::size<vector_c_t>, mpl::int_<2>>
                                >::type
                        >::type
                    >(),
            ""
    );

    static_assert(
            mpl::equal<
                    mpl::vector_c<int, 2, 4, 6, 8>,
                    mpl::transform<
                            vector_c_t,
                            mpl::multiplies<mpl::placeholders::_, mpl::int_<2>>
                    >::type
            >(),
            ""
    );

    using double_first_half_t = double_first_half<mpl::vector_c<int, 1, 2, 3, 4>>::type;
    static_assert(mpl::equal_to<mpl::int_<4>, mpl::size<double_first_half_t>>(), "");
    static_assert(mpl::equal_to<mpl::int_<2>, mpl::at_c<double_first_half_t, 0>::type>(), "");
    static_assert(mpl::equal_to<mpl::int_<4>, mpl::at_c<double_first_half_t, 1>::type>(), "");
    static_assert(mpl::equal_to<mpl::int_<3>, mpl::at_c<double_first_half_t, 2>::type>(), "");
    static_assert(mpl::equal_to<mpl::int_<4>, mpl::at_c<double_first_half_t, 3>::type>(), "");

    static_assert(
            mpl::equal<
                    double_first_half<mpl::vector_c<int, 1, 2, 3, 4>>::type,
                    mpl::vector_c<int, 2, 4, 3, 4>
            >(),
            ""
    );
}

TEST_CASE("5-2", "[tmp]")
{
    using tiny_t = tiny<char, int, double>;

    // expected compile errors
    //using tiny_1_t = mpl::push_back<tiny_t>::type;
    //using tiny_2_t = mpl::push_front<tiny_t>::type;
}

TEST_CASE("5-3", "[tmp]")
{
    using std::is_same;

    using tiny_t = tiny<>;
    using tiny_1_t = tiny<char>;
    using tiny_2_t = tiny<char, int>;
    using tiny_3_t = tiny<char, int, double>;

    static_assert(is_same<tiny<char>, mpl::insert<tiny_t, mpl::begin<tiny_t>::type, char>::type>(), "");

    static_assert(is_same<tiny<int, char>, mpl::insert<tiny_1_t, mpl::begin<tiny_1_t>::type, int>::type>(), "");
    static_assert(is_same<tiny<char, int>, mpl::insert<tiny_1_t, mpl::end<tiny_1_t>::type, int>::type>(), "");

    static_assert(is_same<tiny<int, char, int>, mpl::insert<tiny_2_t, mpl::begin<tiny_2_t>::type, int>::type>(), "");
    static_assert(is_same<tiny<char, int, int>, mpl::insert<tiny_2_t, mpl::end<tiny_2_t>::type, int>::type>(), "");
    static_assert(
            is_same<
                    tiny<char, int, int>,
                    mpl::insert<
                            tiny_2_t,
                            mpl::advance_c<mpl::begin<tiny_2_t>::type, 1>::type,
                            int
                    >::type
            >(),
            ""
    );

    // expected compile error
    //using t = mpl::insert<tiny_3_t, mpl::begin<tiny_3_t>::type, int>::type;
}

TEST_CASE("5-5", "[tmp]")
{
    using std::is_same;

    using tiny_t = tiny<char, int, double>;

    static_assert(
            is_same<
                    tiny<int, double>,
                    mpl::erase<tiny_t, mpl::begin<tiny_t>::type>::type
            >(),
            ""
    );
    static_assert(
            is_same<
                    tiny<char, int>,
                    mpl::erase<tiny_t, mpl::prior<mpl::end<tiny_t>::type>::type>::type
            >(),
            ""
    );
    static_assert(
            is_same<
                    tiny<char, double>,
                    mpl::erase<tiny_t, mpl::advance_c<mpl::begin<tiny_t>::type, 1>::type>::type
            >(),
            ""
    );

    static_assert(
            is_same<
                    tiny<char, int>,
                    mpl::pop_back<tiny_t>::type
            >(),
            ""
    );

    static_assert(
            is_same<
                    tiny<int, double>,
                    mpl::pop_front<tiny_t>::type
            >(),
            ""
    );
}


////////////////////////////////////////////////////////////////////////////////
struct dimensions_tag;

template <typename T>
struct dimensions
{
    using type = dimensions;
    using tag = dimensions_tag;
};


template <typename Dimensions, typename DimensionIndex>
struct dimensions_iterator
{
    using category = mpl::bidirectional_iterator_tag;
};


template <typename Dimensions, int DimensionIndex>
struct dimension_at;

template <typename T, int D0>
struct dimension_at<dimensions<T[D0]>, 0> : mpl::int_<D0>
{ };

template <typename T, int D1, int D0>
struct dimension_at<dimensions<T[D1][D0]>, 0> : mpl::int_<D0>
{ };

template <typename T, int D1, int D0>
struct dimension_at<dimensions<T[D1][D0]>, 1> : mpl::int_<D1>
{ };

template <typename T, int D2, int D1, int D0>
struct dimension_at<dimensions<T[D2][D1][D0]>, 0> : mpl::int_<D0>
{ };

template <typename T, int D2, int D1, int D0>
struct dimension_at<dimensions<T[D2][D1][D0]>, 1> : mpl::int_<D1>
{ };

template <typename T, int D2, int D1, int D0>
struct dimension_at<dimensions<T[D2][D1][D0]>, 2> : mpl::int_<D2>
{ };


template <typename T>
struct dimensions_size;

template <typename T>
struct dimensions_size<dimensions<T>> : mpl::int_<std::rank<T>::value>
{ };


template <typename Dimensions, typename Dimension>
struct push_back_dimension;

template <typename T, int D>
struct push_back_dimension<dimensions<T>, mpl::int_<D>> : dimensions<T[D]>
{ };

template <typename T, int D0, int D1>
struct push_back_dimension<dimensions<T[D0]>, mpl::int_<D1>> : dimensions<T[D1][D0]>
{ };

template <typename T, int D0, int D1, int D2>
struct push_back_dimension<dimensions<T[D1][D0]>, mpl::int_<D2>> : dimensions<T[D2][D1][D0]>
{ };


template <typename Dimensions>
struct pop_back_dimensions;

template <typename T, int D0>
struct pop_back_dimensions<dimensions<T[D0]>> : dimensions<T>
{ };

template <typename T, int D1, int D0>
struct pop_back_dimensions<dimensions<T[D1][D0]>> : dimensions<T[D0]>
{ };

template <typename T, int D2, int D1, int D0>
struct pop_back_dimensions<dimensions<T[D2][D1][D0]>> : dimensions<T[D1][D0]>
{ };


namespace boost { namespace mpl {
        template <>
        struct size_impl<dimensions_tag>
        {
            template <typename Dimensions>
            struct apply : dimensions_size<Dimensions>
            { };
        };

        template <>
        struct begin_impl<dimensions_tag>
        {
            template <typename Dimensions>
            struct apply
            {
                using type = dimensions_iterator<Dimensions, mpl::int_<0>>;
            };
        };

        template <>
        struct end_impl<dimensions_tag>
        {
            template <typename Dimensions>
            struct apply
            {
                using type = dimensions_iterator<Dimensions, size<Dimensions>>;
            };
        };

        template <>
        struct push_back_impl<dimensions_tag>
        {
            template <typename Dimensions, typename Dimension>
            struct apply : push_back_dimension<Dimensions, Dimension>
            { };
        };

        template <>
        struct pop_back_impl<dimensions_tag>
        {
            template <typename Dimensions>
            struct apply : pop_back_dimensions<Dimensions>
            { };
        };

        template <typename Dimensions, typename DimensionIndex>
        struct next<dimensions_iterator<Dimensions, DimensionIndex>>
        {
            using type = dimensions_iterator<
                                Dimensions,
                                typename mpl::next<DimensionIndex>::type
                            >;
        };

        template <typename Dimensions, typename DimensionIndex>
        struct prior<dimensions_iterator<Dimensions, DimensionIndex>>
        {
            using type = dimensions_iterator<
                                Dimensions,
                                typename mpl::prior<DimensionIndex>::type
                            >;
        };

        template <typename Dimensions, typename DimensionIndex>
        struct deref<dimensions_iterator<Dimensions, DimensionIndex>>
                : dimension_at<Dimensions, DimensionIndex::value>
        { };
}} // namespace boost::mpl


TEST_CASE("5-6", "[tmp]")
{
    using seq = dimensions<char [10][5][2]>;

    static_assert(mpl::size<seq>::value == 3, "");
    static_assert(mpl::at_c<seq, 0>::type::value == 2, "");
    static_assert(mpl::at_c<seq, 1>::type::value == 5, "");
    static_assert(mpl::at_c<seq, 2>::type::value == 10, "");
}


////////////////////////////////////////////////////////////////////////////////
TEST_CASE("5-7", "[tmp]")
{
    using std::is_same;

    using seq_t = dimensions<char [10][5][2]>;

    static_assert(mpl::deref<mpl::begin<seq_t>::type>::value == 2, "");
    static_assert(mpl::deref<mpl::next<mpl::begin<seq_t>::type>::type>::value == 5, "");
    static_assert(mpl::deref<mpl::prior<mpl::end<seq_t>::type>::type>::value == 10, "");

    using seq_1_t = dimensions<char>;

    static_assert(mpl::size<seq_1_t>() == 0, "");
    static_assert(
            is_same<
                    dimensions<char [2]>,
                    mpl::push_back<seq_1_t, mpl::int_<2>>::type
            >(),
            ""
    );

    using mpl::placeholders::_;
    static_assert(
            is_same<
                    dimensions<char [10][5][2]>,
                    mpl::copy<
                            mpl::vector<mpl::int_<2>, mpl::int_<5>, mpl::int_<10>>,
                            mpl::back_inserter<dimensions<char>>
                    >::type
            >(),
            ""
    );

    static_assert(is_same<dimensions<char [5][2]>, mpl::pop_back<seq_t>::type>(), "");
}

////////////////////////////////////////////////////////////////////////////////
struct fibonacci_series_tag
{ };

struct fibonacci_series
{
    using type = fibonacci_series;
    using tag = fibonacci_series_tag;
};

template <typename FibonacciSeries, typename CurrentValue, typename NextValue>
struct fibonacci_series_iterator
{
    using category = mpl::forward_iterator_tag;
};


namespace boost { namespace mpl {
        template <>
        struct begin_impl<fibonacci_series_tag>
        {
            template <typename S>
            struct apply
            {
                using type = fibonacci_series_iterator<S, mpl::int_<0>, mpl::int_<1>>;
            };
        };

        template <typename S, typename CurVal, typename NextVal>
        struct next<fibonacci_series_iterator<S, CurVal, NextVal>>
        {
            using type = fibonacci_series_iterator<
                                S,
                                NextVal,
                                typename mpl::plus<CurVal, NextVal>::type
                            >;
        };

        template <typename S, typename CurVal, typename NextVal>
        struct deref<fibonacci_series_iterator<S, CurVal, NextVal>> : CurVal
        { };
}} // namespace boost::mpl


TEST_CASE("5-8", "[tmp]")
{
    using i = mpl::advance_c<mpl::begin<fibonacci_series>::type, 6>::type;
    static_assert(mpl::deref<i>::type::value == 8, "");

    using j = mpl::advance_c<i, 4>::type;
    static_assert(mpl::deref<j>::type::value == 55, "");
}


////////////////////////////////////////////////////////////////////////////////
struct finite_fibonacci_series_tag
{ };

template <int size>
struct finite_fibonacci_series
{
    using type = finite_fibonacci_series;
    using tag = finite_fibonacci_series_tag;
};

template <typename S, typename CurVal, typename NextVal, typename IndexVal>
struct finite_fibonacci_series_iterator
{
    using category = mpl::bidirectional_iterator_tag;
};

template <typename S>
struct finite_fibonacci_series_size;

template <int size>
struct finite_fibonacci_series_size<finite_fibonacci_series<size>> : mpl::int_<size>
{ };


namespace boost { namespace mpl {
        template <>
        struct size_impl<finite_fibonacci_series_tag>
        {
            template <typename S>
            struct apply : finite_fibonacci_series_size<S>
            { };
        };

        template <>
        struct begin_impl<finite_fibonacci_series_tag>
        {
            template <typename S>
            struct apply
            {
                using type = finite_fibonacci_series_iterator<
                                    S,
                                    mpl::int_<0>,
                                    mpl::int_<1>,
                                    mpl::int_<0>
                                >;
            };
        };

        template <>
        struct end_impl<finite_fibonacci_series_tag>
        {
            template <typename S>
            struct apply
            {
                using type = finite_fibonacci_series_iterator<
                                    S,
                                    void,
                                    void,
                                    typename size<S>::type
                                >;
            };
        };

        template <typename S, typename CurVal, typename NextVal, typename IndexVal>
        struct next<finite_fibonacci_series_iterator<S, CurVal, NextVal, IndexVal>>
                : mpl::if_<
                        mpl::equal_to<size<S>, mpl::plus<IndexVal, mpl::int_<1>>>,
                        finite_fibonacci_series_iterator<
                                S,
                                void,
                                void,
                                mpl::plus<IndexVal, mpl::int_<1>>
                        >,
                        finite_fibonacci_series_iterator<
                                S,
                                NextVal,
                                mpl::plus<CurVal, NextVal>,
                                mpl::plus<IndexVal, mpl::int_<1>>
                        >
                    >
        { };

        template <typename S, typename CurVal, typename NextVal, typename IndexVal>
        struct prior<finite_fibonacci_series_iterator<S, CurVal, NextVal, IndexVal>>
        {
            using type = finite_fibonacci_series_iterator<
                                S,
                                mpl::minus<NextVal, CurVal>,
                                CurVal,
                                mpl::minus<IndexVal, mpl::int_<1>>
                            >;
        };

        template <typename S, typename IndexVal>
        struct prior<finite_fibonacci_series_iterator<S, void, void, IndexVal>>
                : mpl::advance<
                        typename mpl::begin<S>::type,
                        mpl::minus<IndexVal, mpl::int_<1>>
                    >
        { };

        template <typename S, typename CurVal, typename NextVal, typename IndexVal>
        struct deref<finite_fibonacci_series_iterator<S, CurVal, NextVal, IndexVal>> : CurVal
        { };
}} // namesapce boost::mpl


TEST_CASE("5-9", "[tmp]")
{
    using seq = finite_fibonacci_series<8>;
    static_assert(mpl::size<seq>() == 8, "");
    static_assert(mpl::back<seq>::type::value == 13, "");
    static_assert(
            mpl::deref<
                    mpl::prior<
                            mpl::prior<
                                    mpl::end<seq>::type
                            >::type
                    >::type
            >::type::value == 8,
            ""
    );
    static_assert(
            mpl::deref<
                    mpl::advance_c<
                            mpl::end<seq>::type,
                            -2
                    >::type
            >::type::value == 8,
            ""
    );
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE("5-10", "[tmp]")
{
    using namespace mpl::placeholders;

    using tree_seq = tree<
                         double,
                         tree<void *, int, long>,
                         char
                     >;

    static_assert(
            mpl::equal<
                    preorder_view<tree_seq>,
                    mpl::vector<double, void *, int, long, char>,
                    std::is_same<_1, _2>
            >(),
            ""
    );

    static_assert(
            mpl::equal<
                    inorder_view<tree_seq>,
                    mpl::vector<int, void *, long, double, char>,
                    std::is_same<_1, _2>
            >(),
            ""
    );

    static_assert(
            mpl::equal<
                    postorder_view<tree_seq>,
                    mpl::vector<int, long, void *, char, double>,
                    std::is_same<_1, _2>
            >(),
            ""
    );
}

