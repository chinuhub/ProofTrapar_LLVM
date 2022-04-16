void __ASSERT() {}

int lock = 0 ;
#define assert(e) if (!(e)) __ASSERT()
#define acquire_lock1() while (!__sync_bool_compare_and_swap( &lock,0, 1)) {}
#define release_lock1() lock = 0


int next_ticket = 0;
int now_serving = 0;

#define FAILED 3 //this is already and the limit of what we can handle

#define NEXT(e) ((e + 1) % FAILED)
// #define NEXT(e) ((e+1 == FAILED)?0:e+1)


int c = 0;

void thr1(){

	int my_ticket; 

	acquire_lock1();
	int value=0;

	if(NEXT(next_ticket) == now_serving){ 
		value = FAILED;
	}
	else
	{
		value = next_ticket;
		next_ticket = NEXT(next_ticket);
	}

	release_lock1();
	
	my_ticket = value;

	if(my_ticket == FAILED){	
	}
	else{
		while(1){

			if(now_serving == my_ticket){
				break;
			}
		}
	}
	c++;
	assert(c == 1);
	c--;
	now_serving++;
}


void thr2(){

	int my_ticket; 

	acquire_lock1();
	int value;

	if(NEXT(next_ticket) == now_serving){ 
		value = FAILED;
	}
	else
	{
		value = next_ticket;
		next_ticket = NEXT(next_ticket);
	}

	release_lock1();
	my_ticket = value;

	if(my_ticket == FAILED){	
	}
	else{
		while(1){

			if(now_serving == my_ticket){
				break;
			}
		}
	}
	c++;
	assert(c == 1);
	c--;
	now_serving++;
}

