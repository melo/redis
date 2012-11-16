/* ZeroMQ I/O service for Redis.
 *
 *
 * ----------------------------------------------------------------------------
 *
 * FIXME: include copyright notice, check with Salvatore
 *
 */

#include "redis.h"
#include "zeromq.h"

#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

static void *context;

/* Initialize the ZeroMQ context, create the sockets and connect/bind them */
void zeroMqInit(void) {
  int major, minor, patch;

  context = zmq_ctx_new();
  if (context == NULL) {
    redisLog(REDIS_WARNING, "Failed to create ZeroMQ context");
    return;
  }

  zmq_version (&major, &minor, &patch);
  redisLog(REDIS_DEBUG, "ZeroMQ context started, version is %d.%d.%d", major, minor, patch);
}

