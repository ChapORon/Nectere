
#ifndef NECTEREDP_EXPORT_H
#define NECTEREDP_EXPORT_H

#ifdef NECTEREDP_STATIC_DEFINE
#  define NECTEREDP_EXPORT
#  define NECTEREDP_NO_EXPORT
#else
#  ifndef NECTEREDP_EXPORT
#    ifdef NectereDp_EXPORTS
        /* We are building this library */
#      define NECTEREDP_EXPORT 
#    else
        /* We are using this library */
#      define NECTEREDP_EXPORT 
#    endif
#  endif

#  ifndef NECTEREDP_NO_EXPORT
#    define NECTEREDP_NO_EXPORT 
#  endif
#endif

#ifndef NECTEREDP_DEPRECATED
#  define NECTEREDP_DEPRECATED __declspec(deprecated)
#endif

#ifndef NECTEREDP_DEPRECATED_EXPORT
#  define NECTEREDP_DEPRECATED_EXPORT NECTEREDP_EXPORT NECTEREDP_DEPRECATED
#endif

#ifndef NECTEREDP_DEPRECATED_NO_EXPORT
#  define NECTEREDP_DEPRECATED_NO_EXPORT NECTEREDP_NO_EXPORT NECTEREDP_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef NECTEREDP_NO_DEPRECATED
#    define NECTEREDP_NO_DEPRECATED
#  endif
#endif

#endif /* NECTEREDP_EXPORT_H */
