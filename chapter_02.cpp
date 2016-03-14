#include "catch.hpp"

#include <type_traits>
#include <cassert>
#include <array>
#include <iostream>


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

////////////////////////////////////////////////////////////////////////////////
template <typename T> struct name_of { static constexpr auto value = ""; };

template <> struct name_of<char>        { static constexpr auto value = "char"; };
template <> struct name_of<short int>   { static constexpr auto value = "short int"; };
template <> struct name_of<int>         { static constexpr auto value = "int"; };
template <> struct name_of<long int>    { static constexpr auto value = "long int"; };


template <typename T>
class type_descriptor
{
public:
    // NOTE: The implicit std::string conversion operator is not a viable candidate for the overload resolution.
    operator std::string ()
    {
        return to_string();
    }

    std::string to_string()
    {
        if (std::is_const<T>()) {
            return type_descriptor<std::remove_const_t<T>>().to_string() + " const";
        }
        if (std::is_volatile<T>()) {
            return type_descriptor<std::remove_volatile_t<T>>().to_string() + " volatile";
        }
        if (std::is_pointer<T>()) {
            return type_descriptor<std::remove_pointer_t<T>>().to_string() + " *";
        }
        if (std::is_lvalue_reference<T>()) {
            return type_descriptor<std::remove_reference_t<T>>().to_string() + " &";
        }
        if (std::is_rvalue_reference<T>()) {
            return type_descriptor<std::remove_reference_t<T>>().to_string() + " &&";
        }
        if (std::is_array<T>()) {
            extents_.clear();
            get_next_extent<T>();
            std::string s = type_descriptor<std::remove_all_extents_t<T>>();
            for (int extent : extents_) {
                if (0 == extent) {
                    s += "[]";
                } else {
                    s += "[" + std::to_string(extent) + "]";
                }
            }
            return s;
        }
        return name_of<T>::value;
    }

private:
    template <typename U>
    void get_next_extent()
    {
        if (!std::is_array<U>()) {
            return;
        }
        extents_.push_back(std::extent<U>());
        get_next_extent<std::remove_extent_t<U>>();
    }

private:
    std::vector<int> extents_;
};


TEST_CASE("2-3", "[tmp]")
{
    // NOTE: Compile error.
    //          The implicit std::string conversion operator is not a viable candidate for the overload resolution.
    //REQUIRE("int" == type_descriptor<int>());

    REQUIRE("int" == type_descriptor<int>().to_string());

    std::string s = type_descriptor<int>();
    REQUIRE("int" == s);

    REQUIRE("char *" == type_descriptor<char *>().to_string());
    REQUIRE("char * *" == type_descriptor<char **>().to_string());
    REQUIRE("char * const *" == type_descriptor<char * const *>().to_string());
    REQUIRE("long int const *" == type_descriptor<long const *>().to_string());
    REQUIRE("char &" == type_descriptor<char &>().to_string());
    REQUIRE("char &&" == type_descriptor<char &&>().to_string());

    static_assert(std::extent<int [3]>() == 3, "");

    static_assert(std::rank<int * []>() == 1, "");
    static_assert(std::extent<int * [], std::rank<int * []>::value>() == 0, "");
    static_assert(std::is_same<int *, std::remove_extent_t<int * []>>(), "");

    static_assert(std::rank<int * [][3]>() == 2, "");
    static_assert(std::extent<int * [][3], std::rank<int * [][3]>::value>() == 0, "");
    static_assert(std::extent<int * [][3], 1>() == 3, "");
    static_assert(std::extent<int * [][3], 2>() == 0, "");
    static_assert(std::is_same<int * [3], std::remove_extent_t<int * [][3]>>(), "");

    REQUIRE("int[]" == type_descriptor<int []>().to_string());
    REQUIRE("int[3]" == type_descriptor<int [3]>().to_string());
    REQUIRE("int *[]" == type_descriptor<int * []>().to_string());
    REQUIRE("int *[][3]" == type_descriptor<int * [][3]>().to_string());
    REQUIRE("int *[][3][4]" == type_descriptor<int * [][3][4]>().to_string());
}


////////////////////////////////////////////////////////////////////////////////
template <std::size_t... i>
using array_extents_t = std::integer_sequence<std::size_t, i...>;


namespace Detail
{
    template <typename ArrayExtentsSequence, typename T>
    struct AllExtentsImpl;

    template <std::size_t... i, typename T>
    struct AllExtentsImpl<array_extents_t<i...>, T>
    {
        using type = array_extents_t<i...>;
    };

    template <std::size_t... i, typename T, std::size_t N>
    struct AllExtentsImpl<array_extents_t<i...>, T[N]> : AllExtentsImpl<array_extents_t<i..., N>, T>
    { };
} // namespace Detail


template <typename T>
struct AllExtents
{
    static_assert(std::is_array<T>(), "T should be an array.");
};

template <typename T>
struct AllExtents<T[]> : Detail::AllExtentsImpl<array_extents_t<0>, T>
{ };

template <typename T, std::size_t N>
struct AllExtents<T[N]> : Detail::AllExtentsImpl<array_extents_t<N>, T>
{ };


template <typename T>
using AllExtentsT = typename AllExtents<T>::type;


namespace Detail
{
    template <std::size_t... i>
    constexpr auto allExtentsImpl(array_extents_t<i...>)
    {
        return std::array<std::size_t, sizeof...(i)>{ i... };
    }
} // namespace Detail

template <typename T>
constexpr auto allExtents()
{
    return Detail::allExtentsImpl(AllExtentsT<T>());
}


template <typename Derived>
struct type_desc_base
{
    operator std::string ()
    {
        return static_cast<Derived *>(this)->to_string();
    }
};


template <typename T>
struct type_desc : type_desc_base<type_desc<T>>
{
    std::string to_string() const
    {
        return name_of<T>::value;
    }
};

template <typename T>
struct type_desc<T const> : type_desc_base<type_desc<T const>>
{
    std::string to_string() const
    {
        return type_desc<T>().to_string() + " const";
    }
};

template <typename T>
struct type_desc<T volatile> : type_desc_base<type_desc<T volatile>>
{
    std::string to_string() const
    {
        return type_desc<T>().to_string() + " volatile";
    }
};

template <typename T>
struct type_desc<T *> : type_desc_base<type_desc<T *>>
{
    std::string to_string() const
    {
        return type_desc<T>().to_string() + " *";
    }
};

template <typename T>
struct type_desc<T &> : type_desc_base<type_desc<T &>>
{
    std::string to_string() const
    {
        return type_desc<T>().to_string() + " &";
    }
};

template <typename T>
struct type_desc<T &&> : type_desc_base<type_desc<T &&>>
{
    std::string to_string() const
    {
        return type_desc<T>().to_string() + " &&";
    }
};

template <typename T>
struct type_desc<T[]> : type_desc_base<type_desc<T[]>>
{
    std::string to_string() const
    {
        std::string s = type_desc<std::remove_all_extents_t<T>>();
        for (auto i : allExtents<T[]>()) {
            s += "[";
            if (i != 0) {
                s += std::to_string(i);
            }
            s += "]";
        }
        return s;
    }
};

template <typename T, std::size_t N>
struct type_desc<T[N]> : type_desc_base<type_desc<T[N]>>
{
    std::string to_string() const
    {
        std::string s = type_desc<std::remove_all_extents_t<T>>();
        for (auto i : allExtents<T[N]>()) {
            s += "[";
            if (i != 0) {
                s += std::to_string(i);
            }
            s += "]";
        }
        return s;
    }
};


TEST_CASE("2-4", "[tmp]")
{
    // NOTE: Compile error.
    //          The implicit std::string conversion operator is not a viable candidate for the overload resolution.
    //REQUIRE("int" == type_desc<int>());

    REQUIRE("int" == type_desc<int>().to_string());

    std::string s = type_desc<int>();
    REQUIRE("int" == s);

    REQUIRE("char *" == type_desc<char *>().to_string());
    REQUIRE("char * *" == type_desc<char **>().to_string());
    REQUIRE("char * const *" == type_desc<char * const *>().to_string());
    REQUIRE("long int const *" == type_desc<long const *>().to_string());
    REQUIRE("char &" == type_desc<char &>().to_string());
    REQUIRE("char &&" == type_desc<char &&>().to_string());

    static_assert(std::extent<int [3]>() == 3, "");

    static_assert(std::rank<int * []>() == 1, "");
    static_assert(std::extent<int * [], std::rank<int * []>::value>() == 0, "");
    static_assert(std::is_same<int *, std::remove_extent_t<int * []>>(), "");

    static_assert(std::rank<int * [][3]>() == 2, "");
    static_assert(std::extent<int * [][3], std::rank<int * [][3]>::value>() == 0, "");
    static_assert(std::extent<int * [][3], 1>() == 3, "");
    static_assert(std::extent<int * [][3], 2>() == 0, "");
    static_assert(std::is_same<int * [3], std::remove_extent_t<int * [][3]>>(), "");

    REQUIRE("int[]" == type_desc<int []>().to_string());
    REQUIRE("int[3]" == type_desc<int [3]>().to_string());
    REQUIRE("int *[]" == type_desc<int * []>().to_string());
    REQUIRE("int *[][3]" == type_desc<int * [][3]>().to_string());
    REQUIRE("int *[][3][4]" == type_desc<int * [][3][4]>().to_string());
}


////////////////////////////////////////////////////////////////////////////////
template <typename Derived>
struct type_eng_desc_base
{
    operator std::string ()
    {
        return static_cast<Derived *>(this)->to_string();
    }
};


template <typename T>
struct type_eng_desc : type_eng_desc_base<type_eng_desc<T>>
{
    std::string to_string() const
    {
        return name_of<T>::value;
    }
};

template <typename T>
struct type_eng_desc<T const> : type_eng_desc_base<type_eng_desc<T const>>
{
    std::string to_string() const
    {
        return "const " + type_eng_desc<T>().to_string();
    }
};

template <typename T>
struct type_eng_desc<T volatile> : type_eng_desc_base<type_eng_desc<T volatile>>
{
    std::string to_string() const
    {
        return "volatile " + type_eng_desc<T>().to_string();
    }
};

template <typename T>
struct type_eng_desc<T *> : type_eng_desc_base<type_eng_desc<T *>>
{
    std::string to_string() const
    {
        return "pointer to " + type_eng_desc<T>().to_string();
    }
};

template <typename T>
struct type_eng_desc<T &> : type_eng_desc_base<type_eng_desc<T &>>
{
    std::string to_string() const
    {
        return "reference to " + type_eng_desc<T>().to_string();
    }
};

template <typename T>
struct type_eng_desc<T &&> : type_eng_desc_base<type_eng_desc<T &&>>
{
    std::string to_string() const
    {
        return "rvalue reference to " + type_eng_desc<T>().to_string();
    }
};

template <typename T>
struct type_eng_desc<T[]> : type_eng_desc_base<type_eng_desc<T[]>>
{
    std::string to_string() const
    {
        return "array of " + type_eng_desc<std::remove_all_extents_t<T>>().to_string();
    }
};

template <typename T, std::size_t N>
struct type_eng_desc<T[N]> : type_eng_desc_base<type_eng_desc<T[N]>>
{
    std::string to_string() const
    {
        return "array of " + type_eng_desc<std::remove_all_extents_t<T>>().to_string();
    }
};

template <typename R, typename... Args>
struct type_eng_desc<R(Args...)> : type_eng_desc_base<R(Args...)>
{
    std::string to_string() const
    {
        return "function returning " + type_eng_desc<R>().to_string();
    }
};

template <typename R, typename... Args>
struct type_eng_desc<R (&) (Args...)> : type_eng_desc_base<R (&) (Args...)>
{
    std::string to_string() const
    {
        return "reference to function returning " + type_eng_desc<R>().to_string();
    }
};

template <typename R, typename... Args>
struct type_eng_desc<R (*) (Args...)> : type_eng_desc_base<R (*) (Args...)>
{
    std::string to_string() const
    {
        return "pointer to function returning " + type_eng_desc<R>().to_string();
    }
};


TEST_CASE("2-5", "[tmp]")
{
    REQUIRE("array of pointer to function returning pointer to char" == type_eng_desc<char * (* [])()>().to_string());
}

