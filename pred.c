#include "defs.h"

#include <fnmatch.h>
#include <libgen.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

bool
apply_pred(const char *path, struct predicate *pred)
{
  return (pred->pred_func)(path, pred);
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
pred_closeparen(const char *path, struct predicate *pred)
{
  return true;
}

bool
pred_exit(const char *path, struct predicate *pred)
{
  exit(0);
  return true;
}

bool
pred_iname(const char *path, struct predicate *pred)
{
  return fnmatch(pred->arg, basename((char *)path), FNM_CASEFOLD) == 0;
}

bool
pred_ipath(const char *path, struct predicate *pred)
{
  return fnmatch(pred->arg, path, FNM_PATHNAME | FNM_CASEFOLD) == 0;
}

bool
pred_name(const char *path, struct predicate *pred)
{
  return fnmatch(pred->arg, basename((char *)path), 0) == 0;
}

bool
pred_openparen(const char *path, struct predicate *pred)
{
  return true;
}

bool
pred_or(const char *path, struct predicate *pred)
{
  if (pred->pred_left == NULL || !apply_pred(path, pred->pred_left))
    {
      return apply_pred(path, pred->pred_right);
    }

  return true;
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
