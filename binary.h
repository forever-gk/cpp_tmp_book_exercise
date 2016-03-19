#ifndef CPP_TMP_BOOK_EXERCISE_BINARY_H
#define CPP_TMP_BOOK_EXERCISE_BINARY_H


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


#endif //CPP_TMP_BOOK_EXERCISE_BINARY_H

