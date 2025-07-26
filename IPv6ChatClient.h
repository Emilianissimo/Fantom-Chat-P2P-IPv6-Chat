#ifndef IPV6CHATCLIENT_H
#define IPV6CHATCLIENT_H

#include "Structures.h"
#include <QMap>
#include <QMutex>
#include <QObject>
#include <QTcpSocket>

class IPv6ChatClient : public QObject {
    Q_OBJECT

public:
    explicit IPv6ChatClient(QObject* parent = nullptr);
    void connectToPeer(const QString& address, int port);
    void sendMessage(const QString& clientID, const QByteArray& message);

private slots:
    void onConnected();
    void onDisconnected();

private:
    QMutex connectionsMutex;
    QMap<QString, PeerConnection> connections;

signals:
    void peerConnected(const QString& clientID);
    void messageSent(const QString& clientID, const QByteArray& message);
};

#endif // IPV6CHATCLIENT_H
