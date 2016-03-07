#include "catch.hpp"

#include <type_traits>
#include <cassert>


////////////////////////////////////////////////////////////////////////////////
template <typename T>
struct add_const_ref
        : std::conditional<
                std::is_reference<T>::value,
                T,
                T const&
            >
{ };

template <typename T>
using add_const_ref_t = typename add_const_ref<T>::type;

TEST_CASE("2-0", "[tmp]")
{
    using std::is_same;

    static_assert(is_same<int const &, add_const_ref_t<int>>(), "");
    static_assert(is_same<int &, add_const_ref_t<int &>>(), "");
}


////////////////////////////////////////////////////////////////////////////////
template <typename T, typename SourceType, typename TargetType>
struct replace_type
{
    // NOTE: this primary template will be selected when T and SourceType is different.
    static_assert(
            std::is_same<T, SourceType>(),
            "T and SourceType should be same type, or SourceType should be part of compound type T."
    );
};

template <typename T, typename SourceType, typename TargetType>
using replace_type_t = typename replace_type<T, SourceType, TargetType>::type;

template <typename S, typename T>
struct replace_type<S, S, T>
{
    using type = T;
};

template <typename S, typename T>
struct replace_type<S *, S, T>
{
    using type = T *;
};

template <typename S, std::size_t N, typename T>
struct replace_type<S * [N], S, T>
{
    using type = T * [N];
};

template <typename R, typename... Arg, typename S, typename T>
struct replace_type<R (*) (Arg...), S, T>
{
    using type = T (*) (replace_type_t<Arg, S, T>...);
};

TEST_CASE("2-1", "[tmp]")
{
    using std::is_same;

    static_assert(
            is_same<
                    int *,
                    replace_type_t<void *, void, int>
            >(),
            ""
    );
    static_assert(
            is_same<
                    long * [10],
                    replace_type_t<int const * [10], int const, long>
            >(),
            ""
    );
    static_assert(
            is_same<
                    long & (*) (long &),
                    replace_type_t<char & (*) (char &), char &, long &>
            >(),
            ""
    );
    static_assert(
            is_same<
                    long & (*) (long &, long &),
                    replace_type_t<char & (*) (char &, char &), char &, long &>
            >(),
            ""
    );
}

////////////////////////////////////////////////////////////////////////////////
template <typename Target, typename Source>
inline Target polymorphic_downcast(Source * x)
{
    static_assert(std::is_pointer<Target>(), "Target should be pointer type.");
    assert(dynamic_cast<Target>(x) != nullptr);
    return static_cast<Target>(x);
}

template <typename Target, typename Source>
inline Target polymorphic_downcast(Source & x)
{
    static_assert(std::is_reference<Target>(), "Target should be reference type.");
    assert(dynamic_cast<std::add_pointer_t<Target>>(&x) != nullptr);
    return static_cast<Target>(x);
}

TEST_CASE("2-2", "[tmp]")
{
    struct A { virtual ~A() { } };
    struct B : A { };

    B b;
    A * a_ptr = &b;
    B * b_ptr = polymorphic_downcast<B *>(a_ptr);

    A & a_ref = b;
    B & b_ref = polymorphic_downcast<B &>(a_ref);
}

