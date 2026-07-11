//
// Created by 小叶 on 2026/7/10.
//

#ifndef ZLIBPP_CONCEPTS_H
#define ZLIBPP_CONCEPTS_H

#include <memory>

namespace zlibpp {
    template<typename T>
struct is_std_unique_ptr : std::false_type {};

    template<typename T, typename Deleter>
    struct is_std_unique_ptr<std::unique_ptr<T, Deleter>> : std::true_type {};

    template<typename T>
    struct is_std_shared_ptr : std::false_type {};

    template<typename T>
    struct is_std_shared_ptr<std::shared_ptr<T>> : std::true_type {};

    template<typename T>
    concept std_strong_smart_ptr =
        requires {
        typename T::element_type;
        } && (is_std_unique_ptr<T>::value || is_std_shared_ptr<T>::value);

    template <typename T>
    concept has_data_and_size = requires(T t)
    {
        t.data();
        t.size();
    };
}

#endif //ZLIBPP_CONCEPTS_H
