#ifndef IPV6CHATCLIENT_H
#define IPV6CHATCLIENT_H

#include <QObject>
#include <QTcpSocket>

class IPv6ChatClient : public QObject {
    Q_OBJECT

public:
    explicit IPv6ChatClient(const QString& serverAddress, int port, QObject* parent = nullptr);
    void sendMessage(const QString& senderID, const QByteArray& message);

private slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();

private:
    QTcpSocket socket;
};

#endif // IPV6CHATCLIENT_H
