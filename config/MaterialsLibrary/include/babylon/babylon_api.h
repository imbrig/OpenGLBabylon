
#ifndef BABYLON_SHARED_EXPORT_H
#define BABYLON_SHARED_EXPORT_H

#ifdef MATERIALSLIBRARY_STATIC_DEFINE
#  define BABYLON_SHARED_EXPORT
#  define MATERIALSLIBRARY_NO_EXPORT
#else
#  ifndef BABYLON_SHARED_EXPORT
#    ifdef MaterialsLibrary_EXPORTS
        /* We are building this library */
#      define BABYLON_SHARED_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define BABYLON_SHARED_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef MATERIALSLIBRARY_NO_EXPORT
#    define MATERIALSLIBRARY_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef MATERIALSLIBRARY_DEPRECATED
#  define MATERIALSLIBRARY_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef MATERIALSLIBRARY_DEPRECATED_EXPORT
#  define MATERIALSLIBRARY_DEPRECATED_EXPORT BABYLON_SHARED_EXPORT MATERIALSLIBRARY_DEPRECATED
#endif

#ifndef MATERIALSLIBRARY_DEPRECATED_NO_EXPORT
#  define MATERIALSLIBRARY_DEPRECATED_NO_EXPORT MATERIALSLIBRARY_NO_EXPORT MATERIALSLIBRARY_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef MATERIALSLIBRARY_NO_DEPRECATED
#    define MATERIALSLIBRARY_NO_DEPRECATED
#  endif
#endif

#endif /* BABYLON_SHARED_EXPORT_H */
