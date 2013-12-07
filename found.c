#include <CoreServices/CoreServices.h>
#include "defs.h"

void
callback(
    ConstFSEventStreamRef streamRef,
    void *clientCallBackInfo,
    size_t numEvents,
    void *eventPaths,
    const FSEventStreamEventFlags eventFlags[],
    const FSEventStreamEventId eventIds[])
{
  struct predicate *pred = clientCallBackInfo;
  int i;

  for (i = 0; i < numEvents; i++)
    {
      char *filename = ((char **)eventPaths)[i];

      if (apply_pred(filename, pred) == 0)
        {
          continue;
        }
    }
}

int
main (int argc, char **argv)
{
  CFStringRef paths[1] = {
      CFStringCreateWithCString(NULL, argv[1], kCFStringEncodingUTF8)
  };
  CFArrayRef pathsToWatch = CFArrayCreate(
      NULL,
      (void *) paths,
      1,
      &kCFTypeArrayCallBacks);

  struct predicate *pred = malloc(sizeof(struct predicate));
  pred->func = &pred_and;
  pred->name = "and";
  pred->pred_left = malloc(sizeof(struct predicate));
  pred->pred_left->func = &pred_iname;
  pred->pred_left->name = "iname";
  pred->pred_left->arg = "*.h";
  pred->pred_right = malloc(sizeof(struct predicate));
  pred->pred_right->func = &pred_print;
  pred->pred_right->name = "print";
  pred->pred_right->arg = NULL;

  FSEventStreamContext context = {
    .version = 0,
    .info = pred,
    .retain = NULL,
    .release = NULL,
    .copyDescription = NULL
  };
  CFAbsoluteTime latency = 1.0;
  FSEventStreamRef stream = FSEventStreamCreate(
      NULL,
      &callback,
      &context,
      pathsToWatch,
      kFSEventStreamEventIdSinceNow,
      latency,
      kFSEventStreamCreateFlagNoDefer | kFSEventStreamCreateFlagFileEvents);
  FSEventStreamScheduleWithRunLoop(
      stream,
      CFRunLoopGetCurrent(),
      kCFRunLoopDefaultMode);
  FSEventStreamStart(stream);
  CFRunLoopRun();
}
