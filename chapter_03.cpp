#include "catch.hpp"

#include <type_traits>

#include <boost/mpl/vector_c.hpp>


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
using mass                  = mpl::vector_c<int, 1, 0, 0, 0, 0, 0, 0>;
using length                = mpl::vector_c<int, 0, 1, 0, 0, 0, 0, 0>;
using time_                 = mpl::vector_c<int, 0, 0, 1, 0, 0, 0, 0>;
using charge                = mpl::vector_c<int, 0, 0, 0, 1, 0, 0, 0>;
using temperature           = mpl::vector_c<int, 0, 0, 0, 0, 1, 0, 0>;
using intensity             = mpl::vector_c<int, 0, 0, 0, 0, 0, 1, 0>;
using amount_of_substane    = mpl::vector_c<int, 0, 0, 0, 0, 0, 0, 1>;

using velocity              = mpl::vector_c<int, 0, 1, -1, 0, 0, 0, 0>; // l/t
using acceleration          = mpl::vector_c<int, 0, 1, -2, 0, 0, 0, 0>; // l/(t^2)
using momentum              = mpl::vector_c<int, 1, 1, -1, 0, 0, 0, 0>; // ml/t
using force                 = mpl::vector_c<int, 1, 1, -2, 0, 0, 0, 0>; // ml/(t^2)

using scalar                = mpl::vector_c<int, 0, 0, 0, 0, 0, 0, 0>;


template <typename T, typename Dimensions>
class quantity
{
public:
    explicit quantity(T x)
            : m_value(x)
    { }

public:
    T value() const
    {
        return m_value;
    }

private:
    T m_value;
};


TEST_CASE("3-5", "[tmp]")
{

}

