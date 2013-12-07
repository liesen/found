#include "defs.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/* Root of the predicate expression. */
static struct predicate *root_pred = NULL;

/* The last predicate allocated. */
static struct predicate *last_pred = NULL;

/* Parse the command line arguments. */
struct predicate *
parse_args(int argc, char *argv[])
{
  const struct parser_table *parse_entry;
  char *pred_name;
  int i;

  root_pred = NULL;

  /* Skip arguments until we have something that looks like an expression. */
  for (i = 1; i < argc && argv[i][0] != '-'; i++)
    {
      /* Do nothing. */ ;
    }

  while (i < argc)
    {
      parse_entry = find_parser(pred_name);

      if (parse_entry == NULL)
        {
          fprintf(stderr, "option not recognized\n");
          exit(1);
        }

      /* Parse the rest. */
      i++;

      if (!(*(parse_entry->parser_func))(parse_entry, argv, &i))
        {
          /* Parser failed. */
          fprintf(stderr, "parser failed\n");
          exit(1);
        }
      else
        {
          last_pred->pred_name = pred_name;
        }
    }

  return root_pred;
}

struct predicate *
new_pred(const struct parser_table *entry)
{
  struct predicate *pred;

  if (root_pred == NULL)
    {
      root_pred = malloc(sizeof(struct predicate));
      last_pred = root_pred;
    }
  else
    {
      pred = malloc(sizeof(struct predicate));
      last_pred->pred_next = pred;
      last_pred = pred;
    }

  last_pred->pred_func = NULL;
  last_pred->pred_name = NULL;
  last_pred->arg = NULL;
  last_pred->pred_left = NULL;
  last_pred->pred_right = NULL;
  last_pred->pred_next = NULL;
  return last_pred;
}
