
//
// emitter.h
//
// Copyright (c) 2014 Stephen Mathieson <me@stephenmathieson.com>
//

#ifndef EMITTER_H
#define EMITTER_H 1

#include "list/list.h"

/**
 * Callback type.
 */

typedef void (emitter_cb)(void *data);

/**
 * Emitter type.
 */

typedef struct {

  /**
   * Registered listeners.
   */

  list_t *listeners;

} emitter_t;

/**
 * Create a new emitter.
 */

emitter_t *
emitter_new();

/**
 * Destroy an emitter, freeing its memory.
 */

void
emitter_destroy(emitter_t *self);

/**
 * Emit an `event` on the given emitter, passing
 * `data` to all registered listeners.
 *
 * Returns -1 on failure.
 */

int
emitter_emit(emitter_t *self, const char *event, void *data);

/**
 * Register listener `cb` for `event`.
 *
 * Returns -1 on failure.
 */

int
emitter_on(emitter_t *self, const char *event, emitter_cb *cb);

#endif
