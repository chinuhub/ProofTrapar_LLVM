void __ASSERT() {}

int lock = 0 ;
#define assert(e) if (!(e)) __ASSERT()
#define acquire_lock() while (!__sync_bool_compare_and_swap( &lock,0, 1)) {}
#define release_lock() lock = 0

void thr1()
{
	int x;
	int y;

	x = 1;
	y = 1;

	int z;
	acquire_lock();
	if(x == y)
	{
		z = 0;
	} else {
		z = 1;
	}

	if(z == 0)
	{
		assert(x == y);
	} else {
		assert(x != y);
	}
	release_lock();
}


void thr2()
{
	int x;
	int y;

	x = 1;
	y = 0;

	int z;
	acquire_lock();
	if(x == y)
	{
		z = 0;
	} else {
		z = 1;
	}

	if(z == 0)
	{
		assert(x == y);
	} else {
		assert(x != y);
	}
	release_lock();
}