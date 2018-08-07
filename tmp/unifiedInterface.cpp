#include "unifiedInterface.hpp"
#include <thread>

using namespace std;
class Global {

public:
    Global(int j) {
        p = new int (j);        
    }
    
    int *p;
    
    int dataReader() const{ if (p) return *p; }
    void dataWriter(int a) { delete(p); p = nullptr; p = new int(a);  }
};

Global global(100); 
generic_lock<Global> glock;

void dataReader() {
    int k;
    std::function<void(const Global&)> readRequest = 
                        [&](const Global&)->void{ k = global.dataReader(); };
    
    while (true) {
      glock.update(readRequest, const_cast<const decltype(global)&> (global));
    }
}

void dataWriter() {
    int a;
    
    std::function<void(Global&)> writeRequest =
                 [&](Global&)->void{ global.dataWriter(a);};

    while (true) {
      glock.update(writeRequest, global);
    }
}

int main()
{
    //test to check spin lock generic implementation
    std::thread reader_thread1(&dataReader); 
    std::thread writer_thread1(&dataWriter); 
    
    reader_thread1.join();
    writer_thread1.join();
}
