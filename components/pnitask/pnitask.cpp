////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "pnitask.h"

////////////////////////////////////////////////////////////////////

static const char* TAG = "task";

////////////////////////////////////////////////////////////////////

namespace pni {

////////////////////////////////////////////////////////////////////

Task::Task(std::string const& name) :
        mName(name),
        mTask(0) {

}

Task::~Task() {
    // Does not cancel automatically.
}

bool Task::start() {
    ESP_LOGV(TAG, "Task::start beg");
    BaseType_t ret = xTaskCreate(taskFunc, mName.c_str(), mStackSize, this, 1, &mTask); // TODO: Fix magic numbers

    if( ret != pdPASS) {
        return false;
    }

    ESP_LOGV(TAG, "Task::start end");
    return true;
}

void Task::cancel() {
    if (mTask != 0) {
        vTaskDelete(mTask);
        mTask = 0;
    }
}

void Task::taskFunc( void* param ) {
    ESP_LOGV(TAG, "Task::taskFunc beg");
    Task* self = reinterpret_cast< Task* > ( param );
    ESP_LOGV(TAG, "Task::start before taskMethod with self = %p", self);
    self->taskMethod();
}

void Task::taskMethod() {
    ESP_LOGV(TAG, "Task::taskMethod default implementation called");
    cancel();
}

void Task::delay(size_t msec) {
    // vTaskDelay(configTICK_RATE_HZ * usec);
    vTaskDelay(msec / portTICK_PERIOD_MS);
}

void Task::delayTicks(TickType_t ticks) {
    vTaskDelay(ticks);
}

void Task::lock() {
    // TODO
    ESP_LOGE(TAG, "lock not implemented yet");
}

void Task::unlock() {
    // TODO
    ESP_LOGE(TAG, "unlock not implemented yet");
}

Task* Task::createAndStart(std::string const& name) {
    auto task = new Task(name);
    task->start();
    return task;
}

////////////////////////////////////////////////////////////////////

TaskLambda::TaskLambda(std::string const& name, Lambda lambda) :
    Task(name),
    mLambda(lambda)
{

}

void TaskLambda::taskMethod() {
    mLambda();
}

Task* TaskLambda::createAndStart(std::string const& name, Lambda lambda) {
    auto task = new TaskLambda(name, lambda);
    task->start();
    return task;
}

////////////////////////////////////////////////////////////////////

} // end namespace pni
