#include "defs.h"

#include <errno.h>
#include <fnmatch.h>
#include <libgen.h>
#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char *replace_str = "{}";

bool
apply_pred(const char *path, int flags, struct predicate *pred)
{
  return (pred->pred_func)(path, flags, pred);
}

bool
pred_and(const char *path, int flags, struct predicate *pred)
{
  if (pred->pred_left == NULL || apply_pred(path, flags, pred->pred_left))
    {
      return apply_pred(path, flags, pred->pred_right);
    }

  return false;
}

bool
pred_closeparen(const char *path, int flags, struct predicate *pred)
{
  return true;
}

bool
pred_event(const char *path, int flags, struct predicate *pred)
{
  return (pred->args.flags & flags) != 0;
}

bool
pred_exec(const char *path, int flags, struct predicate *pred)
{
  static char *buf;

  if (buf == NULL)
    {
      size_t arg_max = sysconf(_SC_ARG_MAX);
      buf = malloc(arg_max);
    }

  struct exec_val *execp = &pred->args.exec;
  int argc = 0;
  char **argv = malloc((execp->argc + 1) * sizeof (char *));
  char *p = buf;

  for (; argc < execp->argc; argc++)
    {
      argv[argc] = p;
      char *arg = execp->argv[argc];
      size_t arglen = strlen(arg);

      do
        {
          size_t len;
          char *s = strstr(arg, replace_str);

          if (s)
            {
              len = s - arg;
            }
          else
            {
              len = arglen;
            }

          // Check length of buf

          strncpy(p, arg, len);
          p += len;
          arg += len;
          arglen -= len;

          if (s)
            {
              strcpy(p, path);
              p += strlen(path);
              arg += strlen(replace_str);
              arglen -= strlen(replace_str);
            }
        }
      while (*arg);

      if (*arg)
        {
          /* Command too long */
          fprintf(stderr, "command too long");
          exit(1);
        }

      *p++ = '\0';
    }

  argv[argc] = NULL;

  fflush(stdout);
  fflush(stderr);

  pid_t child_pid = fork();

  if (child_pid < 0)
    {
      fprintf(stderr, "cannot fork\n");
      exit(1);
    }

  if (child_pid == 0)
    {
      execvp(argv[0], argv);
      exit(1);
    }

  free(argv);

  while (waitpid(child_pid, &(execp->last_child_status), 0) == (pid_t) -1)
    {
      if (errno != EINTR)
        {
          fprintf(stderr, "error waiting for %s\n", argv[0]);
          return false;
        }
    }

  if (WEXITSTATUS(execp->last_child_status) == 0)
    {
      return true;
    }

  return true;
}

bool
pred_exit(const char *path, int flags, struct predicate *pred)
{
  exit(0);
  return true;
}

bool
pred_false(const char *path, int flags, struct predicate *pred)
{
  return false;
}

bool
pred_iname(const char *path, int flags, struct predicate *pred)
{
  return fnmatch(pred->args.str, basename((char *)path), FNM_CASEFOLD) == 0;
}

bool
pred_ipath(const char *path, int flags, struct predicate *pred)
{
  return fnmatch(pred->args.str, path, FNM_PATHNAME | FNM_CASEFOLD) == 0;
}

bool
pred_name(const char *path, int flags, struct predicate *pred)
{
  return fnmatch(pred->args.str, basename((char *)path), 0) == 0;
}

bool
pred_openparen(const char *path, int flags, struct predicate *pred)
{
  return true;
}

bool
pred_or(const char *path, int flags, struct predicate *pred)
{
  if (pred->pred_left == NULL || !apply_pred(path, flags, pred->pred_left))
    {
      return apply_pred(path, flags, pred->pred_right);
    }

  return true;
}

bool
pred_path(const char *path, int flags, struct predicate *pred)
{
  return fnmatch(pred->args.str, path, FNM_PATHNAME) == 0;
}

bool
pred_print(const char *path, int flags, struct predicate *pred)
{
  printf("%s\n", path);
  return true;
}

bool
pred_true(const char *path, int flags, struct predicate *pred)
{
  return true;
}

bool
pred_type(const char *path, int flags, struct predicate *pred)
{
  return (pred->args.flags & flags) != 0;
}
