////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////

#ifndef task_h
#define task_h

#include <string>
#include <functional>

namespace pni {

////////////////////////////////////////////////////////////////////

class Task {

    public:
        Task(std::string const& name);
        virtual ~Task();

            // Can only change stack size before calling `start`.
        void setStackSize(size_t val) { mStackSize = val; }
        size_t getStackSize() const { return mStackSize; }

        bool start();
        virtual void cancel();  // Not called by destructor, should be called at end of `taskMethod`.

        static void delay(size_t msec);             // 10 ms resolution
        static void delayTicks(TickType_t ticks);   // 10 ms resolution

        void lock();
        void unlock();

        class guard {
            public:
                guard(Task* task) : mTask(task) { mTask->lock();}
                ~guard() { mTask->unlock(); }
            private:
                Task* mTask;
        };

        guard makeGuard() { return guard(this); }

        static Task* createAndStart(std::string const& name);

    protected:
        static void taskFunc( void* param );
        virtual void taskMethod();

    private:
        std::string mName = "Unnamed task";
        TaskHandle_t mTask = 0;
        size_t mStackSize = 2000;

};

////////////////////////////////////////////////////////////////////

class TaskLambda : public Task {

    public:
        using Lambda = std::function< void() >;

        TaskLambda(std::string const& name, Lambda lambda);

        static Task* createAndStart(std::string const& name, Lambda lambda);

    protected:
        virtual void taskMethod();

    private:
        
        Lambda mLambda;
        
};

////////////////////////////////////////////////////////////////////

} // end namespace pni

#endif // task_h
