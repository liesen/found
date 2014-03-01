#include "defs.h"
#include <assert.h>
#include <stdbool.h>
#include <string.h>

static bool parse_and(const struct parser_table *entry, char **argv, int *arg_ptr);
static bool parse_exit(const struct parser_table *entry, char **argv, int *arg_ptr);
static bool parse_false(const struct parser_table *entry, char **argv, int *arg_ptr);
static bool parse_iname(const struct parser_table *entry, char **argv, int *arg_ptr);
static bool parse_or(const struct parser_table *entry, char **argv, int *arg_ptr);
static bool parse_true(const struct parser_table *entry, char **argv, int *arg_ptr);

static struct parser_table const parse_table[] =
{
  {"(", parse_openparen, pred_openparen},
  {")", parse_closeparen, pred_closeparen},
  {"and", parse_and, pred_and},
  {"exit", parse_exit, pred_exit},
  {"false", parse_false, pred_false},
  {"iname", parse_iname, pred_iname},
  {"or", parse_or, pred_or},
  {"print", parse_print, pred_print},
  {"true", parse_true, pred_true},
  {0, 0, 0}
};

/* Finds a parser for a predicate name. */
const struct parser_table *
find_parser(char *search_name)
{
  int i = 0;

  /* Skip leading hyphen. */
  if (*search_name == '-')
    {
      search_name++;
    }

  for (i = 0; parse_table[i].parser_name != 0; i++)
    {
      if (strcmp(parse_table[i].parser_name, search_name) == 0)
        {
          return &parse_table[i];
        }
    }

  return NULL;
}

static bool
collect_arg(char **argv, int *arg_ptr, const char **collected_arg)
{
  if ((argv == NULL) || (argv[*arg_ptr] == NULL))
    {
      *collected_arg = NULL;
      return false;
    }

  *collected_arg = argv[*arg_ptr];
  (*arg_ptr)++;
  return true;
}

struct predicate *
insert_primary(const struct parser_table *entry)
{
  assert(entry->pred_func != NULL);
  struct predicate *pred = new_primary_pred(entry);
  pred->arg = NULL;
  return pred;
}

bool
parse_and(const struct parser_table *entry, char **argv, int *arg_ptr)
{
  struct predicate *pred = new_pred(entry);
  pred->pred_type = BI_OP;
  pred->pred_prec = AND_PREC;
  return true;
}

bool
parse_closeparen(const struct parser_table *entry, char **argv, int *arg_ptr)
{
  struct predicate *pred = new_pred(entry);
  pred->pred_type = CLOSE_PAREN;
  pred->pred_prec = NO_PREC;
  return true;
}

bool
parse_exit(const struct parser_table *entry, char **argv, int *arg_ptr)
{
  struct predicate *pred = insert_primary(entry);
  return true;
}

bool
parse_false(const struct parser_table *entry, char **argv, int *arg_ptr)
{
  struct predicate *pred = insert_primary(entry);
  return true;
}

static bool
parse_iname(const struct parser_table *entry, char **argv, int *arg_ptr)
{
  const char *name;

  if (collect_arg(argv, arg_ptr, &name))
    {
      struct predicate *pred = insert_primary(entry);
      pred->arg = name;
      return true;
    }

  return false;
}

bool
parse_openparen(const struct parser_table *entry, char **argv, int *arg_ptr)
{
  struct predicate *pred = new_pred_chk_op(entry);
  pred->pred_type = OPEN_PAREN;
  pred->pred_prec = NO_PREC;
  return true;
}

bool
parse_or(const struct parser_table *entry, char **argv, int *arg_ptr)
{
  struct predicate *pred = new_pred(entry);
  pred->pred_type = BI_OP;
  pred->pred_prec = OR_PREC;
  return true;
}

bool
parse_print(const struct parser_table *entry, char **argv, int *arg_ptr)
{
  struct predicate *pred = insert_primary(entry);
  pred->no_default_print = true;
  return true;
}

bool
parse_true(const struct parser_table *entry, char **argv, int *arg_ptr)
{
  struct predicate *pred = insert_primary(entry);
  return true;
}
