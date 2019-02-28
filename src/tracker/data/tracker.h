#pragma once

#include "types.h"
#include "video/frameprocess.h"
#include "network.h"

typedef std::shared_ptr<struct NetworkInfo> NetworkInfoPtr;

struct NetworkInfo
{
    OpeyeNetPtr net;
    Rect rectangle;
    NetworkInfoPtr children[4];
};

/**
 * The tracker computes gazed at position based on a given frame.
 * 
 * It automatically loads the whole network tree and knows how to navigate through the different layers.
 */
class Tracker
{
  public:
    Tracker(unsigned int depth = 4, const std::string &directory = "data", const std::string &rootName = "net_0");

    Rect track(FramePtr frame);

    unsigned int getDepth() { return depth; }

  private:
    unsigned int depth;
    Rect processLevel(NetworkInfoPtr network, FramePtr frame);
    NetworkInfoPtr loadNetworks(int depth, const std::string &basePath, const Rect &rectangle);

    FrameProcessor processor;

    std::vector<NetworkInfoPtr> networks;
};

SHARED_PTR(Tracker)
