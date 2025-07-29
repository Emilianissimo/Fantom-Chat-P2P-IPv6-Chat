#include "IPv6ChatServer.h"
#include "../utils/ProtocolUtils.h"
#include <QDebug>
#include <QTcpServer>

IPv6ChatServer::IPv6ChatServer(QHostAddress addr, int port, QObject* parent)
    : QObject(parent), server(nullptr), addr(addr), port(port) {}

IPv6ChatServer::~IPv6ChatServer() {
    stopServer();
    qDebug() << "Server: Killed server.";
}

void IPv6ChatServer::run() {
    // Server initiating and strating to listen the port on self Thread
    if (server) return;
    server = new QTcpServer(this);
    connect(server, &QTcpServer::newConnection, this, &IPv6ChatServer::onNewConnection);

    // Garbage: QHostAddress::AnyIPv6
    if (!server->listen(this->addr, port)) {
        qCritical() << "Server: failed to start: " << server->errorString();
        return;
    }

    qDebug() << "Server: started on address: " << "["+ this->addr.toString() + "]:" + QString::number(port);
}

void IPv6ChatServer::onNewConnection() {
    // New client connected
    // Each connected client will be appended to the list of client connections from server side
    // TODO: parse list of connected clients as address book list as available to send message
    QTcpSocket* socket = server->nextPendingConnection();
    if (!socket) return;

    QString clientID = QString("%1:%2")
       .arg(socket->peerAddress().toString())
       .arg(socket->peerPort());

    QMutexLocker locker(&clientsMutex);
    clients.insert(clientID, {clientID, socket});

    connect(socket, &QTcpSocket::readyRead, this, &IPv6ChatServer::onReadyRead, Qt::QueuedConnection);
    connect(socket, &QTcpSocket::disconnected, this, &IPv6ChatServer::onClientDisconnected);

    qDebug() << "Server: New client connected: " << clientID;

    emit clientConnected(clientID);
}

void IPv6ChatServer::onReadyRead() {
    // Read message incoming from client
    // Any message from client will be get and separated by its id
    qDebug() << "Server: ready Read runs";
    QTcpSocket* senderClient = qobject_cast<QTcpSocket*>(sender());
    if (!senderClient) return;

    // We have to ensure, that we get full message and determine client as one for this message
    QByteArray& buffer = socketBuffers[senderClient];
    buffer.append(senderClient->readAll());
    while (buffer.size() >= 4) {
        quint32 msgLen = readUInt32(buffer.left(4));
        if (buffer.size() < 4 + msgLen) break;

        QByteArray fullMessage = buffer.mid(4, msgLen);
        buffer.remove(0, 4 + msgLen);
        int sepIndex = fullMessage.indexOf('\0');
        if (sepIndex == -1) continue;

        QString clientID;
        QMutexLocker locker(&clientsMutex);
        auto client = std::find_if(clients.begin(), clients.end(), [&](const PeerConnection& c){
            return c.socket == senderClient;
        });
        if (client != clients.end()) {
            clientID = client.value().clientID;
        }
        QByteArray message = fullMessage.mid(sepIndex + 1);
        qDebug() << "Server: Received message from: " << clientID << ":" << message;

        emit messageArrived(clientID, message);
    }
}

void IPv6ChatServer::onClientDisconnected() {
    // Client disconnected
    // On disconnection each client, we have to ensure socket delete
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QString disconnectedID;
    QMutexLocker locker(&clientsMutex);
    auto client = std::find_if(clients.begin(), clients.end(), [&](const PeerConnection& c){
        return c.socket == socket;
    });
    if (client != clients.end()) {
        qDebug() << "Disconnected:" << client.value().clientID;
        disconnectedID = client.value().clientID;
        clients.erase(client);
    }

    emit clientDisconnected(disconnectedID);

    socket->deleteLater();
}

void IPv6ChatServer::stopServer() {
    // Server stopping
    // Kill the server and clear clients list, not forgetting to disconnect each
    if (server) {
        server->close();
    }
    QMutexLocker locker(&clientsMutex);
    for (const PeerConnection& client : clients.values()) {
        client.socket->disconnectFromHost();
    }
    clients.clear();
}
