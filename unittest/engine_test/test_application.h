#ifndef TEST_APPLICATION_H_
#define TEST_APPLICATION_H_

#include "application.h"

class TestApplication : public application {
public:
    TestApplication() {}

    void initialize() {}
    void finalize() {}

    virtual ~TestApplication() {}
};

#endif  // TEST_APPLICATION_H_
