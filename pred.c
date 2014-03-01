#include "defs.h"

#include <fnmatch.h>
#include <libgen.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool
apply_pred(const char *path, int flags, struct predicate *pred)
{
  return (pred->pred_func)(path, flags, pred);
}

bool
pred_and(const char *path, int flags, struct predicate *pred)
{
  if (pred->pred_left == NULL || apply_pred(path, flags, pred->pred_left))
    {
      return apply_pred(path, flags, pred->pred_right);
    }

  return false;
}

bool
pred_closeparen(const char *path, int flags, struct predicate *pred)
{
  return true;
}

bool
pred_event(const char *path, int flags, struct predicate *pred)
{
  return (pred->args.flags & flags) != 0;
}

bool
pred_exit(const char *path, int flags, struct predicate *pred)
{
  exit(0);
  return true;
}

bool
pred_false(const char *path, int flags, struct predicate *pred)
{
  return false;
}

bool
pred_iname(const char *path, int flags, struct predicate *pred)
{
  return fnmatch(pred->args.str, basename((char *)path), FNM_CASEFOLD) == 0;
}

bool
pred_ipath(const char *path, int flags, struct predicate *pred)
{
  return fnmatch(pred->args.str, path, FNM_PATHNAME | FNM_CASEFOLD) == 0;
}

bool
pred_name(const char *path, int flags, struct predicate *pred)
{
  return fnmatch(pred->args.str, basename((char *)path), 0) == 0;
}

bool
pred_openparen(const char *path, int flags, struct predicate *pred)
{
  return true;
}

bool
pred_or(const char *path, int flags, struct predicate *pred)
{
  if (pred->pred_left == NULL || !apply_pred(path, flags, pred->pred_left))
    {
      return apply_pred(path, flags, pred->pred_right);
    }

  return true;
}

bool
pred_path(const char *path, int flags, struct predicate *pred)
{
  return fnmatch(pred->args.str, path, FNM_PATHNAME) == 0;
}

bool
pred_print(const char *path, int flags, struct predicate *pred)
{
  printf("%s\n", path);
  return true;
}

bool
pred_true(const char *path, int flags, struct predicate *pred)
{
  return true;
}

bool
pred_type(const char *path, int flags, struct predicate *pred)
{
  return (pred->args.flags & flags) != 0;
}
