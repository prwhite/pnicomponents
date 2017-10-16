////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////

#ifndef pnisem_h
#define pnisem_h

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_log.h"

////////////////////////////////////////////////////////////////////

namespace pni {

////////////////////////////////////////////////////////////////////

    // Binary semaphore
class Semaphore {
    public:
        Semaphore() {
            mSema = xSemaphoreCreateBinary();
        }

        ~Semaphore() {
            vSemaphoreDelete(mSema);
            mSema = 0;
        }

            // The macro portTICK_PERIOD_MS can be used to convert this to a real time.
            // If INCLUDE_vTaskSuspend is set to '1' then specifying the block time as 
            //  portMAX_DELAY will cause the task to block indefinitely (without a timeout).
        void take(TickType_t wait = 0) {
            xSemaphoreTake(mSema, wait);
        }

        void give() {
            xSemaphoreGive(mSema);
        }

    private:
        SemaphoreHandle_t mSema;
};

class Mutex {

    public:
        Mutex() {
            mSema = xSemaphoreCreateMutex();
        }

        ~Mutex () {
            vSemaphoreDelete(mSema);
            mSema = 0;
        }

            // The macro portTICK_PERIOD_MS can be used to convert this to a real time.
            // If INCLUDE_vTaskSuspend is set to '1' then specifying the block time as 
            //  portMAX_DELAY will cause the task to block indefinitely (without a timeout).
        void lock(TickType_t wait = portMAX_DELAY) {
            xSemaphoreTake(mSema, wait);
        }

        void unlock() {
            xSemaphoreGive(mSema);
        }
    

    private:
        SemaphoreHandle_t mSema;
};

class MutexRecursive {

    public:
        MutexRecursive() {
            mSema = xSemaphoreCreateRecursiveMutex();
        }

        ~MutexRecursive () {
            vSemaphoreDelete(mSema);
            mSema = 0;
        }

            // The macro portTICK_PERIOD_MS can be used to convert this to a real time.
            // If INCLUDE_vTaskSuspend is set to '1' then specifying the block time as 
            //  portMAX_DELAY will cause the task to block indefinitely (without a timeout).
        void lock(TickType_t wait = portMAX_DELAY) {
            xSemaphoreTakeRecursive(mSema, wait);
        }

        void unlock() {
            xSemaphoreGiveRecursive(mSema);
        }
            
    private:
        SemaphoreHandle_t mSema;
};

template< class MutexType >
class Guard {
    public:
        Guard(MutexType& mtx) :
                mMutex(mtx) {
            mMutex.lock();
        }

        ~Guard() {
            mMutex.unlock();
        }

    private:
        MutexType& mMutex;
};

    // Convenience func, works with any Mutex class, without requiring
    // specification of template param needed in Guard.
    // Usage:
    //   auto guard = doGuard(mutex);
    // Note: Naively, there should be n extra copy created when calling
    //  Guard, but due to copy elision, that won't happen.
    // Note: Copy elision for this case is probably only guaranteed
    //  on C++17, or maybe C++14... but not C++11.
    // See: http://en.cppreference.com/w/cpp/language/copy_elision
template< class MutexType >
Guard< MutexType > doGuard(MutexType& mutex) {
    return Guard< MutexType >(mutex);
}

////////////////////////////////////////////////////////////////////

} // end namespace pni

#endif // pnisem_h

