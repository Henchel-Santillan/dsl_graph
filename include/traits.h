#ifndef DS_GRAPH_TRAITS_H
#define DS_GRAPH_TRAITS_H


#include <concepts>


namespace dsl::nonlinear {

    template <typename Tp>
    concept Comparable = requires (Tp a, Tp b)
    {
        { a == b } -> std::same_as<bool>;
        { a != b } -> std::same_as<bool>;
        { a >= b } -> std::same_as<bool>;
        { a >  b } -> std::same_as<bool>;
        { a <= b } -> std::same_as<bool>;
        { a <  b } -> std::same_as<bool>;

    };  // concept Comparable

    static constexpr const int default_capacity = 16;

}   // namespace nonlinear

#endif //DS_GRAPH_TRAITS_H
