void __ASSERT() {}

// This file is part of the SV-Benchmarks collection of verification tasks:
// https://github.com/sosy-lab/sv-benchmarks
//
// SPDX-FileCopyrightText: 2016 SCTBench Project
// SPDX-FileCopyrightText: The ESBMC Project
//
// SPDX-License-Identifier: Apache-2.0
// https://github.com/mc-imperial/sctbench/blob/master/benchmarks/concurrent-software-benchmarks/arithmetic_prog_ok.c


int m = 0 ;
#define assert(e) if (!(e)) __ASSERT()
#define acquire_lock() while (!__sync_bool_compare_and_swap( &m,0, 1)) {}
#define release_lock() m = 0

int cond1 = 0;
int lock1 = 0 ;
#define acquire_lock1() while (!__sync_bool_compare_and_swap( &lock1,0, 1)) {}
#define release_lock1() lock1 = 0

int cond2 = 0;
int lock2 = 0 ;
#define acquire_lock2() while (!__sync_bool_compare_and_swap( &lock2,0, 1)) {}
#define release_lock2() lock2 = 0

#define N 4

int num = 0;
int total = 0;
int flag;
int checking = 0;


//pthread_mutex_t m;
//pthread_cond_t empty, full;

void thread1() {

  int i;
  i = 0;
  
  while (i < N)
  {
    //pthread_mutex_lock(&m);
    acquire_lock();
    
    while (num > 0){
      //pthread_cond_wait(&empty, &m);
      
      acquire_lock1();
      
      release_lock();
      
      while(cond1!=1)
      {}
      
      release_lock1();
      
    }
    num++;
    
    //pthread_mutex_unlock(&m);
    release_lock();
    
    //pthread_cond_signal(&full);
    cond2 = 1;
    
    i++;
  }
  
  int l1 = checking;
  checking = 1;
  
  acquire_lock();
  
  if(l1 == 2){
  
  	if (flag){
    		assert(total == ((N * (N + 1)) / 2));
  	}
  }
  
  release_lock();
  
}



void thread2() {
  
  int j;
  j = 0;
  
  while (j < N) {
  
    //pthread_mutex_lock(&m);
    acquire_lock();
   
    while (num == 0){
      //pthread_cond_wait(&empty, &m);
      
      acquire_lock2();
      
      release_lock();
      
      while(cond2!=1)
      {}
      
      release_lock2();
      
    }
      
    total = total + j;
    num--;
    
    release_lock();
    
    //pthread_cond_signal(&empty);
    cond1 = 1;
    
    j++;
    
  }
  total = total + j;
  flag = 1;
  
  int l2 = checking;
  checking = 2;
  
  acquire_lock();
  
  if(l2 == 1){
  
  	if (flag){
    		assert(total == ((N * (N + 1)) / 2));
  	}
  }
  
  release_lock();
}
