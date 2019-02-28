#pragma once

#include "task.h"
#include "data/network.h"
#include <atomic>

class PupilCenterTask : public Task{
public:
    PupilCenterTask();
    ~PupilCenterTask();

    virtual void keyPress(Key key);

    virtual void update(float delta);

    virtual void render(Screen& screen);


private:
};

SHARED_PTR(PupilCenterTask);
