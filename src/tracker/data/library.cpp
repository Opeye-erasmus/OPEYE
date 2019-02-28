#include "library.h"
#include <algorithm>
#include <sstream>
#include <thread>
#include <fstream>

namespace
{
static std::mt19937 generator = std::mt19937();
static unsigned int idGenerator = 0;
} // namespace

Library::Library(unsigned int levels_, bool loadImages, const std::string &directory_)
    : tree(new RegionNode()), levels(levels_), directory(directory_), process(PROCESSOR_SIZE, PROCESSOR_SIZE)
{
    tree->depth = 0;
    //tree->min = {.x=0.0f, .y=0.0f};
    tree->min.x = 0.0f;
    tree->min.y = 0.0f;
    developTree(tree, levels);
    load(loadImages);
}

Library::~Library()
{
    save();
}

unsigned int Library::getLevels() const
{
    return levels;
}

std::pair<Point, Size> Library::getSparseRegionRandomPoint()
{
    std::sort(leafs.begin(), leafs.end(), [](const RegionNodePtr &leafA, const RegionNodePtr &leafB) {
        return leafA->samples.size() < leafB->samples.size();
    });

    const RegionNodePtr &candidate = leafs.front();
    unsigned int max = 0;
    for (max = 0; max < leafs.size() - 1; ++max)
    {
        if (leafs[max + 1]->samples.size() > candidate->samples.size())
        {
            break;
        }
    }

    std::uniform_int_distribution<int> distribution(0, max);
    unsigned int pick = distribution(::generator);

    const RegionNodePtr &region = leafs[pick];
    float scale = 1.0f / (1 << region->depth);
    float x = region->min.x /*+ ::distribution(::generator) * scale*/;
    float y = region->min.y /*+ ::distribution(::generator) * scale*/;

    Point p;
    p.x = x;
    p.y = y;
    Size s;
    s.w = scale;
    s.h = scale;
    std::pair<Point, Size> result = std::pair<Point, Size>(p, s);
    return result;
}

void Library::developTree(RegionNodePtr current, unsigned int levelsLeft)
{
    if (levelsLeft != 0)
    {
        for (unsigned int child = 0; child < 4; ++child)
        {
            RegionNodePtr node = RegionNodePtr(new RegionNode());
            node->depth = current->depth + 1;
            float scale = 1.0f / (1 << node->depth);
            node->min = current->min;
            switch (child)
            {
            case 0:
                break;
            case 1:
                node->min.y += scale;
                break;
            case 2:
                node->min.x += scale;
                break;
            case 3:
                node->min.x += scale;
                node->min.y += scale;
                break;
            }

            developTree(node, levelsLeft - 1);
            current->children[child] = node;
        }
    }
    else
    {
        leafs.push_back(current);
    }
}

unsigned int Library::chooseIndexAndCorpPosition(Point &position) const
{
    if (position.x < 0.5f)
    {
        position.x *= 2.0f;
        if (position.y < 0.5f)
        {
            position.y *= 2.0f;
            return 0;
        }
        else
        {
            position.y -= 0.5f;
            position.y *= 2.0f;
            return 1;
        }
    }
    else
    {
        position.x -= 0.5f;
        position.x *= 2.0f;
        if (position.y < 0.5f)
        {
            position.y *= 2.0f;
            return 2;
        }
        else
        {
            position.y -= 0.5f;
            position.y *= 2.0f;
            return 3;
        }
    }
}

RegionNodePtr Library::traveseToLeaf(RegionNodePtr node, Point relativePosition)
{
    if (node->depth == levels)
    {
        return node;
    }
    else
    {
        unsigned int child = chooseIndexAndCorpPosition(relativePosition);
        return traveseToLeaf(node->children[child], relativePosition);
    }
}

void Library::addSample(const Point &position, FramePtr frame)
{
    SamplePtr sample = SamplePtr(new Sample());
    sample->point = position;
    sample->id = ++idGenerator;
    sample->inMemory = false;
    sample->onDisk = false;
    traveseToLeaf(tree, position)->samples.push_back(sample);

    ThreadPtr t = ThreadPtr(new std::thread([&, sample, frame]() {
        std::ostringstream str;
        FramePtr tmpFrame = process(frame);
        if (tmpFrame)
        {
            sample->frame = dlib::matrix<dlib::rgb_pixel>(dlib::mat(dlib::cv_image<dlib::rgb_pixel>(tmpFrame->get())));
            // do no save if not face is detectable
            sample->inMemory = true;
            str << directory << "/img_" << sample->id << ".jpg";
            cv::imwrite(str.str(), frame->get());
            sample->onDisk = true;
        }
        else
        {
            std::cout << "NOT loaded..." << std::endl;
        }
    }));
    threads.push_back(t);
}

void Library::loadSample(const Point &position, unsigned int id, bool loadImage)
{
    SamplePtr sample = SamplePtr(new Sample());
    sample->point = position;
    sample->id = id;
    sample->inMemory = false;
    sample->onDisk = true;
    traveseToLeaf(tree, position)->samples.push_back(sample);

    if (loadImage)
    {

        //ThreadPtr t = ThreadPtr(new std::thread([&, sample]() {
        //std::cout << "Reading " << sample->id << std::endl;
        std::ostringstream str;
        str << directory << "/img_" << sample->id << ".jpg";
        FramePtr frame;
        std::ifstream exists(str.str() + ".processed.jpg");
        if (exists)
        {
            exists.close();
            cv::Mat image = cv::imread(str.str() + ".processed.jpg");
            frame = FramePtr(new Frame(image));
        }
        else
        {
            cv::Mat image = cv::imread(str.str());
            if (image.cols > 0)
            {
                frame = FramePtr(new Frame(image));
                frame = process(frame);
                if (frame)
                {
                    cv::imwrite(str.str() + ".processed.jpg", frame->get());
                }
            }
            else
            {
                std::cout << "No valid file: " << str.str() << std::endl;
            }
        }
        if (frame)
        {
            sample->frame = dlib::matrix<dlib::rgb_pixel>(dlib::mat(dlib::cv_image<dlib::rgb_pixel>(frame->get())));
            sample->inMemory = true;
            //std::cout << "Loaded processed face: "  << str.str() << std::endl;
        }
        else
        {
            //std::cout << "No face: " << str.str() << std::endl;
        }
        //std::cout << "Done with " << sample->id << std::endl;

        //}));
        //threads.push_back(t);
    }
}

const std::vector<SamplePtr> &Library::getSamples(const Point &position)
{
    return traveseToLeaf(tree, position)->samples;
}

void Library::load(bool loadImages)
{
    std::cout << "Loading..." << std::endl;
    std::ifstream input(directory + "/config.json");
    if (input.is_open())
    {
        json j;
        input >> j;
        idGenerator = j["idGenerator"];
        int i = 0;
        for (auto leaf : j["leafs"])
        {
            Point p;
            p.x = leaf["point"]["x"];
            p.y = leaf["point"]["y"];
            loadSample(p, leaf["id"], loadImages);
            ++i;
        }
        std::cout << "Loading " << i << " samples." << std::endl;
        synchronize();
        std::cout << "Loaded " << leafs.size() << " leaf nodes (images: " << getTotalSamples() << ")." << std::endl;
    }
}

void Library::save()
{
    synchronize();
    json j;
    j["idGenerator"] = idGenerator;
    cv::FileStorage processed("processed.yml", cv::FileStorage::WRITE);
    for (const RegionNodePtr leaf : leafs)
    {
        for (const SamplePtr &sample : leaf->samples)
        {
            json leafDescriptor = {
                {"id", sample->id},
                {"point", {
                              {"x", sample->point.x},
                              {"y", sample->point.y},
                          }}};
            j["leafs"].push_back(leafDescriptor);
        }
    }

    std::ofstream output(directory + "/config.json");
    output << j;
    output.close();
}

void Library::synchronize()
{
    for (auto thread : threads)
        thread->join();
    threads.clear();
}

RegionNodePtr Library::getTree()
{
    synchronize();
    return tree;
}

const std::string &Library::getDirectory() const
{
    return directory;
}

unsigned int Library::getTotalSamples() const
{
    unsigned int result = 0;
    for (const RegionNodePtr leaf : leafs)
    {
        result += leaf->samples.size();
    }
    return result;
}
