
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "pnitask.h"

static const char* TAG = "task";

namespace pni {

Task::Task(std::string const& name) :
        mName(name) {

}

Task::~Task() {

}

bool Task::start() {
    ESP_LOGV(TAG, "Task::start beg");
    BaseType_t ret = xTaskCreate(taskFunc, mName.c_str(), 1000, this, 1, &mTask); // TODO: Fix magic numbers

    if( ret != pdPASS) {
        return false;
    }

    ESP_LOGV(TAG, "Task::start end");
    return true;
}

void Task::cancel() {
    vTaskDelete(0);
}

void Task::taskFunc( void* param ) {
    Task* self = reinterpret_cast< Task* > ( param );
    self->taskMethod();
}

void Task::taskMethod() {

}

void Task::lock() {
    // TODO
}

void Task::unlock() {
    // TODO
}

} // end namespace pni
