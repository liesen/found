#include "defs.h"

#include <fnmatch.h>
#include <stdbool.h>
#include <stdio.h>

bool
apply_pred(const char *path, struct predicate *pred)
{
  return (pred->func)(path, pred);
}

bool
pred_and(const char *path, struct predicate *pred)
{
  if (pred->pred_left == NULL || apply_pred(path, pred->pred_left))
    {
      return apply_pred(path, pred->pred_right);
    }

  return false;
}

bool
pred_ipath(const char *path, struct predicate *pred)
{
  return fnmatch(pred->arg, path, FNM_PATHNAME | FNM_CASEFOLD) == 0;
}

bool
pred_path(const char *path, struct predicate *pred)
{
  return fnmatch(pred->arg, path, FNM_PATHNAME) == 0;
}

bool
pred_print(const char *path, struct predicate *pred)
{
  printf("%s\n", path);
  return true;
}
