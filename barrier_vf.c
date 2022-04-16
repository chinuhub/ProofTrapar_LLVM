void __ASSERT() {}


int MTX = 0; //shared mutex

#define assert(e) if (!(e)) __ASSERT()
#define acquire_lock() while (!__sync_bool_compare_and_swap( &MTX,0, 1)) {}
#define release_lock() MTX = 0

int lock = 0 ;
#define acquire_lock2() while (!__sync_bool_compare_and_swap( &lock,0, 1)) {}
#define release_lock2() lock = 0


int count = 0; //shared

int COND = 0; //condition variables become flag indicating whether the thread was signaled


void thr1(){

  //Barrier2();
  
  acquire_lock();
    
  count++;
  if (count == 3)
  {
    //cnd_broadcast(COND); //pthread_cond_broadcast(&cond);
    
  acquire_lock2();
  COND = 1; 
  release_lock2(); 
  count = 0;
  }
  else
  {
    //cnd_wait(COND,MTX); //pthread_cond_wait(&cond, &m);
    
  release_lock(); 
  
  acquire_lock2(); 
  COND = 0; 
  release_lock2();  
  
  acquire_lock(); 
  }  
    
  release_lock();
  
  assert(0);

} //should not fail for <3 threads



void thr2(){

  //Barrier2();
  
  acquire_lock();
    
  count++;
  if (count == 3)
  {
    //cnd_broadcast(COND); //pthread_cond_broadcast(&cond);
    
  acquire_lock2();
  COND = 1; 
  release_lock2(); 
  count = 0;
  }
  else
  {
    //cnd_wait(COND,MTX); //pthread_cond_wait(&cond, &m);
    
  release_lock(); 
  
  acquire_lock2(); 
  COND = 0; 
  release_lock2();  
  
  acquire_lock(); 
  }  
    
  release_lock();
  
  assert(0);

} //should not fail for <3 threads
