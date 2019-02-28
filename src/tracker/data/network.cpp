#include "network.h"
#include <algorithm>
#include <iostream>

NetworkLibrary::NetworkLibrary(LibraryPtr library, const std::string &directory_)
    : root(nullptr), directory(directory_), totalSamples(library->getTotalSamples())
{
    library->synchronize();
    buildTreeFromLibrary(library);
}

void NetworkLibrary::buildTreeFromLibrary(LibraryPtr library)
{
    RegionNodePtr tree = library->getTree();
    root = NetNodePtr(new NetNode());
    //root->min = {.x=0.0f, .y=0.0f};
    root->min.x = 0.0f;
    root->min.y = 0.0f;
    root->depth = 0;
    root->net = nullptr;
    root->name = "net_0";
    maxDepth = library->getLevels();
    developRegion(tree, root);
}

void NetworkLibrary::developRegion(RegionNodePtr region, NetNodePtr node)
{
    node->samples = TrainingSetPtr(new TrainingSet());
    for (unsigned int i = 0; i < 4; ++i)
    {
        accumulateChildren(region->children[i], node->samples, i);
        if (region->depth < maxDepth - 1)
        {
            NetNodePtr newNode = NetNodePtr(new NetNode());
            newNode->net = nullptr;
            std::stringstream str;
            str << node->name << "_" << i;
            newNode->name = str.str();
            newNode->depth = region->children[i]->depth;
            newNode->min = region->children[i]->min;
            node->children[i] = newNode;
            developRegion(region->children[i], newNode);
        }
    }
}

void NetworkLibrary::accumulateChildren(RegionNodePtr region, TrainingSetPtr samples, unsigned int label)
{
    //All children are samples for this region.
    if (region->depth == maxDepth)
    {
        for (auto sample : region->samples)
        {
            if (sample->inMemory)
            {
                samples->samples.push_back(sample->frame);
                samples->labels.push_back(label);
            }
            else
            {
                //std::cout << "WOW not in mem..." << std::endl;
            }
        }
        std::cout << "Leaf: " << samples->samples.size() << std::endl;
    }
    else
    {
        for (unsigned int i = 0; i < 4; ++i)
            accumulateChildren(region->children[i], samples, label);
    }
}

std::vector<NetNodePtr> NetworkLibrary::getAllNodes()
{
    std::vector<NetNodePtr> result;
    buildAllNodes(result, root);
    return result;
}

void NetworkLibrary::buildAllNodes(std::vector<NetNodePtr> &nodes, NetNodePtr current)
{
    nodes.push_back(current);
    if (current->depth < maxDepth - 1)
    {
        for (unsigned int i = 0; i < 4; ++i)
        {
            buildAllNodes(nodes, current->children[i]);
        }
    }
}

void NetworkLibrary::save()
{
    json j;
    save(root, j["tree"]);
    j["maxDepth"] = maxDepth;

    std::ofstream output(directory + "/net_config.json");
    output << j;
    output.close();
}

void NetworkLibrary::flushNode(NetNodePtr node)
{
    if (node->net)
    {
        dlib::serialize(directory + "/" + node->name) << *(node->net.get());
        node->net = nullptr;
    }
}

void NetworkLibrary::save(NetNodePtr node, json &config)
{
    config["file"] = node->name;
    if (node->net)
    {
        dlib::serialize(directory + "/" + node->name) << *(node->net.get());
    }

    if (node->depth < maxDepth - 1)
    {
        for (int i = 0; i < 4; ++i)
        {
            json jc;
            save(node->children[i], jc);
            config["children"].push_back(jc);
        }
    }
}

unsigned int NetworkLibrary::getTotalSamples() const
{
    return totalSamples;
}
