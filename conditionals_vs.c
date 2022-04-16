void __ASSERT() {}

int m = 0 ;
#define assert(e) if (!(e)) __ASSERT()
#define acquire_lock() while (!__sync_bool_compare_and_swap( &m,0, 1)) {}
#define release_lock() m = 0

int z;

void thr1()
{
	int x;
	int y;

	x = 2;
	y = 3;

	acquire_lock();
	
	if(x == y)
	{
		z = 0;
	}
	else {
		z = 1;
	}

	if(z == 0)
	{
		assert(x == y);
	}
	else {
		assert(x != y);
	}
	
	release_lock();
}


void thr2()
{
	int x;
	int y;

	x = 3;
	y = 3;

	acquire_lock();
	
	if(x == y)
	{
		z = 0;
	}
	else {
		z = 1;
	}

	if(z == 0)
	{
		assert(x == y);
	}
	else {
		assert(x != y);
	}
	
	release_lock();
}

