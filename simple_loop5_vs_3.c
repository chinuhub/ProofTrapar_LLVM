void __ASSERT() {}

int mutex = 0 ;
#define assert(e) if (!(e)) __ASSERT()
#define acquire_lock() while (!__sync_bool_compare_and_swap( &mutex,0, 1)) {}
#define release_lock() mutex = 0


int a = 1;
int b = 2;
int c = 3;
int temp;


void thr2()
{
  for(;;){
  
    acquire_lock();
    
    temp = a;
    a = b;
    b = c;
    c = temp;
    
    release_lock();
  }

}


void thr3()
{
  for(;;){
  
    acquire_lock();
    
    temp = a;
    a = b;
    b = c;
    c = temp;
    
    release_lock();
  }

}


void thr1()
{
  while(1)
  {
    acquire_lock();
    
    assert(a != b);
    
    release_lock();
  }

}

