void __ASSERT() {}

int lock = 0 ;
#define assert(e) if (!(e)) __ASSERT()
#define acquire_lock() while (!__sync_bool_compare_and_swap( &lock,0, 1)) {}
#define release_lock() lock = 0

int state = 0;

void thr1() {
  acquire_lock();
  state = 1;
  // begin: critical section
  assert(state == 1);
  // end: critical section
  release_lock();
}

void thr2() {
  acquire_lock();
  // begin: critical section
  state = 2;
  assert(state == 2);
  // end: critical section
  release_lock();
}

