void __ASSERT() {}

int lock = 0 ;
#define assert(e) if (!(e)) __ASSERT()
#define acquire_lock() while (!__sync_bool_compare_and_swap( &lock,0, 1)) {}
#define release_lock() lock = 0


unsigned value;

void thr1() {

	unsigned v = 0;
	acquire_lock();	
	if(value == 0u-1) {
		release_lock();
	}
	else{
		v = value;
		value = v + 1;
		release_lock();
	        assert(value > v);
	}
}

void thr2() {

	unsigned v = 0;
	acquire_lock();	
	if(value == 0u-1) {
		release_lock();
	}
	else{
		v = value;
		value = v + 1;
		release_lock();
	        assert(value > v);
	}
}

