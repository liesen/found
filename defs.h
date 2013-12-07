#ifndef INC_DEFS_H
#define INC_DEFS_H 1

#include <stdbool.h>

struct predicate;

typedef bool (*PRED_FUNC)(const char *path, struct predicate *pred);

struct predicate {
  PRED_FUNC func;
  char *name;
  char *arg;

  struct predicate *pred_left;
  struct predicate *pred_right;
};

/* pred.c */
bool apply_pred(const char *path, struct predicate *pred);

bool pred_and(const char *path, struct predicate *pred);
bool pred_path(const char *path, struct predicate *pred);
bool pred_print(const char *path, struct predicate *pred);
bool pred_ipath(const char *path, struct predicate *pred);

#endif
