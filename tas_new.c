void __ASSERT() {}

//Simple test_and_set lock with exponential backoff
//
//From Algorithms for Scalable Synchronization on Shared-Memory Multiprocessors, 1991 (Fig. 1).
//Also available as pseudo-code here: http://www.cs.rochester.edu/research/synchronization/pseudocode/ss.html#tas

int mutex = 0 ;
#define assert(e) if (!(e)) __ASSERT()
#define acquire_lock() while (!__sync_bool_compare_and_swap( &mutex,0, 1)) {}
#define release_lock() mutex = 0

#define unlocked 0
#define locked 1

int lock = unlocked;

int c = 0;

void thr1(){

	while(1){
		
	int delay;
	int cond;

	delay = 1;
	
	acquire_lock();
	cond = lock;
	lock = 1;
	release_lock();
	
	while(cond == locked){
	
		if(delay*2 > delay){
			delay = delay * 2;
		}
		
		acquire_lock();
		cond = lock;
		lock = 1;
		release_lock();	
	}
	
	assert(cond != lock);
		
	c++;
	assert(c == 1);
	c--;
	assert(lock != unlocked);
	lock = unlocked; 
	}
}
