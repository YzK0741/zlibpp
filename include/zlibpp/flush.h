//
// Created by 小叶 on 2026/7/11.
//

#ifndef ZLIBPP_FLUSH_H
#define ZLIBPP_FLUSH_H

#include <zlib.h>

namespace zlibpp {
    enum class flush_mode {
        no_flush = Z_NO_FLUSH,
        sync_flush = Z_SYNC_FLUSH,
        full_flush = Z_FULL_FLUSH,
        finish = Z_FINISH,
    };

    [[nodiscard]] inline int to_z_flush(const flush_mode flush_mode) {
        switch (flush_mode) {
            case flush_mode::no_flush: return Z_NO_FLUSH;
            case flush_mode::sync_flush: return Z_SYNC_FLUSH;
            case flush_mode::full_flush: return Z_FULL_FLUSH;
            case flush_mode::finish: return Z_FINISH;
        }
        return Z_NO_FLUSH;
    }
}

#endif //ZLIBPP_FLUSH_H
