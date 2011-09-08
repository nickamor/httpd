#include <stdio.h>
#include "common.h"

void
dbgprint(char *string)
{
  fprintf(stderr, "%s\n", string);
}
