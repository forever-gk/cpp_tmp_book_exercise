#include "catch.hpp"

#include <type_traits>

#include <boost/mpl/apply.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/equal.hpp>
#include <boost/mpl/plus.hpp>
#include <boost/mpl/multiplies.hpp>


namespace mpl = boost::mpl;


////////////////////////////////////////////////////////////////////////////////
template <unsigned long N>
struct binary
{
    static_assert(N % 10 == 1 || N % 10 == 0, "N should have only '1' or '0'.");

    static unsigned const value = binary<N/10>::value * 2 + N % 10;
};

template <>
struct binary<0>
{
    static unsigned const value = 0;
};


TEST_CASE("3-0", "[tmp]")
{
    static_assert(10 == binary<1010>::value, "");

    // expected compile error.
    //static_assert(binary<1919>::value > 0, "");
}


////////////////////////////////////////////////////////////////////////////////
TEST_CASE("3-1", "[tmp]")
{
    using namespace mpl::placeholders;

    using transformed_t = mpl::transform<
                                mpl::vector_c<int, 1, 2, 3>,
                                mpl::plus<_, mpl::int_<1>>
                            >::type;

    static_assert(mpl::equal<transformed_t, mpl::vector_c<int, 2, 3, 4>>(), "");
}


////////////////////////////////////////////////////////////////////////////////
TEST_CASE("3-2", "[tmp]")
{
    using namespace mpl::placeholders;

    using transformed_t = mpl::transform<
                                mpl::vector_c<int, 1, 2, 3>,
                                mpl::multiplies<_1, _1>
                            >::type;

    static_assert(mpl::equal<transformed_t, mpl::vector_c<int, 1, 4, 9>>(), "");
}


////////////////////////////////////////////////////////////////////////////////
template <typename f, typename x>
struct twice
        : mpl::apply<f, typename mpl::apply<f, x>::type>
{ };


template <typename T>
class quad_pointer
{
private:
    using add_ptr_t = std::add_pointer<mpl::placeholders::_>;

    using double_pointer_t = typename twice<add_ptr_t, T>::type;

public:
    using type = typename twice<add_ptr_t, double_pointer_t>::type;
};


TEST_CASE("3-3", "[tmp]")
{
    static_assert(std::is_same<int ****, typename quad_pointer<int>::type>(), "");
}


////////////////////////////////////////////////////////////////////////////////
struct add_ptr
{
    template <typename T>
    struct apply : std::add_pointer<T>
    { };
};


TEST_CASE("3-4", "[tmp]")
{
    using namespace mpl::placeholders;

    static_assert(
            std::is_same<
                    int ****,
                    typename twice<twice<add_ptr, _>, int>::type
            >(),
            ""
    );
}


////////////////////////////////////////////////////////////////////////////////
namespace dimension_analysis
{
    using mass                  = mpl::vector_c<int, 1, 0, 0, 0, 0, 0, 0>;
    using length                = mpl::vector_c<int, 0, 1, 0, 0, 0, 0, 0>;
    using time                  = mpl::vector_c<int, 0, 0, 1, 0, 0, 0, 0>;
    using charge                = mpl::vector_c<int, 0, 0, 0, 1, 0, 0, 0>;
    using temperature           = mpl::vector_c<int, 0, 0, 0, 0, 1, 0, 0>;
    using intensity             = mpl::vector_c<int, 0, 0, 0, 0, 0, 1, 0>;
    using amount_of_substance   = mpl::vector_c<int, 0, 0, 0, 0, 0, 0, 1>;

    using velocity              = mpl::vector_c<int, 0, 1, -1, 0, 0, 0, 0>; // l/t
    using acceleration          = mpl::vector_c<int, 0, 1, -2, 0, 0, 0, 0>; // l/(t^2)
    using momentum              = mpl::vector_c<int, 1, 1, -1, 0, 0, 0, 0>; // ml/t
    using force                 = mpl::vector_c<int, 1, 1, -2, 0, 0, 0, 0>; // ml/(t^2)

    using scalar                = mpl::vector_c<int, 0, 0, 0, 0, 0, 0, 0>;


    template<typename T, typename Dimensions>
    class quantity {
    public:
        explicit quantity(T x)
                : m_value(x) { }

    public:
        T value() const {
            return m_value;
        }

    private:
        T m_value;
    };
} // namespace dimension_analysis


TEST_CASE("3-5", "[tmp]")
{
    using namespace dimension_analysis;

}

