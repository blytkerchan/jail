#ifndef _LIBCONF_POSIX_READCONF_H
#define _LIBCONF_POSIX_READCONF_H

#include "../libconf.h"

#ifdef __cplusplus
extern "C" {
#endif

int libconf_readconf_global(libconf_t * handle);
int libconf_readconf_local(libconf_t * handle);

#ifdef __cplusplus
}
#endif

#endif // _LIBCONF_POSIX_READCONF_H
