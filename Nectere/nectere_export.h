
#ifndef NECTERE_EXPORT_H
#define NECTERE_EXPORT_H

#ifdef NECTERE_STATIC_DEFINE
#  define NECTERE_EXPORT
#  define NECTERE_NO_EXPORT
#else
#  ifndef NECTERE_EXPORT
#    ifdef Nectere_EXPORTS
        /* We are building this library */
#      define NECTERE_EXPORT 
#    else
        /* We are using this library */
#      define NECTERE_EXPORT 
#    endif
#  endif

#  ifndef NECTERE_NO_EXPORT
#    define NECTERE_NO_EXPORT 
#  endif
#endif

#ifndef NECTERE_DEPRECATED
#  define NECTERE_DEPRECATED __declspec(deprecated)
#endif

#ifndef NECTERE_DEPRECATED_EXPORT
#  define NECTERE_DEPRECATED_EXPORT NECTERE_EXPORT NECTERE_DEPRECATED
#endif

#ifndef NECTERE_DEPRECATED_NO_EXPORT
#  define NECTERE_DEPRECATED_NO_EXPORT NECTERE_NO_EXPORT NECTERE_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef NECTERE_NO_DEPRECATED
#    define NECTERE_NO_DEPRECATED
#  endif
#endif

#endif /* NECTERE_EXPORT_H */
