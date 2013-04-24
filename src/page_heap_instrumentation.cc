#include "page_heap.h"
#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include "base/spinlock.h"              // for SpinLockHolder
#include "static_vars.h"       // for Static

using tcmalloc::Static;

extern "C" {
  extern void launch_instrumentation_thread() __attribute((constructor));

  pthread_t insthread;
  int insfd, insclient;
  socklen_t len;
  struct sockaddr_un insaddr, clientaddr;

  static void* instrumentation_thread_loop(void *arg) {
    char buf[1];

    while(1) {
      len = (socklen_t)sizeof(clientaddr);
      insclient = accept(insfd, (struct sockaddr*)&clientaddr, &len);
      read(insclient, buf, sizeof(buf));
      close(insclient);

      SpinLockHolder h(Static::pageheap_lock());
      Static::pageheap()->PrintLargeAllocStats();
    }

    return NULL;
  }

  extern void launch_instrumentation_thread(void) {
    char *socketpath;

    socketpath = getenv("TCMALLOC_INSTRUMENTATION_SOCKET");

    if (socketpath == NULL) {
      fprintf(stderr, "[tcmalloc] TCMALLOC_INSTRUMENTATION_SOCKET isn't set, so we're not launching the thread.\n");
      return;
    }

    insfd = socket(PF_UNIX, SOCK_STREAM, 0);
    if (insfd < 0) {
      fprintf(stderr, "socket() failed\n");
      exit(1);
    }

    memset(&insaddr, 0, sizeof(insaddr));
    insaddr.sun_family = AF_UNIX;

    unlink(socketpath);
    strncpy(insaddr.sun_path, socketpath, sizeof(insaddr.sun_path)-1);

    if (bind(insfd, (struct sockaddr*)&insaddr, sizeof(insaddr)) != 0) {
      fprintf(stderr, "bind() failed %d\n", errno);
      exit(1);
    }

    if (listen(insfd, 5) != 0) {
      fprintf(stderr, "listen() failed\n");
      exit(1);
    }

    pthread_create(&insthread, NULL, instrumentation_thread_loop, NULL);
  }
}
