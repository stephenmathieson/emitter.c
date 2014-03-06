
#include <stdlib.h>
#include <string.h>
#include "emitter.h"

/**
 * Private listener type.
 */

typedef struct {

  /**
   * The listener callback.
   */

  emitter_cb *fn;

  /**
   * The event to listen for.
   */

  const char *event;

} listener_t;

/**
 * Create a new listener.
 */

static listener_t *
listener_new(const char *event, emitter_cb *fn) {
  listener_t *self = malloc(sizeof(listener_t));
  if (self) {
    self->fn = fn;
    self->event = event;
  }
  return self;
}

emitter_t *
emitter_new() {
  emitter_t *self = malloc(sizeof(emitter_t));
  if (self) self->listeners = list_new();
  return self;
}

void
emitter_destroy(emitter_t *self) {
  if (self->listeners) list_destroy(self->listeners);
  free(self);
}

int
emitter_emit(emitter_t *self, const char *event, void *data) {
  list_iterator_t *iterator = list_iterator_new(self->listeners, LIST_TAIL);
  if (NULL == iterator) return -1;

  list_node_t *node;
  while ((node = list_iterator_next(iterator))) {
    listener_t *listener = (listener_t *) node->val;
    if (0 == strcmp(event, listener->event)) listener->fn(data);
  }

  list_iterator_destroy(iterator);
  return 0;
}

int
emitter_on(emitter_t *self, const char *event, emitter_cb *cb) {
  listener_t *listener = NULL;
  list_node_t *node = NULL;
  if (!(listener = listener_new(event, cb))) return -1;
  if (!(node = list_node_new(listener))) return -1;
  if (NULL == list_rpush(self->listeners, node)) return -1;
  return 0;
}
