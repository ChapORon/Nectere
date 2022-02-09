
#ifndef PARG_EXPORT_H
#define PARG_EXPORT_H

#ifdef PARG_STATIC_DEFINE
#  define PARG_EXPORT
#  define PARG_NO_EXPORT
#else
#  ifndef PARG_EXPORT
#    ifdef parg_EXPORTS
        /* We are building this library */
#      define PARG_EXPORT 
#    else
        /* We are using this library */
#      define PARG_EXPORT 
#    endif
#  endif

#  ifndef PARG_NO_EXPORT
#    define PARG_NO_EXPORT 
#  endif
#endif

#ifndef PARG_DEPRECATED
#  define PARG_DEPRECATED __declspec(deprecated)
#endif

#ifndef PARG_DEPRECATED_EXPORT
#  define PARG_DEPRECATED_EXPORT PARG_EXPORT PARG_DEPRECATED
#endif

#ifndef PARG_DEPRECATED_NO_EXPORT
#  define PARG_DEPRECATED_NO_EXPORT PARG_NO_EXPORT PARG_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef PARG_NO_DEPRECATED
#    define PARG_NO_DEPRECATED
#  endif
#endif

#endif /* PARG_EXPORT_H */
