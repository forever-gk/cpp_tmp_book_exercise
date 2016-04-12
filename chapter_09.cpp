#include "catch.hpp"

#include <type_traits>
#include <tuple>

#include <boost/compressed_pair.hpp>


template <typename F, bool F_empty, typename G, bool G_empty>
class storage;

template <typename F, typename G>
class storage<F, false, G, false>
{
protected:
    storage(F const& f, G const& g)
            : f(f), g(g)
    { }

protected:
    F const& get_f() const { return f; }
    G const& get_g() const { return g; }

private:
    F f;
    G g;
};

template <typename F, typename G>
class storage<F, true, G, false>
        : F
{
protected:
    storage(F const& f, G const& g)
            : F(f), g(g)
    { }

protected:
    F const& get_f() const { return *this; }
    G const& get_g() const { return g; }

private:
    G g;
};

template <typename F, typename G>
class storage<F, false, G, true>
        : G
{
protected:
    storage(F const& f, G const& g)
            : f(f), G(g)
    { }

protected:
    F const& get_f() const { return f; }
    G const& get_g() const { return *this; }

private:
    F f;
};

template <typename F, typename G>
class storage<F, true, G, true>
        : F, G
{
protected:
    storage(F const& f, G const& g)
            : F(f), G(g)
    { }

protected:
    F const& get_f() const { return *this; }
    G const& get_g() const { return *this; }
};

template <typename F>
class storage<F, true, F, true>
        : F
{
protected:
    storage(F const& f, F const&)
            : F(f)
    { }

protected:
    F const& get_f() const { return *this; }
    F const& get_g() const { return *this; }
};


template <typename R, typename F, typename G>
class compose_fg
        : storage<
                F, std::is_empty<F>::value,
                G, std::is_empty<G>::value
            >
{
private:
    using base = storage<
                         F, std::is_empty<F>::value,
                         G, std::is_empty<G>::value
                    >;

public:
    compose_fg(F const& f, G const& g)
            : base(f, g)
    { }

    template <typename T>
    R operator () (T const& x) const
    {
        F const& f = this->get_f();
        G const& g = this->get_g();
        return f(g(x));
    }
};


template <typename R, typename F, typename G>
auto compose(F const& f, G const& g)
{
    return compose_fg<R, F, G>(f, g);
}


// refer to http://en.cppreference.com/w/cpp/language/ebo
TEST_CASE("9-0", "[tmp]")
{
    struct Empty1 { };
    struct Empty2 { };
    struct Empty3 { };

    struct X : Empty1 { };
    struct Y : Empty1, Empty2 { };
    struct Z : Empty1, Empty2, Empty3 { };

    static_assert(1 == sizeof(Empty1), "");
    static_assert(1 == sizeof(Empty2), "");
    static_assert(1 == sizeof(Empty3), "");

    // NOTE: the latest clang and msvc do Multiple-EBCO... but they behave differently...
    static_assert(1 == sizeof(X), "");
    static_assert(1 == sizeof(Y), "");

#ifndef _MSC_VER
    static_assert(1 == sizeof(Z), "");
#else
    static_assert(2 == sizeof(Z), "");
#endif

    // NOTE: the latest clang and msvc don't do E(B)CO for pairs.
    static_assert(2 == sizeof(std::pair<Empty1, Empty2>), "");

    static_assert(1 == sizeof(boost::compressed_pair<Empty1, Empty2>), "");

    // NOTE: the latest clang does E(B)CO for tuples, but msvc doesn't.
#ifndef _MSC_VER
    static_assert(1 == sizeof(std::tuple<>), "");
    static_assert(1 == sizeof(std::tuple<Empty1>), "");
    static_assert(1 == sizeof(std::tuple<Empty1, Empty2>), "");
    static_assert(1 == sizeof(std::tuple<Empty1, Empty2, Empty3>), "");
#else
    static_assert(1 == sizeof(std::tuple<>), "");
    static_assert(1 == sizeof(std::tuple<Empty1>), "");
    static_assert(2 == sizeof(std::tuple<Empty1, Empty2>), "");
    static_assert(3 == sizeof(std::tuple<Empty1, Empty2, Empty3>), "");
#endif
}

TEST_CASE("9-1", "[tmp]")
{
    struct S
    {
        int operator () (int i) const
        {
            return i * i;
        }
    };

    auto composedFunc = compose<int>(S(), S());
    REQUIRE(16 == composedFunc(2));
}

