void __ASSERT() {}
#include<stdbool.h>
#define assert(e) if (!(e)) __ASSERT()
 int state=0;

void thr1() {
  while (!__sync_bool_compare_and_swap( &state,0, 1)) {};
  // begin: critical section
  assert(state == 1);
  // end: critical section
  state = 0;
}

void thr2() {
  while (!__sync_bool_compare_and_swap( &state,0, 2)) {};
  // begin: critical section
  assert(state == 2);
  // end: critical section
  state = 0;
}


