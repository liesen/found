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
  int i;

  for (i = 0; i < numEvents; i++)
    {
      char *filename = ((char **)eventPaths)[i];

      if (pred_print(filename) == 0)
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
  FSEventStreamContext context = {
    .version = 0,
    .info = NULL,
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
