void __ASSERT() {}
#define assert(e) if (!(e)) __ASSERT()

int v = 0;
int state = 0 ;
void thread1()
{
  v = 1;
}

void thread2()
{
  if (v) state = 1;
}


