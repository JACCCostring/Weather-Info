#include "networkcapability.h"

networkCapability::networkCapability(QObject *parent) : QTcpServer(parent){}//constructor

void networkCapability::incomingConnection(qintptr handle)
{
    QTcpSocket *host = new QTcpSocket(this);
    if(!host->setSocketDescriptor(handle)){
      qDebug()<<"Socket can not be created!";
      }
  //connecting
  connect(this, SIGNAL(newConnection()), this, SLOT(workConnection()));
  connect(host, SIGNAL(readyRead()), this, SLOT(readingFromHost()));
  connect(this, SIGNAL(disconectHost()), host, SLOT(deleteLater()));//delete host socket
}

void networkCapability::startServer()
{
    if(!this->isListening()){
        //then open to listen
        this->listen(QHostAddress::Any, 6574);
        //isOn = true;
        qDebug()<<"Server listening...";
    }
    else{
    qDebug()<<"Server allready listening";
    }
}

void networkCapability::stopServer()
{
    isOn = false;
    this->close();
    emit disconectHost();
}

bool networkCapability::isOnline()
{
    return isOn;
}

void networkCapability::writeToClient(const QByteArray &data)
{
    if(isOn && ! data.isEmpty()){ //only if server is online and no empty
        socket->write(data);//write to socket
    }
}

void networkCapability::workConnection(){
    //QTcpSocket *newHost = qobject_cast<QTcpSocket*>(sender());//getting same socket from QOBJECT::conenct()
    qDebug()<<"new connection";
}

void networkCapability::readingFromHost()
{
    QTcpSocket *newHost = qobject_cast<QTcpSocket*>(sender());//getting same socket from QOBJECT::connect()
    QByteArray data = newHost->readAll();
    socket = newHost;
    isOn = true;
    //emitting signal
    emit entityData(data);
}
