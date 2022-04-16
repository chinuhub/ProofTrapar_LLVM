void __ASSERT() {}


int MTX = 0; //shared mutex

#define assert(e) if (!(e)) __ASSERT()
#define acquire_lock() while (!__sync_bool_compare_and_swap( &MTX,0, 1)) {}
#define release_lock() MTX = 0


int count = 0; //shared

int COND = 0; //condition variables become flag indicating whether the thread was signaled


void thr1(){
  //Barrier2();
    
  acquire_lock();
  
  count++;
  
  if (count == 3)
  {
    //cnd_broadcast(COND); //pthread_cond_broadcast(&cond);
    COND = 1;
    
    count = 0; 
   }
  else
  {
    //cnd_wait(COND,MTX); //pthread_cond_wait(&cond, &m);
    
    release_lock();
     
    //assume(c);
    
    COND = 0; 
    
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
    COND = 1;
    
    count = 0; 
   }
  else
  {
    //cnd_wait(COND,MTX); //pthread_cond_wait(&cond, &m);
    
    release_lock();
     
    //assume(c);
    
    COND = 0; 
    
    acquire_lock();
  } 
      
  release_lock();
  
  assert(0);

} //should not fail for <3 threads

