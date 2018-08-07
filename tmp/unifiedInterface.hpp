#include <atomic>
#include <functional>
#include <iostream>
#include <mutex>

using namespace std;

//#define  USE_SPIN_LOCK
#define USE_MUTEX_LOCK 
//Implements a unified interface for
//the locking mechanism
//locking api are passed with lamdas which 
//contain the api's for modifying the object

//A linked list which can be used to queue the locks
//Not used as of now

template <class T>
  struct Node {
    bool flag; // a local variable which can be used in future for storing state
    T& functor; //an function object for reading/modifying the  data
    Node *next; //a pointer to the next node in the queue
};

//wrapper for spin lock
template <class T>
class simple_spin_lock {
  std::atomic_flag flag;

 public:
  simple_spin_lock(): flag(ATOMIC_FLAG_INIT) {}

  void performSyncRead(std::function<void(const T&)>& reader, auto& data) {
      lock();
      (reader)(data);
      unlock();
  }

  void performSyncWrite(std::function<void(T&)>& writer, auto& data) {
    lock();
    (writer)(data);
    unlock();
  }
  
  void lock() {
    while(flag.test_and_set());
  }

  void unlock() {
    flag.clear();
  }
};

//wrapper for mutex lock
template <class T>
class simple_mutex_lock {
    
   std::mutex mutex_lock;

public:
  
  void lock() { mutex_lock.lock(); }

  void unlock() { mutex_lock.unlock(); }
  
  void performSyncRead(std::function<void(const T&)>& reader, auto& data) {
      lock();
      reader(data);
      unlock();
  }

  void performSyncWrite(std::function<void(T&)>& writer, auto& data) {
      lock();
      writer(data);
      unlock();
  }
};

template <class T>
class generic_lock {

    public:

    //should initialise Impl_ which contains the atomic object
    generic_lock() {};

    /// update interface for synchronous operations
    //  on datas of T;
    //@param:  reader function object performing read operation;
    //@param:  <T> object  
    void update(std::function<void(const T&)>& reader, const T& a) { 
         impl_.performSyncRead(reader, a); 
    }

    //perform write operation
    //@param: writer function object performing write operation;
    void update(std::function<void(T&)>& writer, T&a) { 
        impl_.performSyncWrite(writer, a); 
    }

    private:
#ifdef USE_SPIN_LOCK
    simple_spin_lock<T> impl_;
#elif defined USE_MUTEX_LOCK 
    simple_mutex_lock<T> impl_;
#endif
};

//todo:
//write impl_ for MCS, Phase fair RW locks.

