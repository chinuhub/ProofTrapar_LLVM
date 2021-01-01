void __ASSERT() {}

/* Testcase from Threader's distribution. For details see:
   http://www.model.in.tum.de/~popeea/research/threader
*/

// #include <pthread.h>
#define assert(e) if (!(e)) __ASSERT()

int flag1 = 0, flag2 = 0; // boolean flags
int turn = 0; // integer variable to hold the ID of the thread whose turn is it
int x = 0; // boolean variable to test mutual exclusion

void thr1() {
  flag1 = 1;
  turn = 1;
  while (flag2==1 && turn==1) {};
  // begin: critical section
  x = 0;
  assert(x<=0);
  // end: critical section
  flag1 = 0;
}

void thr2() {
  flag2 = 1;
  turn = 0;
  while (flag1==1 && turn==0) {};
  // begin: critical section
  x = 1;
  assert(x>=1);
  // end: critical section
  flag2 = 0;
}
