void __ASSERT() {}


//http://www.ibm.com/developerworks/java/library/j-jtp11234/
//Listing 5. Implementing a thread-safe PRNG with synchronization and atomic variables

int lock = 0 ;
#define assert(e) if (!(e)) __ASSERT()
#define acquire_lock() while (!__sync_bool_compare_and_swap( &lock,0, 1)) {}
#define release_lock() lock = 0


int seed; 

#define PseudoRandomUsingAtomic_monitor(){\
	while(1)\
	{\
		assert(seed != 0);\
	}\
}


#define STATE_UNINITIALIZED	0
#define STATE_INITIALIZED	1

int state = STATE_UNINITIALIZED;


void thr1()
{
	acquire_lock();
	
	switch(state)
	{
	case STATE_UNINITIALIZED: 
		
		seed = 1;
		
		state = STATE_INITIALIZED;
		
		release_lock();
		
		PseudoRandomUsingAtomic_monitor(); //never returns
		
		break;
		
	case STATE_INITIALIZED: 
	
		release_lock();
		
		int myrand;
	
		int n = 10;
	 
		int read, nexts, nextInt_return;

		acquire_lock();
	
		read = seed;

		nexts = 5;
	
		assert(nexts != read); 
		seed = nexts;
	
		release_lock();
	
		nextInt_return = nexts % n;
		//assume(nexts < n + 1);
		//nextInt_return = nexts;

		myrand = nextInt_return;
	
		assert(myrand <= 10);
		
		break;
	}
}




void thr2()
{
	acquire_lock();
	
	switch(state)
	{
	case STATE_UNINITIALIZED: 
		
		seed = 1;
		
		state = STATE_INITIALIZED;
		
		release_lock();
		
		PseudoRandomUsingAtomic_monitor(); //never returns
		
		break;
		
	case STATE_INITIALIZED: 
	
		release_lock();
		
		int myrand;
	
		int n = 10;
	 
		int read, nexts, nextInt_return;

		acquire_lock();
	
		read = seed;

		nexts = 5;
	
		assert(nexts != read); 
		seed = nexts;
	
		release_lock();
	
		nextInt_return = nexts % n;
		//assume(nexts < n + 1);
		//nextInt_return = nexts;

		myrand = nextInt_return;
	
		assert(myrand <= 10);
		
		break;
	}
}


