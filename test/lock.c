void __ASSERT() {}

#define assert(e) if (!(e)) __ASSERT()
#define acquire_lock(l) while (!__sync_bool_compare_and_swap( &l,0, 1)) {}
#define release_lock(l) l = 0

int lock = 0 ;

void thr1() {
  acquire_lock(lock);
  // begin: critical section
  assert(lock == 1);
  // end: critical section
  release_lock(lock);
}

void thr2() {
  acquire_lock(lock);
  // begin: critical section
  assert(lock == 1);
  // end: critical section
  release_lock(lock);
}

