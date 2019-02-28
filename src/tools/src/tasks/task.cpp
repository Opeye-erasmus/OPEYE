#include "task.h"

Task::Task()
    : doContinue(true) {
}


bool Task::isDone() {
    return !doContinue.load();
}


void Task::keyPress(Key key) {

}

void Task::mouseClick(Button button, float x, float y) {

}
