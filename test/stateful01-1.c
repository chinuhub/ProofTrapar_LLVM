void __ASSERT() {}

int lock = 0 ;
#define assert(e) if (!(e)) __ASSERT()
#define acquire_lock() while (!__sync_bool_compare_and_swap( &lock,0, 1)) {}
#define release_lock() lock = 0

int data1=10;
int data2=10;

void  thread1()
{  
  acquire_lock();
  data1++;
  release_lock();
  acquire_lock();
  data2++;
  release_lock();

}


void  thread2()
{  
  acquire_lock();
  data1+=5;
  release_lock();

  acquire_lock();
  data2-=6;
  release_lock();

}


