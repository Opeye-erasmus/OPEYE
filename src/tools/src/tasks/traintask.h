#pragma once

#include "task.h"
#include "data/network.h"
#include <atomic>

class TrainTask : public Task{
public:
    TrainTask(unsigned int epochs, LibraryPtr library, const std::string& networkPath);
    ~TrainTask();

    virtual void keyPress(Key key);

    virtual void update(float delta);

    virtual void render(Screen& screen);


private:
    void trainNets();

    void startNext();

    unsigned int epochs;
    NetworkLibrary networkLibrary;

    std::vector<NetNodePtr> nodes;
    unsigned int intialNumberOfNodes;
    unsigned int currentNodeSamples;
    std::string currentNodeName;
    std::mutex mutex;
    std::shared_ptr<std::thread> thread;
    std::atomic<bool> killed;
};

SHARED_PTR(TrainTask);
