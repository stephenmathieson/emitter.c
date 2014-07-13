
#include <stdio.h>
#include <assert.h>
#include "emitter.h"
#include "wildcardcmp/wildcardcmp.h"

#define TEST(name) \
  static void test_##name(void)

#define RUN_TEST(test) \
  test_##test(); \
  puts("    \e[92m✓ \e[90m" #test "\e[0m");

static int invocations = 0;

typedef struct {
  int a;
  int b;
  int c;
} bar_data_t;

static void
on_foo(void *data) {
  ++invocations;
}

static void
on_bar(void *data) {
  bar_data_t *thing = (bar_data_t *) data;
  thing->c = thing->a + thing->b;
}

static int
cmp_fn(const char *a, const char *b) {
  return !wildcardcmp(a, b);
}

TEST(new_emitter) {
  emitter_t *emitter = emitter_new();
  assert(emitter);
  assert(emitter->listeners);
  assert(0 == emitter->listeners->len);
  emitter_destroy(emitter);
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
  invocations = 0;
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
  bar_data_t bar = { 1, 14, 0 };
  assert(0 == emitter_emit(emitter, "bar", (void *) &bar));
  assert(15 == bar.c);
  emitter_destroy(emitter);
}

TEST(on_then_off) {
  emitter_t *emitter = emitter_new();
  assert(emitter);
  assert(0 == emitter_on(emitter, "bar", &on_bar));
  assert(1 == emitter->listeners->len);
  assert(0 == emitter_off(emitter, "bar", &on_bar));
  assert(0 == emitter->listeners->len);
  emitter_destroy(emitter);
}

TEST(off_actually_removes) {
  emitter_t *emitter = emitter_new();
  assert(emitter);

  assert(0 == emitter_on(emitter, "foo", &on_foo));
  assert(1 == emitter->listeners->len);

  invocations = 0;
  for (int i = 0; i < 10; i++) {
    if (5 == i) assert(0 == emitter_off(emitter, "foo", &on_foo));
    assert(0 == emitter_emit(emitter, "foo", NULL));
  }
  assert(5 == invocations);

  emitter_destroy(emitter);
}

TEST(off_all_listeners) {
  emitter_t *emitter = emitter_new();
  assert(emitter);

  assert(0 == emitter_on(emitter, "foo", &on_foo));
  assert(0 == emitter_on(emitter, "foo", &on_bar));
  assert(2 == emitter->listeners->len);

  assert(0 == emitter_on(emitter, "something", &on_foo));
  assert(3 == emitter->listeners->len);

  assert(0 == emitter_off(emitter, "foo", NULL));
  assert(1 == emitter->listeners->len);

  emitter_destroy(emitter);
}

TEST(custom_cmp_function) {
  emitter_t *emitter = emitter_new();
  assert(emitter);
  emitter->cmp = cmp_fn;
  invocations = 0;
  assert(0 == emitter_on(emitter, "foo*", &on_foo));

  assert(0 == emitter_emit(emitter, "foo", NULL));
  assert(0 == emitter_emit(emitter, "foobar", NULL));
  assert(0 == emitter_emit(emitter, "foobaz", NULL));
  assert(0 == emitter_emit(emitter, "blahfoo", NULL));

  assert(3 == invocations);
  emitter_destroy(emitter);
}

int
main() {
  printf("\n  \e[36m%s\e[0m\n", "emitter");
  RUN_TEST(new_emitter);
  RUN_TEST(on_new_event);
  RUN_TEST(emit_unregistered_event);
  RUN_TEST(register_and_emit);
  RUN_TEST(emit_event_with_data);
  RUN_TEST(on_then_off);
  RUN_TEST(off_actually_removes);
  RUN_TEST(off_all_listeners);
  RUN_TEST(custom_cmp_function);
  printf("\n");
  return 0;
}
