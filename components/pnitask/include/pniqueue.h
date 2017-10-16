////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////

#ifndef pniqueue_h
#define pniqueue_h

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"

#include <string>

////////////////////////////////////////////////////////////////////

namespace pni {

////////////////////////////////////////////////////////////////////
    // Wrapper around FreeRTOS xQueue select functions.
    // See http://www.freertos.org/a00018.html
template< class Message >
class Queue {
    public:
        Queue(size_t len) {
            mQueue = xQueueCreate(len, sizeof(Message));
        }

        ~Queue() {
            vQueueDelete(mQueue);
        }

        bool send(Message const& msg, TickType_t wait) {
            auto ret = xQueueSend(mQueue, &msg, wait);
            return ret == pdTRUE ? true : false;
        }

        bool overwrite(Message const& msg) {
            auto ret = xQueueOverwrite(mQueue, &msg);
            return ret == pdTRUE ? true : false;
        }

        bool receive(Message& msg, TickType_t wait) {
            auto ret = xQueueReceive(mQueue, &msg, wait);
            return ret == pdTRUE ? true : false;
        }

        void reg(std::string const& str) {
            vQueueAddToRegistry(mQueue, str.c_str());
        }

        void unreg() {
            vQueueUnregisterQueue(mQueue);
        }

    private:
        QueueHandle_t mQueue;
};

////////////////////////////////////////////////////////////////////

} // end namespace pni

#endif // pniqueue_h
