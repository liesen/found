#include "defs.h"

#include <stdbool.h>
#include <stdio.h>

bool
apply_pred(const char *path, struct predicate *pred)
{
  return (pred->func)(path, pred);
}

bool
pred_print(const char *path, struct predicate *pred)
{
  printf("%s\n", path);
  return true;
}

