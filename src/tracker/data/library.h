#pragma once
#include <random>
#include <thread>

#include "defines.h"
#include "types.h"
#include "video/frameprocess.h"

/**
 * The library managing various samples with labels. The library also assembles the tree to teach the neural networks.
 */
class Library
{
  public:
    Library(unsigned int levels, bool loadImages = true, const std::string &directory = "data");

    ~Library();

    std::pair<Point, Size> getSparseRegionRandomPoint();

    void addSample(const Point &position, FramePtr frame);

    const std::vector<SamplePtr> &getSamples(const Point &position);

    unsigned int getLevels() const;

    unsigned int getTotalSamples() const;

    const std::string &getDirectory() const;

    void synchronize();

  private:
    friend class NetworkLibrary;
    RegionNodePtr getTree();

    void loadSample(const Point &position, unsigned int id, bool loadImage);

    void load(bool loadImages);

    void save();
    RegionNodePtr traveseToLeaf(RegionNodePtr node, Point relativePosition);
    unsigned int chooseIndexAndCorpPosition(Point &position) const;

    void developTree(RegionNodePtr current, unsigned int levelsLeft);

    unsigned int levels;
    RegionNodePtr tree;

    std::vector<RegionNodePtr> leafs;
    std::vector<ThreadPtr> threads;

    const std::string directory;
    FrameProcessor process;
    std::mutex mutex;
};

SHARED_PTR(Library)
