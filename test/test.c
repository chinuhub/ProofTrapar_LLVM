void __ASSERT() {}
#define assert(e) if (!(e)) __ASSERT()
int state=0;

void thr1() {

while(state !=0) ;
state = 1 ;
assert(state == 1);
state = 0;

}

void thr2() {
  state = 2 ;
  assert(state == 2);
  state = 0;
}
