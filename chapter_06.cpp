#include "catch.hpp"

#include "tree.h"

#include <boost/mpl/sizeof.hpp>
#include <boost/mpl/char.hpp>
#include <boost/mpl/string.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/back.hpp>
#include <boost/mpl/comparison.hpp>
#include <boost/mpl/arithmetic.hpp>
#include <boost/mpl/copy.hpp>
#include <boost/mpl/equal.hpp>
#include <boost/mpl/find_if.hpp>


namespace mpl = boost::mpl;


////////////////////////////////////////////////////////////////////////////////
template <typename lhs, typename rhs>
struct min_type
        : mpl::if_<
                mpl::less_equal<mpl::sizeof_<lhs>, mpl::sizeof_<rhs>>,
                lhs,
                rhs
            >
{ };

template <typename Sequence>
struct smallest
        : mpl::copy<
                Sequence,
                mpl::inserter<
                        typename mpl::front<Sequence>::type,
                        min_type<mpl::placeholders::_, mpl::placeholders::_>
                >
          >
{ };


template <typename Sequence>
struct smallest_fold
        : mpl::fold<
                Sequence,
                typename mpl::front<Sequence>::type,
                min_type<mpl::placeholders::_, mpl::placeholders::_>
            >
{ };


TEST_CASE("6-0", "[tmp]")
{
    using std::is_same;

    static_assert(
            is_same<
                    char,
                    smallest<mpl::vector<int[2], char, double &>>::type
            >(),
            ""
    );

    static_assert(
            is_same<
                    char,
                    smallest_fold<mpl::vector<int[2], char, double &>>::type
            >(),
            ""
    );
}


////////////////////////////////////////////////////////////////////////////////
TEST_CASE("multi-character sequence", "[tmp]")
{
    constexpr auto cs = 'abcd';
    static_assert(std::is_same<int const, decltype(cs)>(), "");
    static_assert(4 == sizeof(cs), "");
    static_assert('d' == ((cs >> 0)  & 0xFF), "");
    static_assert('c' == ((cs >> 8)  & 0xFF), "");
    static_assert('b' == ((cs >> 16) & 0xFF), "");
    static_assert('a' == ((cs >> 24) & 0xFF), "");

    /*
    // NOTE: compile warning on clang: character constant too long for its type
    constexpr auto cs1 = 'abcde';
    static_assert(std::is_same<int const, decltype(cs1)>(), "");
    static_assert(4 == sizeof(cs1), "");
    static_assert('e' == ((cs1 >> 0)  & 0xFF), "");
    static_assert('d' == ((cs1 >> 8)  & 0xFF), "");
    static_assert('c' == ((cs1 >> 16) & 0xFF), "");
    static_assert('b' == ((cs1 >> 24) & 0xFF), "");

    // NOTE: 'a' was truncated. compile error on clang.
    //static_assert('a' != ((cs1 >> 32) & 0xFF), "");
     */
}


template <typename AccumulatedPair, typename c>
struct append_digit_value
        : mpl::pair<
                mpl::plus<
                        typename AccumulatedPair::first,
                        mpl::multiplies<
                                mpl::minus<c, mpl::char_<'0'>>,
                                typename AccumulatedPair::second
                        >
                >,
                mpl::multiplies<
                        typename AccumulatedPair::second,
                        mpl::int_<2>
                >
            >
{
    static_assert(
            mpl::or_<
                    mpl::equal_to<mpl::char_<'0'>, c>,
                    mpl::equal_to<mpl::char_<'1'>, c>
            >(),
            "c should be '0' or '1'."
    );
};

template <int chars>
struct binary_char
        : mpl::reverse_fold<
                mpl::string<chars>,
                mpl::pair<mpl::int_<0>, mpl::int_<1>>,
                append_digit_value<mpl::placeholders::_, mpl::placeholders::_>
            >::type::first
{ };


TEST_CASE("6-1", "[tmp]")
{
    static_assert(mpl::size<mpl::string<'1010'>>() == 4, "");
    static_assert('1' == mpl::front<mpl::string<'1010'>>::type::value, "");
    static_assert('0' == mpl::back<mpl::string<'1010'>>::type::value, "");

    static_assert(mpl::equal_to<binary_char<'1000'>::type, mpl::int_<8>>(), "");
    static_assert(mpl::equal_to<binary_char<'1010'>::type, mpl::int_<10>>(), "");

    // expected compile error
    //static_assert(mpl::equal_to<binary_char<'1019'>::type, mpl::int_<10>>(), "");
}

////////////////////////////////////////////////////////////////////////////////
namespace detail
{
    template <typename leaf, typename item>
    struct binary_tree_inserter_impl
            : mpl::if_<
                    mpl::less_equal<
                            item, leaf
                    >,
                    tree<leaf, item, null_item>,
                    tree<leaf, null_item, item>
            >
    { };

    template <typename item>
    struct binary_tree_inserter_impl<tree<>, item>
    {
        using type = tree<item>;
    };

    template <typename item>
    struct binary_tree_inserter_impl<null_item, item>
    {
        using type = item;
    };

    template <typename parent, typename lhs, typename rhs, typename item>
    struct binary_tree_inserter_impl<tree<parent, lhs, rhs>, item>
            : mpl::eval_if<
                    mpl::less_equal<
                            item, parent
                    >,
                    tree<
                            parent,
                            binary_tree_inserter_impl<lhs, item>,
                            rhs
                    >,
                    tree<
                            parent,
                            lhs,
                            binary_tree_inserter_impl<rhs, item>
                    >
                >
    { };
} // namespace detail


template <typename parent, typename lhs, typename item, typename rhs>
struct tree<parent, detail::binary_tree_inserter_impl<lhs, item>, rhs>
{
    using type = tree<
                         parent,
                         typename detail::binary_tree_inserter_impl<lhs, item>::type,
                         rhs
                    >;
};

template <typename parent, typename lhs, typename item, typename rhs>
struct tree<parent, lhs, detail::binary_tree_inserter_impl<rhs, item>>
{
    using type = tree<
                         parent,
                         lhs,
                         typename detail::binary_tree_inserter_impl<rhs, item>::type
                    >;
};


template
<
        typename Tree = tree<>,
        typename Inserter = detail::binary_tree_inserter_impl<mpl::placeholders::_, mpl::placeholders::_>
>
struct binary_tree_inserter
{
    using state = Tree;
    using operation = Inserter;
};


TEST_CASE("6-3", "[tmp]")
{
    using bst = mpl::copy<
                        mpl::vector_c<int, 17, 25, 10, 2, 11>,
                        binary_tree_inserter<>
                    >::type;

    static_assert(
            mpl::equal<
                    inorder_view<bst>,
                    mpl::vector_c<int, 2, 10, 11, 17, 25>
            >(),
            ""
    );
}

////////////////////////////////////////////////////////////////////////////////
struct binary_tree_search_order_view_tag
{ };


template <typename BinaryTree, typename Item>
struct binary_tree_search_order_view
{
    using type = binary_tree_search_order_view;
    using tag = binary_tree_search_order_view_tag;

    using tree = BinaryTree;
    using item = Item;
};


template <typename BinaryTreeSearchOrderView, typename TraversalStack>
struct binary_tree_search_order_view_iterator
{
    using category = mpl::forward_iterator_tag;
};


template <typename T, typename Item>
struct next_search_order
        : mpl::vector<T>
{ };

template <typename Parent, typename LeftChild, typename RightChild, typename Item>
struct next_search_order<tree<Parent, LeftChild, RightChild>, Item>
        : mpl::vector<
                typename mpl::if_<
                                 mpl::less_equal<Item, Parent>,
                                 LeftChild,
                                 RightChild
                         >::type,
                Parent
            >
{ };


namespace boost { namespace mpl {
        template <>
        struct begin_impl<binary_tree_search_order_view_tag>
        {
            template <typename BinaryTreeSearchOrderView>
            struct apply
            {
                using type = binary_tree_search_order_view_iterator<
                                    BinaryTreeSearchOrderView,
                                    typename next_search_order<
                                                 typename BinaryTreeSearchOrderView::tree,
                                                 typename BinaryTreeSearchOrderView::item
                                             >::type
                                >;
            };
        };

        template <>
        struct end_impl<binary_tree_search_order_view_tag>
        {
            template <typename BinaryTreeSearchOrderView>
            struct apply
            {
                using type = binary_tree_search_order_view_iterator<
                                    BinaryTreeSearchOrderView,
                                    void
                                >;
            };
        };

        template <typename BinaryTreeSearchOrderView, typename TraversalStack>
        struct next<binary_tree_search_order_view_iterator<BinaryTreeSearchOrderView, TraversalStack>>
        {
            using type = binary_tree_search_order_view_iterator<
                                BinaryTreeSearchOrderView,
                                typename next_traversal_stack<
                                                next_search_order<
                                                        mpl::placeholders::_,
                                                        typename BinaryTreeSearchOrderView::item
                                                >,
                                                typename mpl::pop_back<TraversalStack>::type
                                            >::type
                            >;
        };

        template <typename BinaryTreeSearchOrderView, typename TraversalStack>
        struct deref<binary_tree_search_order_view_iterator<BinaryTreeSearchOrderView, TraversalStack>>
                : mpl::back<TraversalStack>
        { };
}} // namespace boost::mpl


template <typename Tree, typename Item>
struct binary_tree_search
        : mpl::find_if<
                binary_tree_search_order_view<Tree, Item>,
                mpl::equal_to<mpl::placeholders::_, Item>
            >
{ };


TEST_CASE("6-4", "[tmp]")
{
    using bst = mpl::copy<
                        mpl::vector_c<int, 17, 25, 10, 2, 11>,
                        binary_tree_inserter<>
                    >::type;

    using pos1 = binary_tree_search<bst, mpl::int_<11>>::type;
    using pos2 = binary_tree_search<bst, mpl::int_<20>>::type;
    using end_pos1 = mpl::end<binary_tree_search_order_view<bst, mpl::int_<11>>>::type;
    using end_pos2 = mpl::end<binary_tree_search_order_view<bst, mpl::int_<20>>>::type;

    static_assert(!std::is_same<pos1, end_pos1>(), "");
    static_assert(std::is_same<pos2, end_pos2>(), "");

    static_assert(mpl::equal_to<mpl::deref<pos1>::type, mpl::int_<11>>(), "");
}

