
#include <stdio.h>
#include <assert.h>
#include "src/emitter.h"

#define TEST(name) \
  static void test_##name(void)

#define SUITE(title) \
  printf("\n  \e[36m%s\e[0m\n", title)

#define RUN_TEST(test) \
  test_##test(); \
  puts("    \e[92m✓ \e[90m" #test "\e[0m");

TEST(new_emitter) {
  emitter_t *emitter = emitter_new();
  assert(emitter);
  assert(emitter->listeners);
  assert(0 == emitter->listeners->len);
  emitter_destroy(emitter);
}

static int invocations = 0;

static void
on_foo(void *data) {
  ++invocations;
  // make gcc shutup:
  data = NULL;
}

typedef struct {
  int a;
  int b;
  int c;
} bar_data_t;

static void
on_bar(void *data) {
  bar_data_t *thing = (bar_data_t *) data;
  thing->c = thing->a + thing->b;
}

TEST(on_new_event) {
  emitter_t *emitter = emitter_new();
  assert(emitter);
  assert(0 == emitter_on(emitter, "foo", &on_foo));
  emitter_destroy(emitter);
}

TEST(emit_unregistered_event) {
  emitter_t *emitter = emitter_new();
  assert(emitter);
  assert(0 == emitter_emit(emitter, "foo", NULL));
  emitter_destroy(emitter);
}

TEST(register_and_emit) {
  emitter_t *emitter = emitter_new();
  assert(emitter);
  assert(0 == emitter_on(emitter, "foo", &on_foo));
  for (int i = 0; i < 10; i++)
    assert(0 == emitter_emit(emitter, "foo", NULL));
  assert(10 == invocations);
  emitter_destroy(emitter);
}

TEST(emit_event_with_data) {
  emitter_t *emitter = emitter_new();
  assert(emitter);
  assert(0 == emitter_on(emitter, "bar", &on_bar));
  bar_data_t *bar = malloc(sizeof(bar_data_t));
  bar->a = 1;
  bar->b = 14;
  assert(0 == emitter_emit(emitter, "bar", (void *) bar));
  assert(15 == bar->c);
  emitter_destroy(emitter);
}

int
main() {
  SUITE("emitter");
  RUN_TEST(new_emitter);
  RUN_TEST(on_new_event);
  RUN_TEST(emit_unregistered_event);
  RUN_TEST(register_and_emit);
  RUN_TEST(emit_event_with_data);
  printf("\n");
  return 0;
}
