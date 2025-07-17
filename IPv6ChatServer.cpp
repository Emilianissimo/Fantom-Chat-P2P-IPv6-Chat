#include "IPv6ChatServer.h"
#include <QDebug>
#include <QTcpServer>

IPv6ChatServer::IPv6ChatServer(QHostAddress addr, int port, QObject* parent)
    : QObject(parent), server(nullptr), addr(addr), port(port) {}

IPv6ChatServer::~IPv6ChatServer() {
    stopServer();
    delete server;
    qDebug() << "Server: Killed server.";
}

void IPv6ChatServer::run() {
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

// New client connected
void IPv6ChatServer::onNewConnection() {
    QTcpSocket* client = server->nextPendingConnection();
    if (!client) return;

    QString clientID = client->peerAddress().toString();
    qDebug() << "Server: New client connected: " << clientID;

    clientsMutex.lock();
    clients[clientID] = client;
    clientsMutex.unlock();

    connect(client, &QTcpSocket::readyRead, this, &IPv6ChatServer::onReadyRead);
    connect(client, &QTcpSocket::disconnected, this, &IPv6ChatServer::onClientDisconnected);
}

// Read message incoming from client
void IPv6ChatServer::onReadyRead() {
    qDebug() << "Server: ready Read runs";
    QTcpSocket* senderClient = qobject_cast<QTcpSocket*>(sender());
    if (!senderClient) return;

    QByteArray message = senderClient->readAll();
    QString senderID = senderClient->peerAddress().toString();
    qDebug() << "Server: Received message from: " << senderID << ":" << message;

    QList<QByteArray> parts = message.split(':');
    if (parts.size() < 2) return;

    QString receiverID = parts[0];
    QByteArray actualMessage = parts[1];

    sendMessageToSelfClient(receiverID, actualMessage);
}

void IPv6ChatServer::sendMessageToSelfClient(const QString& clientID, const QByteArray& message) {
    // Send message to my own client (only UI, not the client actually, I suppose)
    // find self client in map (need to understand what identificator to use locally)
    clientsMutex.lock();
    if (clients.contains(clientID)){
        QTcpSocket* targetClient = clients[clientID];
        targetClient->write(message);
    }else{
        qDebug() << "Server: Client not found: " + clientID;
    }
    clientsMutex.unlock();
}

// Client disconnected
void IPv6ChatServer::onClientDisconnected() {
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;

    QString clientID = client->peerAddress().toString();
    qDebug() << "Server: Client disconnected: " << clientID;

    clientsMutex.lock();
    clients.remove(clientID);
    clientsMutex.unlock();

    client->deleteLater();
}

// Server stopping
void IPv6ChatServer::stopServer() {
    if (server) {
        server->close();
    }
    clientsMutex.lock();
    for (QTcpSocket*& client : clients) {
        client->disconnectFromHost();
    }
    clients.clear();
    clientsMutex.unlock();
}
