void __ASSERT() {}

/* Testcase from Threader's distribution. For details see:
   http://www.model.in.tum.de/~popeea/research/threader
*/

#define assert(e) if (!(e)) __ASSERT()

int flag1 = 0, flag2 = 0; // boolean flags
int turn; // integer variable to hold the ID of the thread whose turn is it
int x; // boolean variable to test mutual exclusion

void thr1() {
    flag1 = 1;
    int f2 = flag2;
    while (f2 >= 1) {
        int t = turn;
        if (t != 0) {
            flag1 = 0;
            t = turn;
            while (t != 0) {
                t = turn;
            };
            flag1 = 1;
        }
        f2 = flag2;
    }
    // begin: critical section
    x = 0;
    assert(x<=0);
    // end: critical section
    turn = 1;
    flag1 = 0;
}

void thr2() {
    flag2 = 1;
    int f1 = flag1;
    while (f1 >= 1) {
        int t = turn;
        if (t != 1) {
            flag2 = 0;
            t = turn;
            while (t != 1) {
                t = turn;
            };
            flag2 = 1;
        }
        f1 = flag1;
    }
    // begin: critical section
    x = 1;
    assert(x>=1);
    // end: critical section
    turn = 1;
    flag2 = 0;
}
