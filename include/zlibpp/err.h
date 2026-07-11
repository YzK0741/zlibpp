//
// Created by 小叶 on 2026/7/10.
//

#ifndef ZLIBPP_ERRNO_H
#define ZLIBPP_ERRNO_H

#include <zlib.h>

namespace zlibpp {
    enum class err {
        ok,
        stream_end,
        system_error,
        buffer_error,
        memory_error,
        data_error,
        version_error,
        need_dict,
        invalid_argument,
        invalid_condition,
        unknown_error,
    };

    constexpr err to_err_enum(const int err) noexcept {
        switch (err) {
            case Z_OK: return err::ok;
            case Z_STREAM_END: return err::stream_end;
            case Z_ERRNO: return err::system_error;
            case Z_BUF_ERROR: return err::buffer_error;
            case Z_MEM_ERROR: return err::memory_error;
            case Z_DATA_ERROR: return err::data_error;
            case Z_VERSION_ERROR: return err::version_error;
            case Z_NEED_DICT: return err::need_dict;
            default: return err::unknown_error;
        }
    }

    constexpr int from_err_enum(const err err) noexcept {
        switch (err) {
            case err::ok: return Z_OK;
            case err::stream_end: return Z_STREAM_END;
            case err::system_error: return Z_ERRNO;
            case err::buffer_error: return  Z_BUF_ERROR;
            case err::memory_error: return Z_MEM_ERROR;
            case err::data_error: return Z_DATA_ERROR;
            case err::version_error: return Z_VERSION_ERROR;
            case err::need_dict: return Z_NEED_DICT;
            default: return Z_ERRNO;
        }
    }

    inline const char* get_description(const int err) noexcept {
        return zError(err);
    }

    inline const char* get_description(const err err) noexcept {
        return get_description(from_err_enum(err));
    }

}

#endif //ZLIBPP_ERRNO_H
