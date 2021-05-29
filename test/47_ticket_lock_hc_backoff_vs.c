void __ASSERT() {}

int lock = 0 ;
#define assert(e) if (!(e)) __ASSERT()
#define acquire_lock() while (!__sync_bool_compare_and_swap( &lock,0, 1)) {}
#define release_lock() lock = 0

int c = 0;

void thr1()
{
  acquire_lock();
  c = 1; assert(c == 1); c = 0;
  release_lock();
}

void thr2()
{
  acquire_lock();
  c = 1; assert(c == 1); c = 0;
  release_lock();
}