#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include "get_num.h"

static long getLong(const char *fname, const char *arg, int flags, const char *name );

static int getInt(const char *fname, const char *arg, int flags, const char *name);
