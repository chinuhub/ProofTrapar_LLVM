extern int __VERIFIER_nondet_int(void);

void __ASSERT() {}

#define assert(e) if (!(e)) __ASSERT()



int usecount;
int locked;
int flag1 = 0;
int flag2 = 0;
int release_thr1 = 0;


void thr2 () //dummy_open
{
  while(!release_thr1);

  usecount = usecount + 1;

  if (locked){
  
  }
  else{
  
  	locked = 1;
  	flag1 = 1;
  }
}



void thr1 ()
{
  int rval;
  int count;

  usecount = 0;
  locked = 0;
  
  release_thr1 = 1;
  while(1)
  {
    if(flag1)
      break;
  }

  do
    {
      rval = __VERIFIER_nondet_int();
      
      if (rval == 0)
	{
	  count = 1;
	  
	  usecount = usecount - 1;
  	  locked = 0;

	}
      else
	count = 0;
    }
  while(count != 0);

  usecount = usecount - 1;
  locked = 0;

 
  if (usecount != 0)
  {
    	// this should fail
    	assert (0);
  }
 
}

