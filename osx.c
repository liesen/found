#include <CoreServices/CoreServices.h>
#include "defs.h"

struct event {
  const char *event;
  int flags;
};

static struct event const events[] = {
  // {"access", 0},
  {"modify", kFSEventStreamEventFlagItemModified},
  // {"attrib", 0},
  // {"close_write", 0},
  // {"close_nowrite", 0},
  // {"close", 0},
  // {"open", 0},
  // {"moved_to", 0},
  // {"moved_from", 0},
  // {"move", 0},
  // {"move_self", 0},
  {"create", kFSEventStreamEventFlagItemCreated},
  {"delete", kFSEventStreamEventFlagItemRemoved},
  // {"delete_self", 0},
  // {"unmount", 0},
  {0, 0}
};

struct type {
  const char *type;
  int flags;
};

static struct type const types[] = {
  {"d", kFSEventStreamEventFlagItemIsDir},
  {"f", kFSEventStreamEventFlagItemIsFile},
  {0, 0}
};

bool
parse_event(const struct parser_table *entry, char **argv, int *arg_ptr)
{
  const char *name;

  if (collect_arg(argv, arg_ptr, &name))
    {
      int i = 0;

      for (i = 0; events[i].event != 0; i++)
        {
          if (strcmp(events[i].event, name) == 0)
            {
              struct predicate *pred = insert_primary(entry);
              pred->args.flags = events[i].flags;
              return true;
            }
        }
    }

  return false;
}

bool
parse_type(const struct parser_table *entry, char **argv, int *arg_ptr)
{
  const char *name;

  if (collect_arg(argv, arg_ptr, &name))
    {
      int i = 0;

      for (i = 0; types[i].type != 0; i++)
        {
          if (strcmp(types[i].type, name) == 0)
            {
              struct predicate *pred = insert_primary(entry);
              pred->args.flags = types[i].flags;
              return true;
            }
        }
    }

  return false;
}
