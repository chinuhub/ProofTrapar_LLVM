//Simple test_and_set lock with exponential backoff
//
//From Algorithms for Scalable Synchronization on Shared-Memory Multiprocessors, 1991 (Fig. 1).
//Also available as pseudo-code here: http://www.cs.rochester.edu/research/synchronization/pseudocode/ss.html#tas
void __ASSERT() {}


#define assert(e) if (!(e)) __ASSERT()


#define unlocked 0
#define locked 1
int lock = unlocked;


void VERIFIER_atomic_TAS(int *v, int *o){
	*o = *v;
	*v = 1;
}

void acquire_lock(){
	int delay;
	int cond;

	delay = 1;
	
	VERIFIER_atomic_TAS(&lock,&cond);
	
	while(cond == locked){
		//pause(delay);
		if(delay*2 > delay) 
			delay *= 2;
		VERIFIER_atomic_TAS(&lock,&cond);
	}
	assert(cond != lock);
}

void release_lock(){
	assert(lock != unlocked);
	lock = unlocked; 
}

int c = 0;


void thr1(){

	while(1){
		acquire_lock();
		c++;
		assert(c == 1);
		c--;
		release_lock();
	}
}

void thr2(){

	while(1){
		acquire_lock();
		c++;
		assert(c == 1);
		c--;
		release_lock();
	}
}

