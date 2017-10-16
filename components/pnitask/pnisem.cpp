
#include "pnisem.h"

using namespace pni;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

static void callWithMutex(Mutex& mutex) {
    auto guard = doGuard(mutex);
}

static void test() {
    Mutex mutex;
    callWithMutex(mutex);
}

#pragma GCC diagnostic pop
