#ifndef __ZLIB_ZLIB_H__
#define __ZLIB_ZLIB_H__

typedef usz ZLIB_Status;
enum ZLIB_StatusCodes {
    ZLIB_STATUS_SUCCESS = 0,

    ZLIB_STATUS_COUNT,
#define ZLIB_STATUS_UNKNOWN ZLIB_STATUS_COUNT
};

ZLIB_Status ZLIB_DecompressBuffer(ZLIB_Decompressor *decomp, const void *in, usz inLen, void *out, usz outLen);
ZLIB_Status ZLIB_CompressBuffer(ZLIB_Compressor *comp, const void *in, usz outLen, void *out, usz outLen);


#endif /* __ZLIB_ZLIB_H__ */
