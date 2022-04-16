void __ASSERT() {}


int mutex = 0 ;
#define assert(e) if (!(e)) __ASSERT()
#define acquire_lock() while (!__sync_bool_compare_and_swap( &mutex,0, 1)) {}
#define release_lock() mutex = 0


int g0 = 0,g1 = 0,x = 0;
_Bool lock = 0;


void thr3()
{

  acquire_lock();
  
  if(5)
  {
    g0=0;
    g1=0;
    lock=0;
  }
  
  release_lock();
  
  acquire_lock();
  
  if(lock)
  {
    x=1;
    assert(g0==1 && g1==1);
  }
  
  release_lock();

}


void thr2()
{

  while(1)
  {
    acquire_lock();
    
    assert(g0==g1);
    
    release_lock();
  }

}



void thr1()
{
  acquire_lock();
  
  g0=1;
  g1=1;
  
  release_lock();
  
  lock=1;

}

