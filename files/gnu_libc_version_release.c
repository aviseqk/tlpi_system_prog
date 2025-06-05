#include <stdlib.h>
#include <stdio.h>
#include <gnu/libc-version.h>

void main(void){
  const char *version, *release;

  release = gnu_get_libc_release();
  version = gnu_get_libc_version();

  printf("GNU libc Version:      %s\nGNU libc Release:     %s\n",version, release);
}
