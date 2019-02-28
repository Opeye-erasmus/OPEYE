#include "tracker.h"


Tracker::Tracker(unsigned int depth_, const std::string& directory, const std::string& rootName)
    : processor(PROCESSOR_SIZE, PROCESSOR_SIZE)
    , depth(depth_){

    Rect baseRect;
    baseRect.min.x = 0.0f;
    baseRect.min.y = 0.0f;
    baseRect.dimension.w = 1.0f;
    baseRect.dimension.h = 1.0f;

    loadNetworks(depth, directory + "/" + rootName, baseRect);
}


NetworkInfoPtr Tracker::loadNetworks(int depth, const std::string& basePath, const Rect& rectangle) {


    NetworkInfoPtr networkInfo = NetworkInfoPtr(new NetworkInfo());
    if (depth>0) {
        OpeyeNet net;
        dlib::deserialize(basePath) >> net;
        OpeyeNetPtr network = OpeyeNetPtr(new OpeyeNet(net));
        networkInfo->net = network;
    } else {
        networkInfo->net = nullptr;
    }

    networkInfo->rectangle = rectangle;
    networkInfo->children[0] = nullptr;
    networkInfo->children[1] = nullptr;
    networkInfo->children[2] = nullptr;
    networkInfo->children[3] = nullptr;
    networks.push_back(networkInfo);

    if (depth>0) {
        for (unsigned int i = 0 ;i < 4; ++i) {
            std::stringstream str;
            str << basePath << "_" << i;

            Rect childRectangle = rectangle;

            childRectangle.dimension.w/=2.0f;
            childRectangle.dimension.h/=2.0f;

            switch (i) {
                case 0:
                    break;
                case 1:
                    childRectangle.min.y += childRectangle.dimension.h;
                    break;
                case 2:
                    childRectangle.min.x += childRectangle.dimension.w;
                    break;
                case 3:
                    childRectangle.min.x += childRectangle.dimension.w;
                    childRectangle.min.y += childRectangle.dimension.h;
                    break;
            }

            networkInfo->children[i] = loadNetworks(depth-1, str.str(), childRectangle);
        }
    }
    return networkInfo;
}



Rect Tracker::track(FramePtr frame) {
    auto processedFrame = processor(frame);
    if (processedFrame) {
        return processLevel(networks[0], processedFrame);
    } else {
        Rect rect;
        rect.min.x = 0.0f;
        rect.min.y = 0.0f;
        rect.dimension.w = .01f;
        rect.dimension.h = .01f;
        return rect;
    }
}

Rect Tracker::processLevel(NetworkInfoPtr network, FramePtr frame){
    const dlib::matrix<dlib::rgb_pixel> input =dlib::matrix<dlib::rgb_pixel>(
                dlib::mat(dlib::cv_image<dlib::rgb_pixel>(frame->get()))
    );

    OpeyeNet& net = (*(network->net));
    auto output = net(input);

    NetworkInfoPtr child = network->children[output];
    if (child) {
        if (child->net) {
            return processLevel(child, frame);
        } else {
            return child->rectangle;
        }
    } else {
        return network->rectangle;
    }
}

