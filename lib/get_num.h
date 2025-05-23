#ifndef GET_NUM_H
#define GET_NUM_H

#define GN_NONNEG 01
#define GN_GT_0 02


#define GN_ANY_BASE 0100
#define GN_BASE_8 0200
#define GN_BASE_16 0400

/* Both of these functions return the `arg` converted to string and errors out by printing a message and exiting the program if `arg` is not a valid integer string */
long getLong(const char *arg, int flags, const char *name);

int getInt(const char *arg, int flags, const char *name);

#endif
