#include "catch.hpp"

#include <type_traits>


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

TEST_CASE("2-1", "[tmp]")
{
    using std::is_same;

    static_assert(is_same<int const &, add_const_ref_t<int>>(), "");
    static_assert(is_same<int &, add_const_ref_t<int &>>(), "");
}

