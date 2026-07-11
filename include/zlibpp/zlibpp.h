//
// Created by 小叶 on 2026/7/9.
//

#ifndef ZLIBPP_ZLIBPP_H
#define ZLIBPP_ZLIBPP_H

/**
 *@brief zlibpp is zlib's modern cpp interface
 */
#include <zlib.h>
#if ZLIB_VERNUM < 0x1290
#error "zlib version 1.2.9 or higher required"
#endif

#include "compress.h"
#include "uncompress.h"
#include "stream_compress.h"
#include "stream_uncompress.h"
#include "flush.h"

#endif //ZLIBPP_ZLIBPP_H
