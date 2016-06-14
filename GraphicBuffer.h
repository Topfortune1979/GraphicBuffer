#pragma once

#include "DynamicLibrary.h"
#include <cstdint>
#include <cerrno>

struct ANativeWindowBuffer;

namespace android
{
    class GraphicBuffer;

    // include/system/window.h
    struct android_native_base_t
    {
        uint32_t magic;
        uint32_t version;
        void* reserved[4];
        void (*incRef)(struct android_native_base_t* base);
        void (*decRef)(struct android_native_base_t* base);
    };

    // include/ui/android_native_buffer.h
    struct android_native_buffer_t
    {
        struct android_native_base_t common;
        int32_t width;
        int32_t height;
        int32_t stride;
        int32_t format;
        int32_t usage;
        // ...
    };
}

// utils/Errors.h
enum status_t
{
    OK					= 0,
    UNKNOWN_ERROR		= (-2147483647-1),
    NO_MEMORY			= -ENOMEM,
    INVALID_OPERATION	= -ENOSYS,
    BAD_VALUE			= -EINVAL,
    BAD_TYPE			= (UNKNOWN_ERROR + 1),
    NAME_NOT_FOUND		= -ENOENT,
    PERMISSION_DENIED	= -EPERM,
    NO_INIT				= -ENODEV,
    ALREADY_EXISTS		= -EEXIST,
    DEAD_OBJECT			= -EPIPE,
    FAILED_TRANSACTION	= (UNKNOWN_ERROR + 2),
    BAD_INDEX			= -E2BIG,
    NOT_ENOUGH_DATA		= (UNKNOWN_ERROR + 3),
    WOULD_BLOCK			= (UNKNOWN_ERROR + 4),
    TIMED_OUT			= (UNKNOWN_ERROR + 5),
    UNKNOWN_TRANSACTION = (UNKNOWN_ERROR + 6),
    FDS_NOT_ALLOWED		= (UNKNOWN_ERROR + 7),
};

// ui/PixelFormat.h, system/graphics.h
enum PixelFormat
{
    PIXEL_FORMAT_UNKNOWN		= 0,
    PIXEL_FORMAT_NONE			= 0,
    PIXEL_FORMAT_CUSTOM			= -4,
    PIXEL_FORMAT_TRANSLUCENT	= -3,
    PIXEL_FORMAT_TRANSPARENT	= -2,
    PIXEL_FORMAT_OPAQUE			= -1,
    PIXEL_FORMAT_RGBA_8888		= 1,
    PIXEL_FORMAT_RGBX_8888		= 2,
    PIXEL_FORMAT_RGB_888		= 3,
    PIXEL_FORMAT_RGB_565		= 4,
    PIXEL_FORMAT_BGRA_8888		= 5,
    PIXEL_FORMAT_RGBA_5551		= 6,
    PIXEL_FORMAT_RGBA_4444		= 7,
};

// ui/GraphicBuffer.h
struct GraphicBufferFunctions
{
    typedef void					(*genericFunc)			();
    typedef status_t				(*initCheckFunc)		(android::GraphicBuffer* buffer);
    typedef status_t				(*lockFunc)				(android::GraphicBuffer* buffer, uint32_t usage, void** vaddr);
    typedef status_t				(*unlockFunc)			(android::GraphicBuffer* buffer);
    typedef ANativeWindowBuffer*	(*getNativeBufferFunc)	(const android::GraphicBuffer* buffer);

    genericFunc						constructor;
    genericFunc						destructor;
    lockFunc						lock;
    unlockFunc						unlock;
    getNativeBufferFunc				getNativeBuffer;
    initCheckFunc					initCheck;
};

class GraphicBuffer
{
public:
    // ui/GraphicBuffer.h, hardware/gralloc.h
    enum {
        USAGE_SW_READ_NEVER		= 0x00000000,
        USAGE_SW_READ_RARELY	= 0x00000002,
        USAGE_SW_READ_OFTEN		= 0x00000003,
        USAGE_SW_READ_MASK		= 0x0000000f,

        USAGE_SW_WRITE_NEVER	= 0x00000000,
        USAGE_SW_WRITE_RARELY	= 0x00000020,
        USAGE_SW_WRITE_OFTEN	= 0x00000030,
        USAGE_SW_WRITE_MASK		= 0x000000f0,

        USAGE_SOFTWARE_MASK		= USAGE_SW_READ_MASK | USAGE_SW_WRITE_MASK,

        USAGE_PROTECTED			= 0x00004000,

        USAGE_HW_TEXTURE		= 0x00000100,
        USAGE_HW_RENDER			= 0x00000200,
        USAGE_HW_2D				= 0x00000400,
        USAGE_HW_COMPOSER		= 0x00000800,
        USAGE_HW_VIDEO_ENCODER	= 0x00010000,
        USAGE_HW_MASK			= 0x00071F00,
    };

    GraphicBuffer(uint32_t width, uint32_t height, PixelFormat format, uint32_t usage);
    ~GraphicBuffer();

    status_t lock(uint32_t usage, void** vaddr);
    status_t unlock();
    ANativeWindowBuffer *getNativeBuffer() const;
    uint32_t getStride() const;

private:
    DynamicLibrary library;
    GraphicBufferFunctions functions;
    android::GraphicBuffer *impl = nullptr;
};

