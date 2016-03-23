#include <boost/mpl/vector.hpp>
#include <boost/mpl/list.hpp>
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

