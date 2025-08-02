#ifndef IPV6CHATCLIENT_H
#define IPV6CHATCLIENT_H

#include "../utils/Structures.h"
#include "../encrypting/interfaces/ICryptoKeyPair.h"
#include "../encrypting/interfaces/ICryptoSession.h"
#include "../encrypting/interfaces/ICryptoBackend.h"
#include <QMap>
#include <QMutex>
#include <QObject>
#include <QTcpSocket>

class IPv6ChatClient : public QObject {
    Q_OBJECT

public:
    std::shared_ptr<ICryptoBackend> cryptoBackend;

    explicit IPv6ChatClient(QObject* parent = nullptr);
    void connectToPeer(const QString& address, int port);
    void sendMessage(const QString& selfHost, const QString& clientID, const QByteArray& message);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onSocketError(QAbstractSocket::SocketError);

private:
    QMutex connectionsMutex;
    QMap<QString, PeerConnection> connections;
    QHash<QTcpSocket*, bool> handshakeStatus;

    QHash<QTcpSocket*, std::unique_ptr<ICryptoKeyPair>> clientKeys;
    QHash<QTcpSocket*, std::unique_ptr<ICryptoSession>> sessions;


    QString findClientID(QTcpSocket* socket);

signals:
    void peerConnected(const QString& clientID);
    void peerDisconnected(const QString& clientID);
    void messageSent(const QString& clientID, const QByteArray& message);
};

#endif // IPV6CHATCLIENT_H
