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
static void *publish_sock;

static void zeroMqSetupPublisher (void);

/* Initialize the ZeroMQ context, create the sockets and connect/bind them */
void zeroMqInit(void) {
  int major, minor, patch;

  /* skip if we don't have any use for ZeroMQ */
  if (server.zeromq_publish_bind == NULL) return;

  context = zmq_ctx_new();
  if (context == NULL) {
    redisLog(REDIS_WARNING, "Failed to create ZeroMQ context");
    return;
  }

  zmq_version (&major, &minor, &patch);
  redisLog(REDIS_DEBUG, "ZeroMQ context started, version is %d.%d.%d", major, minor, patch);

  zeroMqSetupPublisher();
}


/*
 *
 * ZeroMQ Publisher socket: forward all Redis PUBLISH'es to ZeroMQ
 *
 */

static void zeroMqSetupPublisher (void) {
  int rc;

  if (server.zeromq_publish_bind == NULL) return;

  publish_sock = zmq_socket(context, ZMQ_PUB);
  if (publish_sock == NULL) {
    int err = zmq_errno();
    redisLog(REDIS_WARNING, "Failed to create the publisher socket: (%d) %s", err, zmq_strerror(err));
    return;
  }

  rc = zmq_bind(publish_sock, server.zeromq_publish_bind);
  if (rc == -1) {
    int err = zmq_errno();
    redisLog(REDIS_WARNING, "Failed to create the publisher socket: (%d) %s", err, zmq_strerror(err));
    zmq_close(publish_sock);
    publish_sock = NULL;
    return;
  }

  redisLog(REDIS_NOTICE, "ZeroMQ publisher socket listening at %s", server.zeromq_publish_bind);
}

void zeroMqPublish(robj *channel, robj *message) {
  int rc;

  if (publish_sock == NULL) return;

  redisAssertWithInfo(NULL,channel,channel->type == REDIS_STRING && message->type == REDIS_STRING);

  rc = zmq_send(publish_sock, (void*)channel->ptr, sdslen(channel->ptr), ZMQ_SNDMORE);
  if (rc == -1) {
   int err = zmq_errno();
   redisLog(REDIS_DEBUG, "ZeroMQ Publish - failed to send channel: (%d) %s", err, zmq_strerror(err));
   return;
  }

  rc = zmq_send(publish_sock, (void*)message->ptr, sdslen(message->ptr), 0);
  if (rc == -1) {
    // FIXME: how to cancel the send of the first part?
   int err = zmq_errno();
   redisLog(REDIS_DEBUG, "ZeroMQ Publish - failed to send the message: (%d) %s", err, zmq_strerror(err));
   return;
  }

  return;
}
