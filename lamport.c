void __ASSERT() {}

/* Testcase from Threader's distribution. For details see:
   http://www.model.in.tum.de/~popeea/research/threader
*/

#define assert(e) if (!(e)) __ASSERT()

int x, y;
int b1, b2; // boolean flags
int X; // boolean variable to test mutual exclusion

void thr1() {
    while (1) {
        b1 = 1;
        x = 1;
        int y1 = y;
        if (y1 != 0) {
            b1 = 0;
            y1 = y;
            while (y1 != 0) {
                y1 = y;
            };
            continue;
        }
        y = 1;
        int x1 = x;
        if (x1 != 1) {
            b1 = 0;
            int b21 = b2;
            while (b21 >= 1) {
                b21 = b2;
            };
            y1 = y;
            if (y1 != 1) {
                y1 = y;
                while (y1 != 0) {
                    y1 = y;
                };
                continue;
            }
        }
        break;
    }
    // begin: critical section
    X = 0;
    assert(X <= 0);
    // end: critical section
    y = 0;
    b1 = 0;
}

void thr2() {
    while (1) {
        b2 = 1;
        x = 2;
        int y2 = y;
        if (y2 != 0) {
            b2 = 0;
            y2 = y;
            while (y2 != 0) {
                y2 = y;
            };
            continue;
        }
        y = 2;
        int x2 = x;
        if (x2 != 2) {
            b2 = 0;
            int b12 = b1;
            while (b12 >= 1) {
                b12 = b1;
            };
            y2 = y;
            if (y2 != 2) {
                y2 = y;
                while (y2 != 0) {
                    y2 = y;
                };
                continue;
            }
        }
        break;
    }
    // begin: critical section
    X = 1;
    assert(X >= 1);
    // end: critical section
    y = 0;
    b2 = 0;;
}
