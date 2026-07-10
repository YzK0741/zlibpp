//
// Created by 小叶 on 2026/7/9.
//

#ifndef ZLIBPP_COMPRESS_H
#define ZLIBPP_COMPRESS_H


#include <expected>
#include <span>
#include <memory>
#include <zlib.h>

#include "../concepts.h"
#include "err.h"



namespace zlibpp {

    inline std::size_t compress_bound(const std::size_t size) {
        return ::compressBound(size);
    }

    template<typename T>
    [[nodiscard]] std::expected<unsigned long, err>
    compress(std::span<T> source, std::span<T> dest) {
        if (source.empty() || dest.empty()) {
            return std::unexpected(err::data_error);
        }

        unsigned long len = dest.size_bytes();

        const auto source_start = reinterpret_cast<uintptr_t>(source.data());
        const uintptr_t source_end = source_start + source.size_bytes();
        const auto dest_start = reinterpret_cast<uintptr_t>(dest.data());
        const uintptr_t dest_end = dest_start + dest.size_bytes();

        if (!(source_end < dest_start || dest_end < source_start)) {

            auto source_copy = std::make_unique<unsigned char[]>(source.size_bytes());
            memcpy(source_copy.get(), source.data(), source.size_bytes());
            if (const auto err = ::compress(
                reinterpret_cast<Bytef *>(dest.data()),
                &len,
                reinterpret_cast<Bytef *>(source_copy.get()),
                static_cast<uLong>(source.size_bytes())
                ); err != Z_OK ) {
                return std::unexpected(to_errno_enum(err));
            }
        } else {
            if (const auto err = ::compress(
                reinterpret_cast<Bytef *>(dest.data()),
                &len,
                reinterpret_cast<Bytef *>(source.data()),
                static_cast<uLong>(source.size_bytes())
                ); err != Z_OK) {
                return std::unexpected(to_errno_enum(err));
            }
        }
        return {len};
    }


    template <has_data_and_size source_container, has_data_and_size dest_container>
    [[nodiscard]] std::expected<unsigned long, err>
    compress (source_container&& source, dest_container&& dest) {
        return compress(std::span{source}, std::span{dest});
    }

    template <typename T>
    [[nodiscard]] std::expected<unsigned long, err>
    compress(
        T* source,
        T* dest,
        std::size_t source_len,
        std::size_t dest_len = 0
        ) {
        if (source == nullptr || source_len < 1) {
            return std::unexpected(err::data_error);
        }

        if (dest == nullptr) {
            return std::unexpected(err::buffer_error);
        }

        if (dest_len == 0) {
            dest_len = source_len;
        }

        return compress(std::span(source, source_len), std::span(dest, dest_len));
    }

    template <std_strong_smart_ptr source_ptr, std_strong_smart_ptr dest_ptr>
    [[nodiscard]] std::expected<unsigned long, err>
    compress(
        const source_ptr& source,
        const dest_ptr& destination,
        std::size_t source_len,
        std::size_t dest_len = 0
        ) {
        return compress(source.get(), destination.get(), source_len, dest_len);
    }

    template <typename T, std_strong_smart_ptr strong_ptr>
    requires std::is_same_v<T, typename strong_ptr::element_type>
    [[nodiscard]] std::expected<unsigned long, err>
    compress(
        const std::weak_ptr<T>& source,
        const strong_ptr& dest,
        std::size_t source_len,
        std::size_t dest_len = 0
        ) {
        return compress(source.lock(), dest, source_len, dest_len);
    }

    template <typename T, std_strong_smart_ptr strong_ptr>
    requires std::is_same_v<T, typename strong_ptr::element_type>
    [[nodiscard]] std::expected<unsigned long, err>
    compress(
        const strong_ptr& source,
        const std::weak_ptr<T>& dest,
        std::size_t source_len,
        std::size_t dest_len = 0
        ) {
        return compress(source, dest.lock(),source_len, dest_len);
    }

    template <typename T1, typename T2>
    [[nodiscard]] std::expected<unsigned long, err>
    compress(
        const std::weak_ptr<T1>& source,
        const std::weak_ptr<T2>& dest,
        std::size_t source_len,
        std::size_t dest_len = 0
        ) {
        return compress(source.lock(), dest.lock(), source_len, dest_len);
    }

    template <typename T>
    [[nodiscard]] std::expected<std::unique_ptr<unsigned char[]>, err>
    compress(std::span<const T> source) {
        const std::size_t expected_size = compress_bound(source.size_bytes());
        const auto result_temp = std::make_unique<unsigned char[]>(expected_size);
        if (auto compress_result = compress(source, std::span(result_temp.get(), expected_size));
            compress_result.has_value()) {
            auto result = std::make_unique<unsigned char[]>(compress_result.value());
            memcpy(result.get(), result_temp.get(), compress_result.value());
            return {result};
        } else {
            return std::unexpected(compress_result.error());
        }
    }

    template <has_data_and_size container>
    [[nodiscard]] std::expected<std::unique_ptr<unsigned char[]>, err>
    compress(container&& source) {
        return compress(std::span{source.data(), source.size()});
    }

    template <typename T>
    [[nodiscard]] std::expected<std::unique_ptr<unsigned char[]>, err>
    compress(const T* source, std::size_t source_len) {
        return compress(std::span(source, source_len));
    }

    template <std_strong_smart_ptr source_ptr>
    [[nodiscard]] std::expected<std::unique_ptr<unsigned char[]>, err>
    compress(const source_ptr& source, std::size_t source_len) {
        return compress(source.get(), source_len);
    }

    template <typename T>
    [[nodiscard]] std::expected<std::unique_ptr<unsigned char[]>, err>
    compress(const std::weak_ptr<T>& source, std::size_t source_len) {
        return compress(source.lock(), source_len);
    }
}

#endif //ZLIBPP_COMPRESS_H
