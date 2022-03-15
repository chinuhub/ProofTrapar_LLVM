void __ASSERT() {}

/* Testcase from Threader's distribution. For details see:
   http://www.model.in.tum.de/~popeea/research/threader
*/

//#include <pthread.h>
#define assert(e) if (!(e)) __ASSERT()

int l=0;
#define acquire_lock() while (!__sync_bool_compare_and_swap( &l,0, 1)) {}
#define release_lock() l = 0

int w, r, x, y;

void writer() {

  acquire_lock();
  if(w==0 && r==0){
     w = 1;
     release_lock();
     x = 3;
     w = 0;
  }
  else{
     release_lock();
  }
  
}

void reader() {
  
  acquire_lock();
  if(w==0){
     r = r+1;
     release_lock();
     y = x;
     assert(y==x);
     r = r-1;
  }
  else{
     release_lock();
  }
  
}

void writer2() {

  acquire_lock();
  if(w==0 && r==0){
     w = 1;
     release_lock();
     x = 3;
     w = 0;
  }
  else{
     release_lock();
  }
  
}

void reader2() {
  
  acquire_lock();
  if(w==0){
     r = r+1;
     release_lock();
     y = x;
     assert(y==x);
     r = r-1;
  }
  else{
     release_lock();
  }
  
}


