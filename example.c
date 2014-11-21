
#include <stdio.h>
#include "src/emitter.h"

typedef struct {
  int a;
  int b;
  int c;
} numbers_t;

void
once_foo(void *data) {
  printf("once_foo called\n");
}

void
on_foo(void *data) {
  numbers_t *num = (numbers_t *) data;

  printf("on_foo:\n"
         "  a: %d\n"
         "  b: %d\n"
         "  c: %d\n"
       , num->a
       , num->b
       , num->c);
}

void
on_bar(void *data) {
  char *msg = (char *) data;
  printf("on_bar: %s\n", msg);
}

int
main(void) {
  emitter_t *emitter = emitter_new();

  emitter_once(emitter, "foo", &once_foo);

  emitter_on(emitter, "foo", &on_foo);
  emitter_on(emitter, "bar", &on_bar);

  emitter_emit(emitter, "bar", "abcd");
  emitter_emit(emitter, "bar", "ok!");

  emitter_emit(emitter, "foo", &(numbers_t) {1,2,3});
  emitter_emit(emitter, "foo", &(numbers_t) {4,5,6});

  emitter_destroy(emitter);

  return 0;
}