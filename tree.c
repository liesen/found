#include "defs.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/* Root of the predicate expression. */
static struct predicate *root_pred = NULL;

/* Head of the predicate list. */
static struct predicate *head_pred = NULL;

/* The last predicate allocated. */
static struct predicate *last_pred = NULL;

struct predicate *
parse_tree(struct predicate **input)
{
  struct predicate *next_pred = NULL;
  struct predicate *this_pred = *input;

  if (*input == NULL)
    {
      fprintf(stderr, "invalid expression\n");
      exit(1);
    }

  switch ((*input)->pred_type)
    {
    case NO_TYPE:
      fprintf(stderr, "invalid expression\n");
      exit(1);
      break;

    case BI_OP:
      fprintf(stderr, "invalid expression; you have used a binary operator `%s' with nothing before it.\n",
              this_pred->pred_name);
      exit(1);
      break;

    case PRIMARY_TYPE:
      next_pred = *input;
      *input = (*input)->pred_next;
      break;

    case UN_OP:
      next_pred = *input;
      *input = (*input)->pred_next;
      next_pred->pred_right = parse_tree(input);
      break;

    default:
      fprintf(stderr, "oops -- invalid expression type!\n");
      exit(1);
      break;
    }

  if (*input == NULL)
    return next_pred;

  return next_pred;
}

/* Parse the command line arguments. */
struct predicate *
parse_args(int argc, char *argv[])
{
  const struct parser_table *parse_entry;
  char *pred_name;
  int i;

  head_pred = NULL;

  /* Skip arguments until we have something that looks like an expression. */
  for (i = 1; i < argc && argv[i][0] != '-'; i++)
    {
      /* Do nothing. */ ;
    }

  while (i < argc)
    {
      pred_name = argv[i];
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
          if (argv[i])
            {
              fprintf(stderr, "invalid argument `%s' to `%s'\n",
                      argv[i], pred_name);
              exit(1);
            }
          else
            {
              fprintf(stderr, "missing argument to `%s'\n", pred_name);
              exit(1);
            }
        }
      else
        {
          last_pred->pred_name = pred_name;
        }
    }

  root_pred = parse_tree(&head_pred);
  return root_pred;
}

struct predicate *
new_pred(const struct parser_table *entry)
{
  struct predicate *pred;

  if (head_pred == NULL)
    {
      head_pred = malloc(sizeof(struct predicate));
      last_pred = head_pred;
    }
  else
    {
      pred = malloc(sizeof(struct predicate));
      last_pred->pred_next = pred;
      last_pred = pred;
    }

  last_pred->pred_func = NULL;
  last_pred->pred_name = NULL;
  last_pred->pred_type = NO_TYPE;
  last_pred->pred_prec = NO_PREC;
  last_pred->arg = NULL;
  last_pred->pred_left = NULL;
  last_pred->pred_right = NULL;
  last_pred->pred_next = NULL;
  return last_pred;
}
