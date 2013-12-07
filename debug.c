#include "defs.h"
#include <stdio.h>

void
print_predicate(FILE *fp, const struct predicate *pred)
{
  fprintf(fp, "%s", pred->pred_name);

  if (pred->arg)
    fprintf(fp, " %s", pred->arg);

  fprintf(fp, "\n");
}

void
print_tree(FILE *fp, const struct predicate *pred, int indent)
{
  int i;

  if (pred == NULL)
    return;

  for (i = 0; i < indent; i++)
    fprintf(fp, "  ");

  fprintf(fp, "pred=[");
  print_predicate(fp, pred);
  fprintf(fp, "]\n");

  for (i = 0; i < indent; i++)
    fprintf(fp, "  ");

  if (pred->pred_left)
    {
      fprintf(fp, "left:\n");
      print_tree(fp, pred->pred_left, indent + 1);
    }

  if (pred->pred_right)
    {
      fprintf(fp, "right:\n");
      print_tree(fp, pred->pred_right, indent + 1);
    }
}

