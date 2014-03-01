#ifndef INC_DEFS_H
#define INC_DEFS_H 1

#include <stdbool.h>
#include <stdio.h>

struct predicate;

typedef bool (*PRED_FUNC)(const char *path, int flags, struct predicate *pred);

enum predicate_type
{
  NO_TYPE,
  PRIMARY_TYPE,
  UN_OP,
  BI_OP,
  OPEN_PAREN,
  CLOSE_PAREN
};

enum predicate_precedence
{
  NO_PREC,
  OR_PREC,
  AND_PREC,
  NEGATE_PREC
};

struct predicate
{
  PRED_FUNC pred_func;

  char *pred_name;

  enum predicate_type pred_type;

  enum predicate_precedence pred_prec;

  bool no_default_print;

  bool artificial;

  /* Argument passed to the predicate. */
  union
  {
    const char *str;
    int flags;
  } args;

  /* Left and right predicates for binary operations.  The right predicate
     is used as argument to unary operations. */
  struct predicate *pred_left;
  struct predicate *pred_right;

  /* Pointer to the next predicate. */
  struct predicate *pred_next;
};

/* parser.c */
struct parser_table;

typedef bool (*PARSE_FUNC)(const struct parser_table *p,
    char *argv[], int *arg_ptr);

struct parser_table
{
  char *parser_name;
  PARSE_FUNC parser_func;
  PRED_FUNC pred_func;
};

const struct parser_table *find_parser(char *search_name);
bool parse_closeparen(const struct parser_table *entry, char **argv, int *arg_ptr);
bool parse_event(const struct parser_table *entry, char **argv, int *arg_ptr);
bool parse_negate(const struct parser_table *entry, char **argv, int *arg_ptr);
bool parse_openparen(const struct parser_table *entry, char **argv, int *arg_ptr);
bool parse_print(const struct parser_table *entry, char **argv, int *arg_ptr);
bool parse_type(const struct parser_table *entry, char **argv, int *arg_ptr);
bool collect_arg(char **argv, int *arg_ptr, const char **collected_arg);

/* pred.c */
bool apply_pred(const char *path, int flags, struct predicate *pred);

bool pred_and(const char *path, int flags, struct predicate *pred);
bool pred_closeparen(const char *path, int flags, struct predicate *pred);
bool pred_exit(const char *path, int flags, struct predicate *pred);
bool pred_event(const char *path, int flags, struct predicate *pred);
bool pred_false(const char *path, int flags, struct predicate *pred);
bool pred_iname(const char *path, int flags, struct predicate *pred);
bool pred_ipath(const char *path, int flags, struct predicate *pred);
bool pred_name(const char *path, int flags, struct predicate *pred);
bool pred_openparen(const char *path, int flags, struct predicate *pred);
bool pred_or(const char *path, int flags, struct predicate *pred);
bool pred_path(const char *path, int flags, struct predicate *pred);
bool pred_print(const char *path, int flags, struct predicate *pred);
bool pred_true(const char *path, int flags, struct predicate *pred);
bool pred_type(const char *path, int flags, struct predicate *pred);

struct predicate * insert_primary(const struct parser_table *entry);

/* tree.c */
struct predicate *parse_args(int argc, char *argv[]);
struct predicate *new_pred(const struct parser_table *entry);
struct predicate *new_pred_chk_op(const struct parser_table *entry);
struct predicate *new_primary_pred(const struct parser_table *entry);

/* debug.c */
void print_predicate(FILE *fp, const struct predicate *pred);
void print_tree(FILE *fp, const struct predicate *pred, int indent);

#endif
