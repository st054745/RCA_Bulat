#include <QApplication>

#include "RCA/robotcontroladapter.h"
#include "Test/ControlUnit/controlunit.h"
#include "Test/Planner/planner.h"
#include "Test/Scene/scene.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Scene* scene = new Scene;
    Q_UNUSED(scene);

    RobotControlAdapter* RCA = new RobotControlAdapter;
    Q_UNUSED(RCA);

    Planner* planner = new Planner("p");
    planner->show();

    ControlUnit* unit1 = new ControlUnit("t");
    unit1->show();

    ControlUnit* unit2 = new ControlUnit("f");
    unit2->show();

    return a.exec();
}
