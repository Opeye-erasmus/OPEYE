#include "traintask.h"
#include <sstream>

TrainTask::TrainTask(unsigned int epochs_, LibraryPtr library_, const std::string &networkPath)
    : epochs(epochs_), networkLibrary(library_, networkPath)
{
    nodes = networkLibrary.getAllNodes();
    intialNumberOfNodes = nodes.size();
    killed.store(false);

    thread = std::shared_ptr<std::thread>(new std::thread([this]() {
        bool empty = false;
        do
        {
            {
                this->startNext();
            }
            {
                LOCK(mutex)
                empty = nodes.empty();
            }
        } while (!empty && !killed.load());
        std::cout << "All done." << std::endl;
        networkLibrary.save();
        done();
    }));
}

TrainTask::~TrainTask()
{
    killed.store(true);
    thread->join();
}

void TrainTask::keyPress(Key key)
{
}

void TrainTask::startNext()
{
    NetNodePtr node = nullptr;
    {
        LOCK(mutex)
        node = nodes.back();
        nodes.pop_back();
    }
    currentNodeName = node->name;
    currentNodeSamples = node->samples->samples.size();

    node->net = OpeyeNetPtr(new OpeyeNet());
    dlib::layer<1>(*(node->net.get())).layer_details().set_bias_learning_rate_multiplier(990);
    dlib::dnn_trainer<OpeyeNet> trainer(*(node->net.get()), dlib::sgd(0.0, 0.99));
    trainer.be_verbose();
    trainer.set_learning_rate(7.5e-5);
    trainer.set_min_learning_rate(1e-8);
    trainer.set_mini_batch_size(250);
    trainer.set_synchronization_file(networkLibrary.getDirectory() + "/" + node->name + ".tmpsync");
    trainer.set_max_num_epochs(epochs);
    trainer.train(node->samples->samples, node->samples->labels);
    networkLibrary.flushNode(node);
}

void TrainTask::update(float delta)
{
}

void TrainTask::render(Screen &screen)
{
    std::stringstream stream1, stream2, stream3;
    stream1 << "Processing network: " << (intialNumberOfNodes - nodes.size()) << " of " << intialNumberOfNodes;
    stream2 << "with " << currentNodeSamples << " samples for " << epochs << " epochs. Name: " << currentNodeName;
    stream3 << "loaded " << networkLibrary.getTotalSamples() << " samples";
    screen.fill({0.0f, 0.0f}, {1.0f, 1.0f}, 0, 0, 0);
    Point p1, p2, p3;
    p1.x = 0.1;
    p1.y = 0.1;
    p2.x = 0.1;
    p2.y = 0.2;
    p3.x = 0.1;
    p3.y = 0.3;
    screen.putText(stream1.str(), p1);
    screen.putText(stream2.str(), p2);
    screen.putText(stream3.str(), p3);
}

void TrainTask::trainNets()
{
}
