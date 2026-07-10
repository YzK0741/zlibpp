//
// Created by 小叶 on 2026/7/10.
//

#ifndef ZLIBPP_DECOMPRESS_H
#define ZLIBPP_DECOMPRESS_H
#include <memory>
#include <span>
#include <zlib.h>

#include "concepts.h"
#include "err.h"

namespace zlibpp {
    template <typename T>
    [[nodiscard]] std::expected<unsigned long, err>
    uncompress(std::span<T> source, std::span<T> dest) {
        unsigned long len = dest.size_bytes();

        const auto source_start = reinterpret_cast<uintptr_t>(source.data());
        const uintptr_t source_end = source_start + source.size_bytes();
        const auto dest_start = reinterpret_cast<uintptr_t>(dest.data());
        const uintptr_t dest_end = dest_start + dest.size_bytes();

        if (!(source_end < dest_start || dest_end < source_start)) {
            auto source_copy = std::make_unique<unsigned char[]>(source.size_bytes());
            memcpy(source_copy.get(), source.data(), source.size_bytes());
            if (const int err = ::uncompress(
                reinterpret_cast<Bytef *>(dest.data()),
                &len,
                reinterpret_cast<Bytef *>(source_copy.get()),
                static_cast<uLong>(source.size_bytes())
            ); err != Z_OK) {
                return std::unexpected(to_err_enum(err));
            }
            return {len};
        } else {
            if (int err = ::uncompress(
                reinterpret_cast<Bytef *>(dest.data()),
                &len,
                reinterpret_cast<Bytef *>(source.data()),
                static_cast<uLong>(source.size_bytes())
            ); err != Z_OK) {
                return std::unexpected(to_err_enum(err));
            }
            return {len};
        }
    }

    template <has_data_and_size source_container, has_data_and_size dest_container>
    [[nodiscard]] std::expected<unsigned long, err>
    uncompress (source_container&& source, dest_container&& dest) {
        return uncompress(std::span{source}, std::span{dest});
    }

    template <typename T>
    [[nodiscard]] std::expected<unsigned long, err>
    uncompress(T* source, T* dest, std::size_t source_len, std::size_t dest_len = 0) {
        if (source == nullptr  || source_len == 0) {
            return std::unexpected(err::data_error);
        }

        if (dest == nullptr) {
            return std::unexpected(err::buffer_error);
        }

        if (dest_len == 0) {
            dest_len = source_len;
        }

        return uncompress(std::span(source, source_len), std::span(dest, dest_len));
    }

    template <std_strong_smart_ptr source_ptr, std_strong_smart_ptr dest_ptr>
    [[nodiscard]] std::expected<unsigned long, err>
    uncompress(
        const source_ptr& source,
        const dest_ptr& dest,
        std::size_t source_len,
        std::size_t dest_len = 0
        ) {
        return uncompress(source.get(), dest.get(), source_len, dest_len);
    }

    template <std_strong_smart_ptr source_ptr, typename T>
    [[nodiscard]] std::expected<unsigned long, err>
    uncompress(
        const source_ptr& source,
        const std::weak_ptr<T>& dest,
        std::size_t source_len,
        std::size_t dest_len = 0
        ) {
        return uncompress(source, dest.lock(), source_len, dest_len);
    }

    template <std_strong_smart_ptr dest_ptr, typename T>
    [[nodiscard]] std::expected<unsigned long, err>
    uncompress(
        const std::weak_ptr<T>& source,
        const dest_ptr& dest,
        std::size_t source_len,
        std::size_t dest_len = 0
        ) {
        return uncompress(source.lock(), dest, source_len, dest_len);
    }

    template <typename T1, typename T2>
    [[nodiscard]] std::expected<unsigned long, err>
    uncompress(
        const std::weak_ptr<T1>& source,
        const std::weak_ptr<T2>& dest,
        std::size_t source_len,
        std::size_t dest_len = 0
        ) {
        return uncompress(source.lock(), dest.lock(), source_len, dest_len);
    }

    template <typename T>
    [[nodiscard]] std::expected<std::unique_ptr<unsigned char[]>, err>
    uncompress(std::span<T> source, std::size_t expected_dest_size = 0) {
        if (!expected_dest_size) {
            expected_dest_size = source.size_bytes() * 5;
        }
        const auto dest_temp = std::make_unique<unsigned char[]>(expected_dest_size);

        if (auto uncompress_result = uncompress(source, std::span(dest_temp.get(), expected_dest_size));
            uncompress_result.has_value()) {
            auto dest = std::make_unique<unsigned char[]>(*uncompress_result);
            memcpy(dest.get(), dest_temp.get(), *uncompress_result);
            return {dest};
        } else {
            return std::unexpected(uncompress_result.error());
        }
    }

    template <has_data_and_size container>
    [[nodiscard]] std::expected<std::unique_ptr<unsigned char[]>, err>
    uncompress(container&& source, std::size_t expected_dest_size = 0) {
        return uncompress(std::span{source.data(), source.size()}, expected_dest_size);
    }

    template <typename T>
    [[nodiscard]] std::expected<std::unique_ptr<unsigned char[]>, err>
    uncompress(T* source, std::size_t source_len, std::size_t expected_dest_size = 0) {
        return uncompress(std::span(source, source_len), expected_dest_size);
    }

    template <std_strong_smart_ptr source_ptr>
    [[nodiscard]] std::expected<std::unique_ptr<unsigned char[]>, err>
    uncompress(const source_ptr& source, std::size_t source_len, std::size_t expected_dest_size = 0) {
        return uncompress(source.get(), source_len, expected_dest_size);
    }

    template <typename T>
    [[nodiscard]] std::expected<std::unique_ptr<unsigned char[]>, err>
    uncompress(const std::weak_ptr<T>& source, std::size_t source_len, std::size_t expected_dest_size = 0) {
        return uncompress(source.lock(), source_len, expected_dest_size);
    }

}
#endif //ZLIBPP_DECOMPRESS_H
