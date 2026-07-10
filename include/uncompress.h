//
// Created by 小叶 on 2026/7/10.
//

#ifndef ZLIBPP_DECOMPRESS_H
#define ZLIBPP_DECOMPRESS_H
#include <memory>
#include <optional>
#include <span>
#include <zlib.h>

#include "concepts.h"

namespace zlibpp {
    template <typename T>
    [[nodiscard]] std::optional<unsigned long>
    uncompress(std::span<T> source, std::span<T> dest) {
        unsigned long len = dest.size_bytes();

        const auto source_start = reinterpret_cast<uintptr_t>(source.data());
        const uintptr_t source_end = source_start + source.size_bytes();
        const auto dest_start = reinterpret_cast<uintptr_t>(dest.data());
        const uintptr_t dest_end = dest_start + dest.size_bytes();

        if (!(source_end <= dest_start || dest_end <= source_start)) {
            auto source_copy = std::make_unique<unsigned char[]>(source.size_bytes());
            memcpy(source_copy.get(), source.data(), source.size_bytes());
            if (::uncompress(
                reinterpret_cast<Bytef *>(dest.data()),
                &len,
                reinterpret_cast<Bytef *>(source_copy.get()),
                static_cast<uLong>(source.size_bytes())
            ) != Z_OK) {
                return std::nullopt;
            }
            return {len};
        } else {
            if (::uncompress(
                reinterpret_cast<Bytef *>(dest.data()),
                &len,
                reinterpret_cast<Bytef *>(source.data()),
                static_cast<uLong>(source.size_bytes())
            ) != Z_OK) {
                return std::nullopt;
            }
            return {len};
        }
    }

    template <typename source_container, typename dest_container>
    [[nodiscard]] std::optional<unsigned long>
    uncompress (source_container&& source, dest_container&& dest) {
        return uncompress(std::span{source}, std::span{dest});
    }

    template <typename T>
    [[nodiscard]] std::optional<unsigned long>
    uncompress(T* source, T* dest, std::size_t source_len, std::size_t dest_len = 0) {
        if (source == nullptr || dest == nullptr || source_len == 0) {
            return std::nullopt;
        }

        if (dest_len == 0) {
            dest_len = source_len;
        }

        return uncompress(std::span(source, source_len), std::span(dest, dest_len));
    }

    template <std_strong_smart_ptr source_ptr, std_strong_smart_ptr dest_ptr>
    [[nodiscard]] std::optional<unsigned long>
    uncompress(
        const source_ptr& source,
        const dest_ptr& dest,
        std::size_t source_len,
        std::size_t dest_len = 0
        ) {
        return uncompress(source.get(), dest.get(), source_len, dest_len);
    }

    template <std_strong_smart_ptr source_ptr, typename T>
    [[nodiscard]] std::optional<unsigned long>
    uncompress(
        const source_ptr& source,
        const std::weak_ptr<T>& dest,
        std::size_t source_len,
        std::size_t dest_len = 0
        ) {
        return uncompress(source, dest.lock(), source_len, dest_len);
    }

    template <std_strong_smart_ptr dest_ptr, typename T>
    std::optional<unsigned long>
    uncompress(
        const std::weak_ptr<T>& source,
        const dest_ptr& dest,
        std::size_t source_len,
        std::size_t dest_len = 0
        ) {
        return uncompress(source.lock(), dest, source_len, dest_len);
    }

    template <typename T1, typename T2>
    [[nodiscard]] std::optional<unsigned long>
    uncompress(
        const std::weak_ptr<T1>& source,
        const std::weak_ptr<T2>& dest,
        std::size_t source_len,
        std::size_t dest_len = 0
        ) {
        return uncompress(source.lock(), dest.lock(), source_len, dest_len);
    }

    template <typename T>
    [[nodiscard]] std::optional<std::unique_ptr<unsigned char[]>>
    uncompress(std::span<T> source) {
        const std::size_t expected_size = source.size_bytes() * 2;
        const auto dest_temp = std::make_unique<unsigned char[]>(expected_size);

        if (auto uncompress_result = uncompress(source, std::span(dest_temp.get(), expected_size)); uncompress_result) {
            auto dest = std::make_unique<unsigned char[]>(*uncompress_result);
            memcpy(dest.get(), dest_temp.get(), *uncompress_result);
            return {dest};
        } else {
            return std::nullopt;
        }
    }

    template <typename container>
    [[nodiscard]] std::optional<std::unique_ptr<unsigned char[]>>
    uncompress(container&& source) {
        return uncompress(std::span{source.data(), source.size()});
    }

    template <typename T>
    [[nodiscard]] std::optional<std::unique_ptr<unsigned char[]>>
    uncompress(T* source, std::size_t source_len) {
        return uncompress(std::span(source, source_len));
    }

    template <std_strong_smart_ptr source_ptr>
    [[nodiscard]] std::optional<std::unique_ptr<unsigned char[]>>
    uncompress(const source_ptr& source, std::size_t source_len) {
        return uncompress(source.get(), source_len);
    }

    template <typename T>
    [[nodiscard]] std::optional<std::unique_ptr<unsigned char[]>>
    uncompress(const std::weak_ptr<T>& source, std::size_t source_len) {
        return uncompress(source.lock(), source_len);
    }

}
#endif //ZLIBPP_DECOMPRESS_H
