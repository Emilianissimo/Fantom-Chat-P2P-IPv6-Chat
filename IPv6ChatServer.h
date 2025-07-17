#ifndef IPV6CHATSERVER_H
#define IPV6CHATSERVER_H
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
    QMap<QString, QTcpSocket*> clients;
    QMutex clientsMutex;
    QString myClientID;
    QHostAddress addr;
    int port;

public:
    explicit IPv6ChatServer(QHostAddress addr, int port, QObject* parent = nullptr);
    ~IPv6ChatServer();

public slots:
    void run();
    void onNewConnection();
    void onReadyRead();
    void onClientDisconnected();
    void sendMessageToSelfClient(const QString& clientID, const QByteArray& message);
    void stopServer();
};
#endif // IPV6CHATSERVER_H
