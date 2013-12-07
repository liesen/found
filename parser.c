#include "defs.h"
#include <assert.h>
#include <stdbool.h>
#include <string.h>

static bool parse_and(const struct parser_table *entry, char **argv, int *arg_ptr);

static struct parser_table const parse_table[] =
{
  {"(", parse_openparen, pred_openparen},
  {")", parse_closeparen, pred_closeparen},
  {"and", parse_and, pred_and},
  {"print", parse_print, pred_print},
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

struct predicate *
insert_primary(const struct parser_table *entry)
{
  assert(entry->pred_func != NULL);
  struct predicate *pred = new_pred(entry);
  pred->pred_func = entry->pred_func;
  pred->pred_name = entry->parser_name;
  pred->pred_type = PRIMARY_TYPE;
  pred->arg = NULL;
  return pred;
}

bool
parse_and(const struct parser_table *entry, char **argv, int *arg_ptr)
{
  struct predicate *pred = new_pred(entry);
  pred->pred_func = pred_and;
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
parse_openparen(const struct parser_table *entry, char **argv, int *arg_ptr)
{
  struct predicate *pred = new_pred(entry);
  pred->pred_type = OPEN_PAREN;
  pred->pred_prec = NO_PREC;
  return true;
}

bool
parse_print(const struct parser_table *entry, char **argv, int *arg_ptr)
{
  struct predicate *pred = insert_primary(entry);
  pred->no_default_print = true;
  return true;
}
