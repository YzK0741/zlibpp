//
// Created by 小叶 on 2026/7/10.
//

#ifndef ZLIBPP_CONCEPTS_H
#define ZLIBPP_CONCEPTS_H

#include <memory>

namespace zlibpp {
    template<typename T>
    concept std_strong_smart_ptr = requires {
        typename T::element_type;
    } && (std::is_same_v<T, std::unique_ptr<typename T::element_type>> ||
           std::is_same_v<T, std::shared_ptr<typename T::element_type>>);

    template <typename T>
    concept has_data_and_size = requires(T t)
    {
        t.data();
        t.size();
    };
}

#endif //ZLIBPP_CONCEPTS_H
