extern void __VERIFIER_atomic_begin();
extern void __VERIFIER_atomic_end();

//Ticket lock with proportional backoff
//
//From Algorithms for Scalable Synchronization on Shared-Memory Multiprocessors, 1991 (Fig. 2).
//Also available as pseudo-code here: http://www.cs.rochester.edu/research/synchronization/pseudocode/ss.html#ticket

void __ASSERT() {}

#define assert(e) if (!(e)) __ASSERT()


int next_ticket = 0;
int now_serving = 0;

#define FAILED 3 //this is already and the limit of what we can handle

#define NEXT(e) ((e + 1) % FAILED)
// #define NEXT(e) ((e+1 == FAILED)?0:e+1)

int fetch_and_increment__next_ticket(){

	__VERIFIER_atomic_begin();
	
	int value;

		if(NEXT(next_ticket) == now_serving){ 
			value = FAILED;
		}
		else
		{
			value = next_ticket;
			next_ticket = NEXT(next_ticket);
		}

	__VERIFIER_atomic_end();
	
	return value;
}

void acquire_lock(){

	int my_ticket; 

	my_ticket = fetch_and_increment__next_ticket(); //returns old value; arithmetic overflow is harmless (Alex: it is not if we have 2^64 threads)

	if(my_ticket == FAILED){
	
	}else{
		while(1){
			//pause(my_ticket - now_serving);
			// consume this many units of time
			// on most machines, subtraction works correctly despite overflow
			if(now_serving == my_ticket){
				break;
			}
		}
	}

}

void release_lock(){
	now_serving++;
}


int c = 0;

void thr1(){
	acquire_lock();
	c++;
	assert(c == 1);
	c--;
	release_lock();
}


void thr2(){
	acquire_lock();
	c++;
	assert(c == 1);
	c--;
	release_lock();
}

