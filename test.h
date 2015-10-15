#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

#ifndef _POSIX_PRIORITY_SCHEDULING
#error POSIX : pas d'ordonnancement TR
#endif