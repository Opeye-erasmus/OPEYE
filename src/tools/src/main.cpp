#include <iostream>
#include "tasks/recordertask.h"
#include "tasks/traintask.h"
#include "tasks/trackertask.h"
#include "tasks/magnifytask.h"
#include "tasks/pupilcentertask.h"
#include "tasks/evaluationtask.h"
#include "utils/keyboard.h"
#include "utils/mouse.h"
#include <args.hpp>

int main(int argc, char *argv[])
{

    args::ArgumentParser parser("Opeye Gaze estimation.", "");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::Flag recorder(parser, "record", "Record samples", {'r'});
    args::Flag trainer(parser, "train", "Traing the network", {'t'});
    args::Flag tester(parser, "test", "Test the network", {'v'});
    args::Flag magnify(parser, "magnify", "Magnify with the network", {'m'});
    args::Flag pupil(parser, "pupil", "Pupil center", {'p'});
    args::Flag simplify(parser, "simplify", "Simplify", {'s'});
    args::Flag evaluator(parser, "evaluator", "Evaluate a network", {'b'});
    args::ValueFlag<std::string> imagePathOption(parser, "image path", "Path to the training images", {'i'});
    args::ValueFlag<std::string> networkPathOption(parser, "network path", "Path to the networks", {'n'});
    args::ValueFlag<unsigned int> levelsOption(parser, "levels", "Depth of the neuronal network to work with", {'l'});
    args::ValueFlag<unsigned int> epochsOption(parser, "epochs", "Number of epochs to train the neuronal network", {'e'});
    args::ValueFlag<unsigned int> resolutionOption(parser, "resolution", "Resolution of the window(i.e. 1080 or 2160)", {'x'});

    try
    {
        parser.ParseCLI(argc, argv);
    }
    catch (args::Help)
    {
        std::cout << parser;
        return 0;
    }
    catch (args::ParseError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }
    catch (args::ValidationError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }

    std::string networkPath = "network";
    if (networkPathOption.Get() != "")
    {
        networkPath = networkPathOption.Get();
    }

    std::string imagePath = "samples";
    if (imagePathOption.Get() != "")
    {
        imagePath = imagePathOption.Get();
    }

    unsigned int levels = 4;
    if (levelsOption.Get() > 0)
    {
        levels = levelsOption.Get();
    }

    unsigned int epochs = 500;
    if (epochsOption.Get() > 0)
    {
        epochs = epochsOption.Get();
    }

    unsigned int resolution = 1080;
    if (resolutionOption.Get() > 0)
    {
        resolution = resolutionOption.Get();
    }

    TaskPtr task = nullptr;
    if (recorder)
    {
        levels = 6;
        LibraryPtr library(new Library(levels, false, imagePath));
        task = RecorderTaskPtr(new RecorderTask(library, simplify));
    }
    else if (trainer)
    {
        LibraryPtr library(new Library(levels, true, imagePath));
        library->synchronize();
        task = TrainTaskPtr(new TrainTask(epochs, library, networkPath));
    }
    else if (magnify)
    {
        TrackerPtr tracker(new Tracker(levels, networkPath, "net_0"));
        task = MagnifyTaskPtr(new MagnifyTask(tracker));
    }
    else if (tester)
    {
        TrackerPtr tracker(new Tracker(levels, networkPath, "net_0"));
        task = TrackerTaskPtr(new TrackerTask(tracker));
    }
    else if (evaluator)
    {
        TrackerPtr tracker(new Tracker(levels, networkPath, "net_0"));
        task = EvaluationTaskPtr(new EvaluationTask(tracker));
    }
    else if (pupil)
    {
        task = PupilCenterTaskPtr(new PupilCenterTask());
    }
    else
    {
        std::cout << parser;
        return 0;
    }

    Screen screen("Opeye", resolution);
    Keyboard keyboard;
    Mouse mouse(screen);
    Point mousePosition;
    while (!task->isDone())
    {

        task->update(0.1f);
        task->render(screen);

        screen.display();

        Button button = mouse.update(mousePosition);
        if (button != BUTTON_NONE)
        {
            task->mouseClick(button, mousePosition.x, mousePosition.y);
        }

        Key key = keyboard.update();

        if (key != KEY_NONE)
        {

            if (key == KEY_ESCAPE)
            {
                return 1;
                break;
            }
            else
            {
                task->keyPress(key);
            }
        }
    }
    return 0;
}
