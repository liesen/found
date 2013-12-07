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
main(int argc, char **argv)
{
  CFStringRef paths[1] = {
      CFStringCreateWithCString(NULL, argv[1], kCFStringEncodingUTF8)
  };
  CFArrayRef pathsToWatch = CFArrayCreate(
      NULL,
      (void *) paths,
      1,
      &kCFTypeArrayCallBacks);

  struct predicate *pred = parse_args(argc, argv);

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
