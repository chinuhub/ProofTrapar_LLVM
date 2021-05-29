void __ASSERT() {}

int lock = 0 ;
#define assert(e) if (!(e)) __ASSERT()
#define acquire_lock() while (!__sync_bool_compare_and_swap( &lock,0, 1)) {}
#define release_lock() lock = 0


int a = 1;
int b = 2;
int c = 3;
int temp;


void thr1()
{
    acquire_lock();
    temp = a;
    a = b;
    b = c;
    c = temp;
    assert(a!=b&&b!=c&&c!=a);
    release_lock();  
}

void thr2()
{
    acquire_lock();
    temp = a;
    a = b;
    b = c;
    c = temp;
    assert(a!=b&&b!=c&&c!=a);
    release_lock();  
}
