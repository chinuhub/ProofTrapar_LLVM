void __ASSERT() {}


int mutexa = 0 ;
#define assert(e) if (!(e)) __ASSERT()
#define acquire_lock1() while (!__sync_bool_compare_and_swap( &mutexa,0, 1)) {}
#define release_lock1() mutexa = 0

int mutexb = 0 ;
#define assert(e) if (!(e)) __ASSERT()
#define acquire_lock2() while (!__sync_bool_compare_and_swap( &mutexb,0, 1)) {}
#define release_lock2() mutexb = 0


int count = 0;


void thr1()
{
  while(1)
  {
    acquire_lock1();
    
    assert(count >= -1);
    
    acquire_lock2();
    
    assert(count == 0);
    
    release_lock2();
    
    release_lock1();
    
  }
 
}

void thr2()
{

  int t2 = 3;

  if(t2==3){
  
  	acquire_lock1();
  	count = count +1;
  	count = count -1; 	
  	release_lock1();
    
  }
  else{
  
  	acquire_lock2();
  	count = count -1;
  	count = count +1;
  	release_lock2();
    
   }

}
