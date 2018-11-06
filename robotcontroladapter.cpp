#include "robotcontroladapter.h"

RobotControlAdapter::RobotControlAdapter()
{
    sceneSocket = new QTcpSocket(this);
    sceneSocket->connectToHost("localhost", 1111);

    if (this->listen(QHostAddress::LocalHost, 5555))
    {
        qDebug() << "Listening RCA";
    }
    else
    {
        qDebug() << "Not listening RCA";
    }
}

void RobotControlAdapter::incomingConnection(int socketDescriptor)
{
    // Создаем новый сокет - канал связи между одним из ControlUnit
    QTcpSocket* socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);

    // Добавляем сокет в список ожидания
    waitSockets.append(socket);

    // Необходимые соединения слотов и сигналов
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(deleteSocket()));
}

void RobotControlAdapter::readyRead()
{
    // find the sender and cast into the socket
    QObject* object = QObject::sender();
    QTcpSocket* socket = static_cast<QTcpSocket*>(object);

    // read data
    QByteArray data = socket->readAll();

    if (data.size() == 1) // Object names, or shutdown command
    {
        if (planner == nullptr && data == "p") // It's a planner and it's not active yet
        {
            planner = socket;
            waitSockets.removeOne(socket);
        }
        else if (planner != nullptr && data == "e") // Shutdown command
        {
            qDebug() << "Planer shutdown";
        }
        else if (clients.contains(data) == false) // client isn't in the list and sends us his name
        {
            clients.insert(data, socket);
            waitSockets.removeOne(socket);
        }
        else // Senseless command
        {
            qDebug() << "Senseless command 3";
        }
    }
    else
    {
        QList<QByteArray> list = data.split('|'); // get a list of commands

        for (int i = 0; i < list.size(); i++)
        {
            QByteArray cmd = list[i];
            QList<QByteArray> temp = cmd.split(':');

            if (temp.size() != 2) // command without symbol ':', skip
            {
                qDebug() << "Command without split symbol :";
                continue;
            }

            if (socket == planner) // signal from planner
            {
                if (clients.contains(temp[0])) // if client exists
                {
                    //temp[1].remove(0, 1); // delete space
                    clients[temp[0]]->write(temp[1]); // send message to the unit
                }
                else // not exist
                {
                    qDebug() << "Client doesn't exist";
                }
            }
            else // signal from one of the units
            {
                // send message to the scene
                sceneSocket->write("{" + temp[0] + " : " + temp[1] + "}");
            }
        }
    }
}

void RobotControlAdapter::deleteSocket()
{
    // find the sender and cast into the socket
    QObject* object = QObject::sender();
    QTcpSocket* socket = static_cast<QTcpSocket*>(object);

    // Описать отключение сокета
}
