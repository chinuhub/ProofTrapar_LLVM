void __ASSERT() {}

/* Testcase from Threader's distribution. For details see:
   http://www.model.in.tum.de/~popeea/research/threader
*/


int lock = 0 ;
#define assert(e) if (!(e)) __ASSERT()
#define acquire_lock() while (!__sync_bool_compare_and_swap( &lock,0, 1)) {}
#define release_lock() lock = 0

int x=1;

void thr1() {
  acquire_lock();
  x = 0;
  x = x+1;
  assert(x>0);
  release_lock();
}


void thr2() {
  acquire_lock();
  x = 0;
  x = x+1;
  assert(x>0);
  release_lock();
}
