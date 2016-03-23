#include <boost/mpl/vector.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/mpl/list_c.hpp>
#include <boost/mpl/list.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/joint_view.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/equal.hpp>

#include "catch.hpp"


namespace mpl = boost::mpl;


////////////////////////////////////////////////////////////////////////////////
template <typename IteratorSeq>
struct zip_iterator
{
    using category = mpl::forward_iterator_tag;

    using base = IteratorSeq;

    using type = typename mpl::transform<
                                IteratorSeq,
                                mpl::deref<mpl::placeholders::_>
                            >::type;
};


template <typename... Sequence>
struct zip_view
        : mpl::iterator_range<
                zip_iterator<
                        typename mpl::transform<
                                        mpl::vector<Sequence...>,
                                        mpl::begin<mpl::placeholders::_>
                                    >::type
                >,
                zip_iterator<
                        typename mpl::transform<
                                        mpl::vector<Sequence...>,
                                        mpl::end<mpl::placeholders::_>
                                    >::type
                >
            >
{ };


namespace boost { namespace mpl {
        template <typename IteratorSeq>
        struct next<zip_iterator<IteratorSeq>>
        {
            using type = zip_iterator<
                                typename mpl::transform<
                                                IteratorSeq,
                                                next<mpl::placeholders::_>
                                            >::type
                            >;
        };

        // NOTE: There is no specialization of mpl::deref for zip_iterator.
        //          It seems that the 'type' member of zip_iterator is used for mpl::deref's return type.
        //          mpl::begin and mpl::end will be provided by the mpl::iterator_range automatically.
}} // namespace boost::mpl


TEST_CASE("7-0", "[tmp]")
{
    using tc0 = mpl::vector<char, int, float, double>;
    using tc1 = mpl::list<double, float, int, char>;
    using tc2 = mpl::vector<char *, int *, float *, double *>;

    using zipped_t = zip_view<tc0, tc1, tc2>;

    static_assert(
            mpl::equal<
                    mpl::vector<char, double, char *>,
                    mpl::deref<
                            mpl::begin<zipped_t>::type
                    >::type
            >(),
            ""
    );

    static_assert(
            mpl::equal<
                    mpl::vector<float, int, float *>,
                    mpl::deref<
                            mpl::advance_c<mpl::begin<zipped_t>::type, 2>::type
                    >::type
            >(),
            ""
    );
}


////////////////////////////////////////////////////////////////////////////////
template <typename Sequence, typename RotationCenterPositionIterator>
struct rotate_view
        : mpl::joint_view<
                mpl::iterator_range<
                        RotationCenterPositionIterator,
                        typename mpl::end<Sequence>::type
                >,
                mpl::iterator_range<
                        typename mpl::begin<Sequence>::type,
                        RotationCenterPositionIterator
                >
            >
{ };


TEST_CASE("7-3", "[tmp]")
{
    using v = mpl::vector_c<int, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4>;
    using view = rotate_view<
                        v,
                        mpl::advance_c<mpl::begin<v>::type, 5>::type
                    >;

    static_assert(
            mpl::equal<
                    view,
                    mpl::range_c<int, 0, 10>
            >(),
            ""
    );
}


////////////////////////////////////////////////////////////////////////////////
template <typename IndexSequenceIterator, typename RandomAccessIteratorBegin>
struct permutation_iterator
{
    using category = mpl::forward_iterator_tag;

    using type = typename mpl::deref<
                                  typename mpl::advance<
                                                   RandomAccessIteratorBegin,
                                                   typename mpl::deref<IndexSequenceIterator>::type
                                               >::type
                            >::type;
};


namespace boost { namespace mpl {
        template <typename IndexSequenceIterator, typename RandomAccessIteratorBegin>
        struct next<permutation_iterator<IndexSequenceIterator, RandomAccessIteratorBegin>>
        {
            using type = permutation_iterator<
                                typename mpl::next<IndexSequenceIterator>::type,
                                RandomAccessIteratorBegin
                            >;
        };
}} // namespace boost::mpl


template <typename IndexSequence, typename RandomAccessSequence>
struct permutation_view
        : mpl::iterator_range<
                permutation_iterator<
                        typename mpl::begin<IndexSequence>::type,
                        typename mpl::begin<RandomAccessSequence>::type
                >,
                permutation_iterator<
                        typename mpl::end<IndexSequence>::type,
                        typename mpl::begin<RandomAccessSequence>::type
                >
            >
{ };


TEST_CASE("7-6", "[tmp]")
{
    static_assert(
            mpl::equal<
                    mpl::vector_c<int, 33, 22, 44, 11, 33>,
                    permutation_view<
                            mpl::list_c<int, 2, 1, 3, 0, 2>,
                            mpl::vector_c<int, 11, 22, 33, 44>
                    >
            >(),
            ""
    );
}

