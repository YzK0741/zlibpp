//
// Created by 小叶 on 2026/7/11.
//

#ifndef ZLIBPP_STREAM_COMPRESS_H
#define ZLIBPP_STREAM_COMPRESS_H

#include <expected>
#include <span>
#include <zlib.h>
#include <cassert>
#include "concepts.h"
#include "err.h"
#include "flush.h"

namespace zlibpp {
    class compressor {
        z_stream strm = {};
        bool in_compress = false;
    public:
        explicit compressor(const int level =  Z_DEFAULT_COMPRESSION) noexcept {
            const int code = deflateInit(&this->strm, level);
            assert(code == Z_OK && "deflateInit failed");
        };

        template <typename T>
        std::expected<void, err> set_dest(std::span<T> dest) noexcept {
            if (dest.data() == nullptr || dest.size_bytes() == 0) {
                return std::unexpected(err::invalid_argument);
            }
            this->strm.next_out = reinterpret_cast<Bytef *>(dest.data());
            this->strm.avail_out = static_cast<uInt>(dest.size_bytes());
            this->in_compress = true;
            return {};
        }

        template <has_data_and_size container>
        std::expected<void, err> set_dest(container dest) noexcept {
            return this->set_dest(std::span(dest.data(), dest.size()));
        }

        template <typename T>
        std::expected<void, err> set_dest(T* dest, std::size_t dest_size) noexcept {
            return this->set_dest(std::span(dest, dest_size));
        }

        template <std_strong_smart_ptr source_ptr>
        std::expected<void, err> set_dest(source_ptr dest, const std::size_t dest_size) noexcept {
            return this->set_dest(dest.get(), dest_size);
        }

        template <typename T>
        std::expected<void, err> set_dest(std::weak_ptr<T> dest, std::size_t dest_size) noexcept {
            return this->set_dest(dest.lock(), dest_size);
        }

        template <typename T>
        std::expected<void, err> compress(std::span<T> source, const flush_mode flush = flush_mode::no_flush) noexcept {
            if (source.data() == nullptr || source.size_bytes() == 0) {
                return std::unexpected(err::invalid_argument);
            }

            this->strm.next_in = reinterpret_cast<Bytef *>(source.data());
            this->strm.avail_in = static_cast<uInt>(source.size_bytes());
            const int code = deflate(&this->strm, to_z_flush(flush));
            if (code == Z_OK || code == Z_STREAM_END) {
                return {};
            }
            return std::unexpected(to_err_enum(code));
        }

        template <typename T>
        std::expected<void, err> compress(T* source, std::size_t source_size, const flush_mode flush = flush_mode::no_flush) noexcept {
            return this->compress(std::span(source, source_size), flush);
        }

        template <std_strong_smart_ptr source_ptr>
        std::expected<void, err> compress(source_ptr source, std::size_t source_size, const flush_mode flush = flush_mode::no_flush) noexcept {
            return this->compress(std::span(source.get(), source_size), flush);
        }

        template <typename T>
        std::expected<void, err> compress(std::weak_ptr<T> source, std::size_t source_size, const flush_mode flush = flush_mode::no_flush) noexcept {
            return this->compress(std::span(source.lock(), source_size), flush);
        }

        std::expected<void, err> end_compress() noexcept {
            const int code = deflate(&strm, Z_FINISH);
            this->in_compress = false;
            if (code == Z_OK || code == Z_STREAM_END) {
                return {};
            }
            return std::unexpected(to_err_enum(code));
        }

        std::expected<void, err> reset() noexcept {
            if (!this->in_compress) {
                deflateReset(&this->strm);
                return {};
            }
            return std::unexpected(err::invalid_condition);
        }

        void force_reset() {
            deflateReset(&this->strm);
            this->in_compress = false;
        }

        [[nodiscard]] std::size_t bytes_written() const noexcept {
            return strm.total_out;
        }

        [[nodiscard]] std::size_t bytes_read() const noexcept {
            return strm.total_in;
        }

        ~compressor() noexcept {
            deflateEnd(&this->strm);
        };
    };
}

#endif //ZLIBPP_STREAM_COMPRESS_H
