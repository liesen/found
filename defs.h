#ifndef INC_DEFS_H
#define INC_DEFS_H 1

#include <stdbool.h>

struct predicate;

typedef bool (*PRED_FUNC)(const char *path, struct predicate *pred);

struct predicate {
  PRED_FUNC func;
  char *name;
  char *arg;
};

/* pred.c */
bool pred_print(const char *path, struct predicate *pred);

bool apply_pred(const char *path, struct predicate *pred);

#endif
