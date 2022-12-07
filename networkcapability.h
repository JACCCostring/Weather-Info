#ifndef NETWORKCAPABILITY_H
#define NETWORKCAPABILITY_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class networkCapability :public QTcpServer
{
    Q_OBJECT
public:
    explicit networkCapability(QObject *parent = nullptr);
    //methods
    void incomingConnection(qintptr handle) override;
    void writeToClient(const QByteArray&);
    void startServer();
    void stopServer();
    bool isOnline();

signals:
    void entityData(const QByteArray&);
    void disconectHost();

private slots:
    void workConnection();
    void readingFromHost();

private:
    QTcpSocket *socket;
    bool isOn;
};

#endif // NETWORKCAPABILITY_H
