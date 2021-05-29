void __ASSERT() {}

int lock = 0 ;
#define assert(e) if (!(e)) __ASSERT()
#define acquire_lock() while (!__sync_bool_compare_and_swap( &lock,0, 1)) {}
#define release_lock() lock = 0

int i = 3, j = 6;

#define NUM 5
#define LIMIT (2*NUM+6)

void t1() {
  for (int k = 0; k < NUM; k++) {
    acquire_lock();
    i = j + 1;
    assert(i==j+1);
    release_lock();

  }

}

void t2() {
  for (int k = 0; k < NUM; k++) {
    acquire_lock();
    j = i + 1;
    assert(j==i+1); 
    release_lock();
  }

}
