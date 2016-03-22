#ifndef CPP_TMP_BOOK_EXERCISE_TREE_H
#define CPP_TMP_BOOK_EXERCISE_TREE_H


#include <boost/mpl/int.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/logical.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/empty.hpp>
#include <boost/mpl/insert_range.hpp>


//==============================================================================
struct null_child
{ };


template <typename Parent, typename LeftChild = null_child, typename RightChild = null_child>
struct tree;


template <typename T>
struct is_tree : boost::mpl::false_
{ };

template <typename Parent, typename LeftChild, typename RightChild>
struct is_tree<tree<Parent, LeftChild, RightChild>> : boost::mpl::true_
{ };


template <typename Parent, typename LeftChild, typename RightChild>
struct tree
{
    static_assert(!is_tree<Parent>(), "Parent should not be a tree itself.");
};


namespace detail
{
    namespace mpl = boost::mpl;

    //==============================================================================
    struct preorder_view_tag
    { };


    template <typename PreOrderView, typename TraversalStack>
    struct preorder_view_iterator
    {
        using category = mpl::forward_iterator_tag;
    };


    template <typename T>
    struct next_preorder
            : mpl::vector<T>
    { };

    template <typename Parent, typename LeftChild, typename RightChild>
    struct next_preorder<tree<Parent, LeftChild, RightChild>>
            : mpl::vector<
                    RightChild,
                    LeftChild,
                    Parent
              >
    { };


    //==============================================================================
    struct inorder_view_tag
    { };


    template <typename InOrderView, typename TraversalStack>
    struct inorder_view_iterator
    {
        using category = mpl::forward_iterator_tag;
    };


    template <typename T>
    struct next_inorder
            : mpl::vector<T>
    { };

    template <typename Parent, typename LeftChild, typename RightChild>
    struct next_inorder<tree<Parent, LeftChild, RightChild>>
            : mpl::insert_range<
                    mpl::vector<RightChild, Parent>,
                    typename mpl::end<mpl::vector<RightChild, Parent>>::type,
                    typename next_inorder<LeftChild>::type
                >
    { };


    //==============================================================================
    struct postorder_view_tag
    { };


    template <typename PostOrderView, typename TraversalStack>
    struct postorder_view_iterator
    {
        using category = mpl::forward_iterator_tag;
    };


    template <typename T>
    struct next_postorder
            : mpl::vector<T>
    { };

    template <typename Parent, typename LeftChild, typename RightChild>
    struct next_postorder<tree<Parent, LeftChild, RightChild>>
            : mpl::insert_range<
                    mpl::vector<Parent, RightChild>,
                    typename mpl::end<mpl::vector<Parent, RightChild>>::type,
                    typename next_postorder<LeftChild>::type
                >
    { };


    //==============================================================================
    template <typename TraversalStack>
    struct pop_back_until_not_null_child
            : mpl::eval_if<
                    mpl::or_<
                            mpl::empty<TraversalStack>,
                            mpl::not_<std::is_same<null_child, typename mpl::back<TraversalStack>::type>>
                    >,
                    TraversalStack,
                    pop_back_until_not_null_child<typename mpl::pop_back<TraversalStack>::type>
                >
    { };


    template <typename NextOrderGenerator, typename TraversalStack>
    struct push_back_next_traversal
            : mpl::insert_range<
                    typename mpl::pop_back<TraversalStack>::type,
                    typename mpl::end<typename mpl::pop_back<TraversalStack>::type>::type,
                    typename mpl::apply<NextOrderGenerator, typename mpl::back<TraversalStack>::type>::type
                >
    { };


    template <typename NextOrderGenerator, typename TraversalStack>
    class next_traversal_stack
    {
    private:
        using ts = typename mpl::eval_if<
                                    mpl::empty<TraversalStack>,
                                    mpl::vector<>,
                                    pop_back_until_not_null_child<TraversalStack>
                                >::type;

    public:
        using type = typename mpl::eval_if<
                                        mpl::empty<ts>,
                                        mpl::identity<void>,
                                        push_back_next_traversal<NextOrderGenerator, ts>
                                >::type;
    };
} // namespace detail


template <typename Tree>
struct preorder_view
{
    using type = preorder_view;
    using tag = detail::preorder_view_tag;

    using tree = Tree;
};


template <typename Tree>
struct inorder_view
{
    using type = inorder_view;
    using tag = detail::inorder_view_tag;

    using tree = Tree;
};


template <typename Tree>
struct postorder_view
{
    using type = postorder_view;
    using tag = detail::postorder_view_tag;

    using tree = Tree;
};


namespace boost { namespace mpl {
        using namespace detail;

        //======================================================================
        template <>
        struct begin_impl<preorder_view_tag>
        {
            template <typename PreOrderView>
            struct apply
            {
                using type = preorder_view_iterator<
                                     PreOrderView,
                                     typename next_preorder<typename PreOrderView::tree>::type
                                >;
            };
        };

        template <>
        struct end_impl<preorder_view_tag>
        {
            template <typename PreOrderView>
            struct apply
            {
                using type = preorder_view_iterator<
                                     PreOrderView,
                                     void
                                >;
            };
        };

        template <typename PreOrderView, typename TraversalStack>
        struct next<preorder_view_iterator<PreOrderView, TraversalStack>>
        {
            using type = preorder_view_iterator<
                                 PreOrderView,
                                 typename next_traversal_stack<
                                                  next_preorder<mpl::placeholders::_>,
                                                  typename mpl::pop_back<TraversalStack>::type
                                              >::type
                            >;
        };

        template <typename PreOrderView, typename TraversalStack>
        struct deref<preorder_view_iterator<PreOrderView, TraversalStack>>
                : mpl::back<TraversalStack>
        { };


        //======================================================================
        template <>
        struct begin_impl<inorder_view_tag>
        {
            template <typename InOrderView>
            struct apply
            {
                using type = inorder_view_iterator<
                                     InOrderView,
                                     typename next_inorder<typename InOrderView::tree>::type
                                >;
            };
        };

        template <>
        struct end_impl<inorder_view_tag>
        {
            template <typename InOrderView>
            struct apply
            {
                using type = inorder_view_iterator<
                                     InOrderView,
                                     void
                                >;
            };
        };

        template <typename InOrderView, typename TraversalStack>
        struct next<inorder_view_iterator<InOrderView, TraversalStack>>
        {
            using type = inorder_view_iterator<
                                 InOrderView,
                                 typename next_traversal_stack<
                                                  next_inorder<mpl::placeholders::_>,
                                                  typename mpl::pop_back<TraversalStack>::type
                                              >::type
            >;
        };

        template <typename InOrderView, typename TraversalStack>
        struct deref<inorder_view_iterator<InOrderView, TraversalStack>>
                : mpl::back<TraversalStack>
        { };


        //======================================================================
        template <>
        struct begin_impl<postorder_view_tag>
        {
            template <typename PostOrderView>
            struct apply
            {
                using type = postorder_view_iterator<
                                     PostOrderView,
                                     typename next_postorder<typename PostOrderView::tree>::type
                                >;
            };
        };

        template <>
        struct end_impl<postorder_view_tag>
        {
            template <typename PostOrderView>
            struct apply
            {
                using type = postorder_view_iterator<
                                     PostOrderView,
                                     void
                                >;
            };
        };

        template <typename PostOrderView, typename TraversalStack>
        struct next<postorder_view_iterator<PostOrderView, TraversalStack>>
        {
            using type = postorder_view_iterator<
                                 PostOrderView,
                                 typename next_traversal_stack<
                                                  next_postorder<mpl::placeholders::_>,
                                                  typename mpl::pop_back<TraversalStack>::type
                                              >::type
            >;
        };

        template <typename PostOrderView, typename TraversalStack>
        struct deref<postorder_view_iterator<PostOrderView, TraversalStack>>
                : mpl::back<TraversalStack>
        { };
}} // namespace boost::mpl


#endif //CPP_TMP_BOOK_EXERCISE_TREE_H

