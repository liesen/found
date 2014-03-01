#include "defs.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/* Root of the predicate expression tree. */
static struct predicate *root_pred = NULL;

/* Head of the predicate list. */
static struct predicate *head_pred = NULL;

/* The last predicate allocated. */
static struct predicate *last_pred = NULL;

static struct predicate *
scan_rest(struct predicate **input, struct predicate *head_pred,
          enum predicate_precedence prev_prec);

struct predicate *
parse_tree(struct predicate **input, enum predicate_precedence prev_prec,
           struct predicate *prev_pred)
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

    case CLOSE_PAREN:
      if ((prev_pred->pred_type == UN_OP || prev_pred->pred_type == BI_OP)
          && !this_pred->artificial)
        {
          fprintf(stderr, "expected an expression between '%s' and ')'",
                  prev_pred->pred_name);
          exit(1);
        }
      else if ((*input)->artificial)
        {
          fprintf(stderr, "expected an expression after '%s'",
                  prev_pred->pred_name);
          exit(1);
        }
      else
        {
          fprintf(stderr, "invalid expression: too many ')'");
          exit(1);
        }
      break;

    case PRIMARY_TYPE:
      next_pred = *input;
      *input = (*input)->pred_next;
      break;

    case UN_OP:
      next_pred = *input;
      *input = (*input)->pred_next;
      next_pred->pred_right = parse_tree(input, NEGATE_PREC, next_pred);
      break;

    case OPEN_PAREN:
      if (((*input)->pred_next == NULL) || (*input)->pred_next->artificial)
        {
          fprintf(stderr, "invalid expression; expected to find a `)' but didn't see one. Perhaps you need an extra predicate after `%s'",
                  this_pred->pred_name);
          exit(1);
        }

      prev_pred = *input;
      *input = (*input)->pred_next;

      if ((*input)->pred_type == CLOSE_PAREN)
        {
          fprintf(stderr, "invalid expression; empty parentheses not allowed.\n");
          exit(1);
        }

      next_pred = parse_tree(input, NO_PREC, prev_pred);

      if ((*input == NULL)
          || ((*input)->pred_type != CLOSE_PAREN))
        {
          fprintf(stderr, "invalid expression; expected to find a `)' somewhere but didn't see one.\n");
          exit(1);
        }

      *input = (*input)->pred_next;  /* move over close paren */
      break;

    default:
      fprintf(stderr, "oops -- invalid expression type!\n");
      exit(1);
      break;
    }

  if (*input == NULL)
    return next_pred;

  if ((*input)->pred_prec > prev_prec)
    {
      next_pred = scan_rest(input, next_pred, prev_prec);

      if (next_pred == NULL)
        {
          fprintf(stderr, "invalid expression\n");
          exit(1);
        }
    }

  return next_pred;
}

static struct predicate *
scan_rest(struct predicate **input, struct predicate *head_pred,
          enum predicate_precedence prev_prec)
{
  struct predicate *tree;

  if ((*input == NULL) || ((*input)->pred_type == CLOSE_PAREN))
    return NULL;

  tree = head_pred;

  while ((*input != NULL) && ((*input)->pred_prec > prev_prec))
    {
      switch ((*input)->pred_type)
        {
        case NO_TYPE:
        case PRIMARY_TYPE:
        case UN_OP:
        case OPEN_PAREN:
          fprintf(stderr, "invalid expression\n");
          exit(1);
          break;

        case BI_OP:
          {
            struct predicate *prev_pred = *input;
            (*input)->pred_left = tree;
            tree = *input;
            *input = (*input)->pred_next;
            tree->pred_right = parse_tree(input, tree->pred_prec, prev_pred);
          }
          break;

        case CLOSE_PAREN:
          return tree;

        default:
          fprintf(stderr, "oops -- invalid expression type!\n");
          exit(1);
          break;
        }
    }

  return tree;
}

bool
default_prints(struct predicate *pred)
{
  while (pred != NULL)
    {
      if (pred->no_default_print)
        return false;

      pred = pred->pred_next;
    }

  return true;
}

bool
looks_like_expression (const char *arg, bool leading)
{
  switch (arg[0])
  {
    case '-':
      if (arg[1])  /* "-foo" is an expression.  */
        return true;
      else
        return false;  /* Just "-" is a filename. */
      break;

    case ')':
    case ',':
      if (arg[1])
        return false;  /* )x and ,z are not expressions */
      else
        return !leading;  /* A leading ) or , is not either */

      /* ( and ! are part of an expression, but (2 and !foo are
       * filenames.
       */
    case '!':
    case '(':
      if (arg[1])
        return false;
      else
        return true;

    default:
      return false;
  }
}

/* Parse the command line arguments. */
struct predicate *
parse_args(int argc, char *argv[])
{
  const struct parser_table *parse_entry;
  char *pred_name;
  struct predicate *temp_pred;
  const struct parser_table *entry_open, *entry_close, *entry_print;
  int i;

  head_pred = NULL;

  entry_open = find_parser("(");
  entry_close = find_parser(")");
  entry_print = find_parser("print");

  /* Enclose expression in `(...)'. */
  parse_openparen(entry_open, argv, &argc);
  last_pred->pred_name = "(";
  head_pred->artificial = true;

  /* Skip arguments until we have something that looks like an expression. */
  for (i = 1; i < argc && !looks_like_expression(argv[i], true); i++)
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

  if (head_pred->pred_next == NULL)
    {
      temp_pred = head_pred;
      head_pred = last_pred = head_pred->pred_next;
      free(temp_pred);
      parse_print(entry_print, argv, &argc);
      last_pred->pred_name = "-print";
    }
  else if (!default_prints(head_pred->pred_next))
    {
      /* One or more predicates that produce output were given;
         remove the unneeded initial `('. */
      temp_pred = head_pred;
      head_pred = head_pred->pred_next;
      free(temp_pred);
    }
  else
    {
      /* `( user-supplied-expression ) -print' */
      /* Append artificial close-paren and print predicates */
      parse_closeparen(entry_close, argv, &argc);
      last_pred->pred_name = ")";
      last_pred->artificial = true;
      parse_print(entry_print, argv, &argc);
      last_pred->pred_name = "-print";
      last_pred->artificial = true;
    }

  temp_pred = head_pred;
  root_pred = parse_tree(&temp_pred, NO_PREC, NULL);
  return root_pred;
}

struct predicate *
new_pred_chk_op(const struct parser_table *entry)
{
  struct predicate *pred;
  static const struct parser_table *entry_and = NULL;

  if (entry_and == NULL)
    entry_and = find_parser("and");

  if (last_pred != NULL)
    switch (last_pred->pred_type)
      {
      case NO_TYPE:
        fprintf(stderr, "oops -- invalid default insertion of and!\n");
        exit(1);
        break;

      case PRIMARY_TYPE:
      case CLOSE_PAREN:
        pred = new_pred(entry_and);
        pred->pred_func = pred_and;
        pred->pred_name = "-a";
        pred->pred_type = BI_OP;
        pred->pred_prec = AND_PREC;
        pred->no_default_print = false;
        pred->artificial = true;
        break;

      default:
        break;
      }

  pred = new_pred(entry);
  return pred;
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

  last_pred->pred_func = entry->pred_func;
  last_pred->pred_name = NULL;
  last_pred->pred_type = NO_TYPE;
  last_pred->pred_prec = NO_PREC;
  last_pred->no_default_print = false;
  last_pred->artificial = false;
  last_pred->arg = NULL;
  last_pred->pred_left = NULL;
  last_pred->pred_right = NULL;
  last_pred->pred_next = NULL;
  return last_pred;
}

struct predicate *
new_primary_pred (const struct parser_table *entry)
{
  struct predicate *pred = new_pred_chk_op(entry);
  pred->pred_func = entry->pred_func;
  pred->pred_name = entry->parser_name;
  pred->pred_type = PRIMARY_TYPE;
  return pred;
}
