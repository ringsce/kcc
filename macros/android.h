#ifndef ANDROID_H
#define ANDROID_H

#ifdef __cplusplus
extern "C" {
#endif

// -------------------------------
// Basic typedefs
// -------------------------------
typedef signed char         int8_t;
typedef unsigned char       uint8_t;
typedef short               int16_t;
typedef unsigned short      uint16_t;
typedef int                 int32_t;
typedef unsigned int        uint32_t;
typedef long long           int64_t;
typedef unsigned long long  uint64_t;

typedef int32_t             pid_t;
typedef uint32_t            uid_t;
typedef uint32_t            gid_t;

// -------------------------------
// Android Logging (android/log.h)
// -------------------------------
enum {
    ANDROID_LOG_UNKNOWN = 0,
    ANDROID_LOG_DEFAULT,
    ANDROID_LOG_VERBOSE,
    ANDROID_LOG_DEBUG,
    ANDROID_LOG_INFO,
    ANDROID_LOG_WARN,
    ANDROID_LOG_ERROR,
    ANDROID_LOG_FATAL,
    ANDROID_LOG_SILENT,
};

// Stub logging API (replace with real libandroid at link time)
int __android_log_print(int prio, const char* tag, const char* fmt, ...);

static inline int __android_log_print(int prio, const char* tag, const char* fmt, ...) {
    (void)prio; (void)tag; (void)fmt;
    return 0; // no-op if not linked with libandroid
}

// -------------------------------
// Android Native Window (android/native_window.h)
// -------------------------------
struct ANativeWindow;
struct ANativeWindow_Buffer {
    void* bits;
    int   width;
    int   height;
    int   stride;
    int   format;
    int   reserved[4];
};

// Pixel format enums
enum {
    WINDOW_FORMAT_RGBA_8888   = 1,
    WINDOW_FORMAT_RGBX_8888   = 2,
    WINDOW_FORMAT_RGB_565     = 4,
};

// Window transform enums
enum {
    WINDOW_TRANSFORM_IDENTITY = 0x00,
    WINDOW_TRANSFORM_MIRROR_HORIZONTAL = 0x01,
    WINDOW_TRANSFORM_MIRROR_VERTICAL   = 0x02,
    WINDOW_TRANSFORM_ROTATE_90         = 0x04,
    WINDOW_TRANSFORM_ROTATE_180        = 0x03,
    WINDOW_TRANSFORM_ROTATE_270        = 0x07,
};

// -------------------------------
// Native Activity (android/native_activity.h)
// -------------------------------
struct ANativeActivity {
    void* instance;
    struct ANativeWindow* window;
    struct AAssetManager* assetManager;
    const char* internalDataPath;
    const char* externalDataPath;
    int sdkVersion;
};

// Input queue placeholder
struct AInputQueue;

// -------------------------------
// Asset Manager (android/asset_manager.h)
// -------------------------------
struct AAssetManager;
struct AAsset;

enum {
    AASSET_MODE_UNKNOWN = 0,
    AASSET_MODE_RANDOM  = 1,
    AASSET_MODE_STREAMING = 2,
    AASSET_MODE_BUFFER = 3
};

// -------------------------------
// ALooper (android/looper.h)
// -------------------------------
struct ALooper;

// -------------------------------
// EGL / OpenGL ES basics
// (Minimal subset to build against)
// -------------------------------
typedef int EGLint;
typedef unsigned int EGLBoolean;
typedef void* EGLDisplay;
typedef void* EGLSurface;
typedef void* EGLContext;
typedef void* EGLConfig;

#define EGL_FALSE 0
#define EGL_TRUE  1

#define EGL_DEFAULT_DISPLAY ((EGLDisplay)0)
#define EGL_NO_DISPLAY ((EGLDisplay)0)
#define EGL_NO_CONTEXT ((EGLContext)0)
#define EGL_NO_SURFACE ((EGLSurface)0)

#define EGL_RED_SIZE               0x3024
#define EGL_GREEN_SIZE             0x3023
#define EGL_BLUE_SIZE              0x3022
#define EGL_ALPHA_SIZE             0x3021
#define EGL_DEPTH_SIZE             0x3025
#define EGL_STENCIL_SIZE           0x3026
#define EGL_NONE                   0x3038
#define EGL_RENDERABLE_TYPE        0x3040
#define EGL_OPENGL_ES2_BIT         0x0004

// -------------------------------
// OpenGL ES 2.0 basics
// -------------------------------
typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef void GLvoid;
typedef signed char GLbyte;
typedef short GLshort;
typedef int GLint;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef int GLsizei;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;

// Common GL enums
#define GL_COLOR_BUFFER_BIT       0x00004000
#define GL_DEPTH_BUFFER_BIT       0x00000100
#define GL_STENCIL_BUFFER_BIT     0x00000400

#define GL_TRIANGLES              0x0004
#define GL_TRIANGLE_STRIP         0x0005
#define GL_TRIANGLE_FAN           0x0006

#define GL_FLOAT                  0x1406
#define GL_UNSIGNED_SHORT         0x1403
#define GL_UNSIGNED_INT           0x1405

#ifdef __cplusplus
}
#endif

#endif // ANDROID_H
