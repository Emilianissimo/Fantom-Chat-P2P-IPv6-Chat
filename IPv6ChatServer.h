#ifndef IPV6CHATSERVER_H
#define IPV6CHATSERVER_H

#include "Structures.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QMutex>
#include <QVector>
#include <QMap>


class IPv6ChatServer : public QObject {
    Q_OBJECT

private:
    QTcpServer *server;
    QMutex clientsMutex;
    QString myClientID;
    QHostAddress addr;
    int port;
    QMap<QString, PeerConnection> clients;
    QMap<QTcpSocket*, QByteArray> socketBuffers;

public:
    explicit IPv6ChatServer(QHostAddress addr, int port, QObject* parent = nullptr);
    ~IPv6ChatServer();

public slots:
    void run();
    void onNewConnection();
    void onReadyRead();
    void onClientDisconnected();
    void stopServer();

signals:
    void messageArrived(const QString& clientID, const QByteArray& message);

    void clientConnected(const QString& clientID);
    void clientDisconnected(const QString& clientID);
};
#endif // IPV6CHATSERVER_H
