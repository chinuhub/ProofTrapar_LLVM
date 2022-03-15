void __ASSERT() {}

/* Testcase from Threader's distribution. For details see:
   http://www.model.in.tum.de/~popeea/research/threader
*/

//#include <pthread.h>
#define assert(e) if (!(e)) __ASSERT()

int i=1, j=1;

#define NUM 2


void thr1() {

 for (int k=0; k<NUM; k++){
    i+=j;
   }
   
  assert(i<8); 
}


void thr2() {
  
  for (int k=0; k<NUM; k++){
    j+=i;
 }
 
 assert(j<8);
}

