#include "defs.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static bool parse_and(const struct parser_table *entry, char **argv, int *arg_ptr);
static bool parse_exec(const struct parser_table *entry, char **argv, int *arg_ptr);
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
  {"event", parse_event, pred_event},
  {"exec", parse_exec, pred_exec},
  {"exit", parse_exit, pred_exit},
  {"false", parse_false, pred_false},
  {"iname", parse_iname, pred_iname},
  {"or", parse_or, pred_or},
  {"print", parse_print, pred_print},
  {"true", parse_true, pred_true},
  {"type", parse_type, pred_type},
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

bool
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
  pred->args.str = NULL;
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
parse_exec(const struct parser_table *entry, char **argv, int *arg_ptr)
{
  struct predicate *pred = insert_primary(entry);
  struct exec_val *execp = &pred->args.exec;
  char *brace_arg;
  int brace_count = 0;
  int i = *arg_ptr, j;

  for (j = i;
       argv[j] != NULL && (argv[j][0] != ';' || argv[j][1] != '\0');
       j++)
    {
      if (strstr(argv[j], "{}") != NULL)
        {
          brace_count++;
          brace_arg = argv[j];
        }
    }

  if (i == j || argv[j] == NULL)
    {
      *arg_ptr = j;
      free(pred);
      return false;
    }

  execp->argc = j - i;
  execp->argv = &argv[i];

  if (argv[j] == NULL)
    {
      *arg_ptr = j;
    }
  else
    {
      *arg_ptr = j + 1;
    }

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
      pred->args.str = name;
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
