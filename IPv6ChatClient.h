#ifndef IPV6CHATCLIENT_H
#define IPV6CHATCLIENT_H

#include <QMap>
#include <QObject>
#include <QTcpSocket>

class IPv6ChatClient : public QObject {
    Q_OBJECT

public:
    explicit IPv6ChatClient(QObject* parent = nullptr);
    void connectToPeer(const QString& address, int port);
    void sendMessage(const QString& peerID, const QByteArray& message);

private slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();

private:
    struct PeerConnection {
        QTcpSocket* socket;
        QString peerID;
    };
    QMap<QString, PeerConnection> connections;
};

#endif // IPV6CHATCLIENT_H
