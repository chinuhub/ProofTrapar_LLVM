void __ASSERT() {}

/* Testcase from Threader's distribution. For details see:
   http://www.model.in.tum.de/~popeea/research/threader
*/

//#include <pthread.h>
#define assert(e) if (!(e)) __ASSERT()

int l;
#define acquire_lock() while (!__sync_bool_compare_and_swap( &l,0, 1)) {}
#define release_lock() l = 0

int a_in=8;
int b_in =6;;
int a = 8;
int b = 6;


void thr1() {

  while(a!=b) {
 
    acquire_lock();
    if(a>b)
    	a=a-b;
    release_lock();
    
  }

   int c =a_in%a;
   assert(c==0);
}

void thr2() {

  while(a!=b) {
  
    acquire_lock();
    if(b>a)
    	b=b-a;
    release_lock();
  }
  
  int d = b_in%a;
  assert(d==0);
}

