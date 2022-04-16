void __ASSERT() {}


#define assert(e) if (!(e)) __ASSERT()

int mThread=0;
int start_main=0;
int mStartLock=0;
int __COUNT__ =0;

#define acquire_lock() while (!__sync_bool_compare_and_swap( &mStartLock,0, 1)) {}
#define release_lock() mStartLock = 0



void thr1() { //nsThread::Init (mozilla/xpcom/threads/nsThread.cpp 1.31)

  int PR_CreateThread__RES = 1;
  
  acquire_lock();
  
  start_main=1;
  
  //__VERIFIER_atomic_thr1(PR_CreateThread__RES);
  
  if( __COUNT__ == 0 ) { // atomic check(0);
	
	mThread = PR_CreateThread__RES; 
	__COUNT__ = __COUNT__ + 1; 
  }
  else{ 
      	assert(0);
  } 
  
  release_lock();

}


void thr2() { //nsThread::Main (mozilla/xpcom/threads/nsThread.cpp 1.31)

  int self = mThread;
  
  while (start_main==0);
  
  acquire_lock();
  
  release_lock();
  
  //__VERIFIER_atomic_thr2(self);
  
  
  if( __COUNT__ == 1 ) { // atomic check(1);
	
	__COUNT__ = __COUNT__ + 1;
  } 
  else 
  { 
      assert(0);
  } 

}

