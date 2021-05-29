void __ASSERT() {}

int lock = 0 ;
#define assert(e) if (!(e)) __ASSERT()
#define acquire_lock() while (!__sync_bool_compare_and_swap( &lock,0, 1)) {}
#define release_lock() lock = 0

int data = 0;
int res =0;
int res2 =0;
void thread1()
{
  acquire_lock();
  data++;
  res = data ;
  assert(res = data);
  release_lock();
}


void thread2()
{
  acquire_lock(); 
  data=data + 2;
  res2 = data;
  assert(res2 = data) ; 
  release_lock();
}

