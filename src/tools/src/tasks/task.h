#pragma once
#include "defines.h"
#include "video/screen.h"
#include <atomic>


class Task {
public:
    Task();

    virtual void keyPress(Key key);

    virtual void mouseClick(Button button, float x, float y);
    
    virtual void update(float delta) = 0;
    
    virtual void render(Screen& screen) = 0;

    bool isDone();
protected:
    void done() {
        doContinue.store(false);
    };

    std::atomic<bool> doContinue;

};


SHARED_PTR(Task)
