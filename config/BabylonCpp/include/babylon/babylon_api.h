
#ifndef BABYLON_SHARED_EXPORT_H
#define BABYLON_SHARED_EXPORT_H

#ifdef BABYLONCPP_STATIC_DEFINE
#  define BABYLON_SHARED_EXPORT
#  define BABYLONCPP_NO_EXPORT
#else
#  ifndef BABYLON_SHARED_EXPORT
#    ifdef BabylonCpp_EXPORTS
        /* We are building this library */
#      define BABYLON_SHARED_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define BABYLON_SHARED_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef BABYLONCPP_NO_EXPORT
#    define BABYLONCPP_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef BABYLONCPP_DEPRECATED
#  define BABYLONCPP_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef BABYLONCPP_DEPRECATED_EXPORT
#  define BABYLONCPP_DEPRECATED_EXPORT BABYLON_SHARED_EXPORT BABYLONCPP_DEPRECATED
#endif

#ifndef BABYLONCPP_DEPRECATED_NO_EXPORT
#  define BABYLONCPP_DEPRECATED_NO_EXPORT BABYLONCPP_NO_EXPORT BABYLONCPP_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef BABYLONCPP_NO_DEPRECATED
#    define BABYLONCPP_NO_DEPRECATED
#  endif
#endif

#endif /* BABYLON_SHARED_EXPORT_H */
