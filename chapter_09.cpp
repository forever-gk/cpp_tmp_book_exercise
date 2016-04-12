#include "catch.hpp"

#include <type_traits>


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
    F const& get_f() { return f; }
    G const& get_g() { return g; }

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
    F const& get_f() { return *this; }
    G const& get_g() { return g; }

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
    F const& get_f() { return f; }
    G const& get_g() { return *this; }

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
    F const& get_f() { return *this; }
    G const& get_g() { return *this; }
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


TEST_CASE("", "[tmp]")
{

}

