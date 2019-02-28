#pragma once

#include <dlib/dnn.h>
#include <memory>

#include "library.h"

using OpeyeNet = dlib::loss_multiclass_log<dlib::fc<4, quarterLocator<dlib::input_rgb_image>>>;
typedef std::shared_ptr<OpeyeNet> OpeyeNetPtr;

typedef std::shared_ptr<struct TrainingSet> TrainingSetPtr;
struct TrainingSet
{
    std::vector<dlib::matrix<dlib::rgb_pixel>> samples;
    std::vector<unsigned long> labels;
};

typedef std::shared_ptr<struct NetNode> NetNodePtr;
struct NetNode
{
    RegionNodePtr region;
    NetNodePtr children[4];
    TrainingSetPtr samples;
    OpeyeNetPtr net;
    Point min;
    std::string name;
    unsigned int depth;
};

/**
 * The core network library.
 * 
 * It holds the differen layers of neural networks.
 */
class NetworkLibrary
{
  public:
    NetworkLibrary(LibraryPtr library, const std::string &directory);

    std::vector<NetNodePtr> getAllNodes();

    void save();

    const std::string &getDirectory() { return directory; }

    unsigned int getTotalSamples() const;

    void flushNode(NetNodePtr node);

  private:
    void save(NetNodePtr node, json &config);
    void buildAllNodes(std::vector<NetNodePtr> &nodes, NetNodePtr current);

    void buildTreeFromLibrary(LibraryPtr library);
    void developRegion(RegionNodePtr region, NetNodePtr node);
    void accumulateChildren(RegionNodePtr region, TrainingSetPtr node, unsigned int label);

    unsigned int maxDepth;
    NetNodePtr root;
    std::string directory;
    unsigned int totalSamples;
};
