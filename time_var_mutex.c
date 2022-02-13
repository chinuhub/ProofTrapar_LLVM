void __ASSERT() {}

/* Testcase from Threader's distribution. For details see:
   http://www.model.in.tum.de/~popeea/research/threader
*/

int m_inode=0; // protects the inode
int m_busy=0; // protects the busy flag

// #include <pthread.h>
#define assert(e) if (!(e)) __ASSERT()
#define acquire_lock_i() while (!__sync_bool_compare_and_swap( &m_inode,0, 1)) {}
#define release_lock_i() m_inode = 0

#define acquire_lock_b() while (!__sync_bool_compare_and_swap( &m_busy,0, 1)) {}
#define release_lock_b() m_busy = 0


int block=0;
int busy=0; // boolean flag indicating whether the block has been allocated to an inode
int inode=0;


void allocator(){
  //pthread_mutex_lock(&m_inode);
  acquire_lock_i();
  
  if(inode == 0){
    //pthread_mutex_lock(&m_busy);
    acquire_lock_b();
    busy = 1;
    //pthread_mutex_unlock(&m_busy);
    release_lock_b();
    inode = 1;
  }
  block = 1;
  assert(block == 1);
  //pthread_mutex_unlock(&m_inode);
  release_lock_i();
}


void deallocator(){
  //pthread_mutex_lock(&m_busy);
  acquire_lock_b();
  if(busy == 0){
    block = 0;
    assert(block == 0);
  }
  //pthread_mutex_unlock(&m_busy);
  release_lock_b();
}




 
