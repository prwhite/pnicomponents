
#ifndef task_h
#define task_h

#include <string>

namespace pni {

class Task {

    public:
        Task(std::string const& name);
        virtual ~Task();

        bool start();
        virtual void cancel();  // Not called by destructor, should be called at end of `taskMethod`.

        void lock();
        void unlock();

    protected:
        static void taskFunc( void* param );
        virtual void taskMethod();

    private:
        std::string mName = "Unnamed task";
        void* mFuncPtr = 0;
        TaskHandle_t mTask = 0;

};

} // end namespace pni

#endif // task_h
