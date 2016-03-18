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
                    twice<twice<add_ptr, _>, int>::type
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
    class quantity
    {
    public:
        explicit quantity(T x)
                : m_value(x)
        { }

        template <typename OtherDimensions>
        quantity(quantity<T, OtherDimensions> const& q)
                : m_value(q.value())
        {
            static_assert(mpl::equal<Dimensions, OtherDimensions>(), "OtherDimensions should be same as Dimensions.");
        }

    public:
        T value() const
        {
            return m_value;
        }

    private:
        T m_value;
    };

    template <typename T1, typename T2, typename D1, typename D2>
    auto operator + (quantity<T1, D1> const& lhs, quantity<T2, D2> const& rhs)
    {
        static_assert(mpl::equal<D1, D2>(), "D1 and D2 dimensions should be same.");
        return quantity<decltype(lhs.value() + rhs.value()), D1>(lhs.value() + rhs.value());
    }

    template <typename T1, typename T2, typename D1, typename D2>
    auto operator - (quantity<T1, D1> const& lhs, quantity<T2, D2> const& rhs)
    {
        static_assert(mpl::equal<D1, D2>(), "D1 and D2 dimensions should be same.");
        return quantity<decltype(lhs.value() + rhs.value()), D1>(lhs.value() - rhs.value());
    }

    using mpl::placeholders::_;

    template <typename D1, typename D2>
    struct multiply_dimensions
            : mpl::transform<D1, D2, mpl::plus<_, _>>
    { };

    template <typename T1, typename T2, typename D1, typename D2>
    auto operator * (quantity<T1, D1> const& lhs, quantity<T2, D2> const& rhs)
    {
        return quantity<
                   decltype(lhs.value() * rhs.value()),
                   typename multiply_dimensions<D1, D2>::type
               >(lhs.value() * rhs.value());
    }

    template <typename D1, typename D2>
    struct divide_dimensions
            : mpl::transform<D1, D2, mpl::minus<_, _>>
    { };

    template <typename T1, typename T2, typename D1, typename D2>
    auto operator / (quantity<T1, D1> const& lhs, quantity<T2, D2> const& rhs)
    {
        return quantity<
                decltype(lhs.value() / rhs.value()),
                typename divide_dimensions<D1, D2>::type
               >(lhs.value() / rhs.value());
    }
} // namespace dimension_analysis


TEST_CASE("3-5", "[tmp]")
{
    using namespace dimension_analysis;

    quantity<float, length> l{ 1.0f };
    quantity<float, mass> m{ 2.0f };

    // expected compile error
    //m = l;

    quantity<float, length> len1{ 1.0f };
    quantity<float, length> len2{ 2.0f };

    len1 = len1 + len2;
    REQUIRE(len1.value() == 3.0f);

    // expected compile error
    //len1 = len2 + quantity<float, mass>{ 3.7f };

    quantity<float, mass> m1{ 5.0f };
    quantity<float, acceleration> a1{ 9.8f };
    REQUIRE((m1 * a1).value() == 49.0f);

    quantity<float, force> f1 = m1 * a1;

    quantity<float, mass> m2 = f1 / a1;
    float rounding_error = std::abs((m2 - m1).value());

    f1 = f1 + m1 * a1;
    REQUIRE(f1.value() == 98.0f);
}

