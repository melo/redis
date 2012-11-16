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
static voic *publish_socket;

/* Initialize the ZeroMQ context, create the sockets and connect/bind them */
void zeroMqInit(void) {
  context = zmq_ctx_new();
  if (context == NULL) {
    // FIXME: log the error
    return;
  }
}

