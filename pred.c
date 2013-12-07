#include "defs.h"

#include <stdbool.h>
#include <stdio.h>

bool
pred_print(const char *path)
{
  printf("%s\n", path);
  return true;
}

