#pragma once
#if defined(_MSC_VER)
// Windows
    #if defined(LAPLACIAN_PYRAMID_IMPORT)
        #define EXPORT_LAPLACIAN_PYRAMID __declspec(dllimport)
    #else
        #define EXPORT_LAPLACIAN_PYRAMID __declspec(dllexport)
    #endif
#elif defined(__GNUC__)
//  GCC
    #define EXPORT_LAPLACIAN_PYRAMID __attribute__((visibility("default")))
#else
    //  do nothing and hope for the best?
    #define EXPORT_LAPLACIAN_PYRAMID
    #pragma warning Unknown dynamic link import/export semantics.
#endif