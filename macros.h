#ifndef _MACROS_H
#define _MACROS_H

#define strip_slash(str) if (str[strlen(str) - 1] == '/') str[strlen(str) - 1] = 0
#define cat_slash(str) if (str[strlen(str) - 1] != '/') strcat(str, "/")

#endif // _MACROS_H
