
//
// emitter.c
//
// Copyright (c) 2014 Stephen Mathieson <me@stephenmathieson.com>
//

#include <stdlib.h>
#include <string.h>
#include "emitter.h"

/**
 * Listener type.
 */

typedef enum {
  LISTENER_TYPE_ON,
  LISTENER_TYPE_ONCE
} listener_type_t;

/**
 * Listener.
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

  /**
   * The type of listener.
   */

  listener_type_t type;
} listener_t;

/**
 * Predeclare helper functions.
 */

static int add_listener(
    emitter_t *self
  , const char *event
  , emitter_cb *cb
  , listener_type_t type
);

static int remove_listener(emitter_t *self, listener_t *listener);

/**
 * Create a new listener.
 */

static listener_t *
listener_new(const char *event, emitter_cb *fn) {
  listener_t *self = malloc(sizeof(listener_t));
  if (self) {
    self->fn = fn;
    self->event = event;
    self->type = LISTENER_TYPE_ON; // default value
  }
  return self;
}

/**
 * Default event-name comparator.
 */

static int
default_cmp(const char *a, const char *b) {
  return strcmp(a, b);
}

/**
 * Create a new emitter.
 */

emitter_t *
emitter_new() {
  emitter_t *self = malloc(sizeof(emitter_t));
  if (self) {
    self->cmp = default_cmp;
    list_t *listeners = self->listeners = list_new();
    if (listeners) listeners->free = free;
  }
  return self;
}

/**
 * Destroy an emitter.
 */

void
emitter_destroy(emitter_t *self) {
  if (self->listeners) list_destroy(self->listeners);
  free(self);
}

/**
 * Emit an event.
 */

int
emitter_emit(emitter_t *self, const char *event, void *data) {
  list_iterator_t *iterator = list_iterator_new(self->listeners, LIST_TAIL);
  if (NULL == iterator) return -1;

  list_node_t *node;
  while ((node = list_iterator_next(iterator))) {
    listener_t *listener = (listener_t *) node->val;
    if (0 == self->cmp(listener->event, event)) {
      listener->fn(data);
      if (LISTENER_TYPE_ONCE == listener->type) {
        remove_listener(self, listener);
      }
    }
  }

  list_iterator_destroy(iterator);
  return 0;
}

/**
 * Remove `listener` from `self`.
 */

static int
remove_listener(emitter_t *self, listener_t *listener) {
  list_iterator_t *iterator = list_iterator_new(self->listeners, LIST_TAIL);
  if (NULL == iterator) return -1;

  list_node_t *node;
  while ((node = list_iterator_next(iterator))) {
    if(listener != (listener_t *) node->val)
      continue;

    list_remove(self->listeners, node);
  }

  list_iterator_destroy(iterator);
  return 0;
}

/**
 * Add a listener for `event` to `self`.
 */

static int
add_listener(emitter_t *self
      , const char *event
      , emitter_cb *cb
      , listener_type_t type) {
  listener_t *listener = NULL;
  list_node_t *node = NULL;
  if (!(listener = listener_new(event, cb))) return -1;
  if (!(node = list_node_new(listener))) return -1;
  if (NULL == list_rpush(self->listeners, node)) return -1;
  listener->type = type;
  return 0;
}

int
emitter_on(emitter_t *self, const char *event, emitter_cb *cb) {
  return add_listener(self, event, cb, LISTENER_TYPE_ON);
}

int
emitter_once(emitter_t *self, const char *event, emitter_cb *cb) {
  return add_listener(self, event, cb, LISTENER_TYPE_ONCE);
}

int
emitter_off(emitter_t *self, const char *event, emitter_cb *cb) {
  list_iterator_t *iterator = list_iterator_new(self->listeners, LIST_TAIL);
  if (NULL == iterator) return -1;

  list_node_t *node;
  while ((node = list_iterator_next(iterator))) {
    listener_t *listener = (listener_t *) node->val;
    if (strcmp(event, listener->event)) continue;

    if (cb) {
      if (cb == listener->fn) {
        list_remove(self->listeners, node);
        break;
      }
    } else {
      list_remove(self->listeners, node);
    }
  }

  list_iterator_destroy(iterator);
  return 0;
}
