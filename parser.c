#include "defs.h"
#include <assert.h>
#include <stdbool.h>
#include <string.h>

static struct parser_table const parse_table[] =
{
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
  pred->arg = NULL;
  return pred;
}

bool
parse_print(const struct parser_table *entry, char **argv, int *arg_ptr)
{
  insert_primary(entry);
  return true;
}
