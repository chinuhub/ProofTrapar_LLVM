void __ASSERT() {}

//Symmetry-Aware Predicate Abstraction for Shared-Variable Concurrent Programs (Extended Technical Report). CoRR abs/1102.2330 (2011)

int lock = 0 ;
#define assert(e) if (!(e)) __ASSERT()
#define acquire_lock() while (!__sync_bool_compare_and_swap( &lock,0, 1)) {}
#define release_lock() lock = 0


int r = 0;
int s = 0;


void thr1(){

  int l = 0;
  
  acquire_lock();
  r = r + 1;
  release_lock();
  
  if(r == 1){
  
  	while(1){
 
  		s = s + 1;
    		l = l + 1;
    		assert(s == l);
    	
 	 }
  }

}

void thr2(){

  int l = 0;
  
  acquire_lock();
  r = r + 1;
  release_lock();
  
  if(r == 1){
  
  	while(1){
 
  		s = s + 1;
    		l = l + 1;
    		assert(s == l);
    	
 	 }
  }

}

