#include "mainwindow.h"
#include "src/headers/AwesomeGlobal.h"

#include "ui_mainwindow.h"
#include <QFont>
#include <QDebug>
#include <QString>
#include <QSettings>
#include <QMetaObject>
#include <QMessageBox>

#include <curl/curl.h>
#include <Requests.h>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , settings(new QSettings("config.ini", QSettings::IniFormat))
    , socketServer(nullptr)
    , socketServerThread(new QThread(this))
    , socketClient(nullptr)
    , clientSocketsThread(new QThread(this))
    , request(new Requests)
{
    ui->setupUi(this);

    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 1);

    ui->burger_button->setIcon(awesome->icon(fa::fa_solid, fa::fa_bars));

    ui->port_input->setText(QString::number(DEFAULT_SERVER_PORT));

    ui->chat_stacked_widget->setCurrentIndex(1);

    this->showMaximized();
    this->UploadConfig();
}

MainWindow::~MainWindow()
{
    QVariantList sizes;
    for (int& size : ui->splitter->sizes()) {
        sizes.append(size);
    }

    settings->setValue("splitterSizes", sizes);
    settings->sync();

    // To ensure that it will be destroyed in the same thread it is located, we gonna use deleteLater
    // No need to invoke here, server stops in the destructor (deleteLater calls it)
    if (socketServerThread && socketServerThread->isRunning()) {
        socketServerThread->quit();
        socketServerThread->wait();
    }

    if (clientSocketsThread && clientSocketsThread->isRunning()) {
        clientSocketsThread->quit();
        clientSocketsThread->wait();
    }

    if (socketServer){
        socketServer->deleteLater();
        socketServer = nullptr;
    }

    if (socketClient){
        socketClient->deleteLater();
        socketClient = nullptr;
    }

    delete socketServerThread;
    socketServerThread = nullptr;

    delete clientSocketsThread;
    clientSocketsThread = nullptr;

    delete settings;
    delete ui;
}


// On show, after initing all of the UI, run internal configurations
void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        PastInit();
    }
}


// Past init complex handler
void MainWindow::PastInit(){
#if !USE_LOCAL_IPV6
    QString externalIP = request->get("https://api64.ipify.org", true);
    QHostAddress addr;
    QString protocolName = "";
    if (!addr.setAddress(externalIP)) {
        QMessageBox::warning(this, "Error", "Invalid IP address received: " + externalIP);
        return;
    }

    if (addr.protocol() != QAbstractSocket::IPv6Protocol){
        QMessageBox::warning(this, "Error", "Your connection does not provide IPv6 address. Connection is unavailable.");
        // return;
    }
    else if (addr.protocol() == QAbstractSocket::IPv4Protocol){
        protocolName = "/IPv4";
    } else if(addr.setAddress(externalIP) && addr.protocol() == QAbstractSocket::IPv6Protocol){
        protocolName = "/IPv6";
    }
    ui->ip_text->setText(externalIP + protocolName);
    this->selfHostAddress = addr;
#else
    ui->ip_text->setText("::1/IPv6");
    this->selfHostAddress = QHostAddress("::1");
#endif
}


// Initial configurations
void MainWindow::InitServer(int serverPort)
{
    // Move server to Thread
    socketServerThread->start();

    // Run server into the thread by invoking
    QMetaObject::invokeMethod(socketServerThread, [this, serverPort](){
        socketServer = new IPv6ChatServer(this->selfHostAddress, serverPort);
        socketServer->run();
        connect(socketServer, &IPv6ChatServer::messageArrived, this, &MainWindow::onMessageArrived);
        connect(socketServer, &IPv6ChatServer::clientDisconnected, this, &MainWindow::onServerClientDisconnected);
        connect(socketServer, &IPv6ChatServer::clientConnected, this, &MainWindow::onServerClientConnected);
    }, Qt::QueuedConnection);
    if (socketServerThread->isRunning()){
        ui->start_server_button->setDisabled(true);
        ui->port_input->setReadOnly(true);
    }
}

void MainWindow::InitClient()
{
    clientSocketsThread->start();

    QMetaObject::invokeMethod(clientSocketsThread, [this](){
        socketClient = new IPv6ChatClient();
    }, Qt::QueuedConnection);
}

void MainWindow::UploadConfig()
{
    QVariantList  splitterSizes = settings->value("splitterSizes", QVariant::fromValue(ui->splitter->sizes())).toList();
    QList<int> sizes;
    for (QVariant& size : splitterSizes) {
        sizes.append(size.toInt());
    }
    ui->splitter->setSizes(sizes);
    if (sizes[0] < 150){
        QGridLayout *profileGrid = qobject_cast<QGridLayout*>(ui->profile_panel->layout());
        QGridLayout *ipGrid = qobject_cast<QGridLayout*>(ui->ip_panel->layout());
        QGridLayout *startServerGrid = qobject_cast<QGridLayout*>(ui->start_server_panel->layout());
        this->HideSidebarElements(profileGrid, ipGrid, startServerGrid);
    }
}

// Chat pages changing handler
void MainWindow::OpenChatPage(QString clientID)
{
    if (clientID != currentChatClientID){
        isCurrentChatClientOnline = false;
        ui->status_text->setIcon(awesome->icon(fa::fa_solid, fa::fa_times));
        ui->status_text->setText("Offline");
        currentChatClientID = clientID;
        ui->clientID_text->setText(clientID);
        ui->chat_stacked_widget->setCurrentIndex(0);
    }
}

// Custom emited thread Signals

// Client
void MainWindow::onPeerConnected(const QString& clientID)
{
    QMessageBox::information(this, "INFO", "Connected to the peer: " + clientID);

    // Save clientID to use later in DB/File/Cache.
    connect(socketClient, &IPv6ChatClient::messageSent, this, &MainWindow::onMessageSent);
    OpenChatPage(clientID);
}


void MainWindow::onMessageSent(const QString& clientID, const QByteArray& message)
{
    qDebug() << "Message sent: " << message << clientID;
}

//Server
void MainWindow::onMessageArrived(const QString& clientID, const QByteArray& message)
{
    qDebug() << "Message arrived: " << message << clientID;
}

void MainWindow::onServerClientConnected(const QString& clientID)
{
    if (clientID == currentChatClientID){
        isCurrentChatClientOnline = true;
        ui->status_text->setIcon(awesome->icon(fa::fa_solid, fa::fa_check));
        ui->status_text->setText("Online");
    }
}

void MainWindow::onServerClientDisconnected(const QString& clientID)
{
    if (clientID == currentChatClientID){
        isCurrentChatClientOnline = false;
        ui->status_text->setIcon(awesome->icon(fa::fa_solid, fa::fa_times));
        ui->status_text->setText("Offline");
    }
}

// Classic UI slot signals
void MainWindow::on_start_server_button_clicked()
{
    bool isValid = true;
    int port = ui->port_input->text().toInt(&isValid);

    if(30000 > port || port > 65535){
        isValid = false;
    }

    if(!isValid){
        ui->port_input->setStyleSheet("border: 1px solid #dc3545");
        QMessageBox::warning(this, "Error", "Provide port in range of 30000-65535");
        return;
    }

    InitServer(port);
    InitClient();
}

void MainWindow::on_port_input_textChanged()
{
    ui->port_input->setStyleSheet("border: 1px solid white");
}

void MainWindow::on_write_to_button_clicked()
{
    bool isValid = true;
    int port = ui->port_input->text().toInt(&isValid);

    if(30000 > port || port > 65535){
        isValid = false;
    }

    if(!isValid){
        QMessageBox::warning(this, "Error", "Provide port in range of 30000-65535");
        return;
    }

    QHostAddress clientAddress;
    QString clientIP = ui->client_address_input->text();

    if (clientIP.trimmed().isEmpty()) {
        QMessageBox::warning(this, "Error", "Client IP cannot be empty");
        return;
    }
    if (!clientAddress.setAddress(clientIP)) {
        QMessageBox::warning(this, "Error", "Invalid IP address received: " + clientIP);
        return;
    }
    if (clientAddress.protocol() != QAbstractSocket::IPv6Protocol){
        QMessageBox::warning(this, "Error", "Address is not IPv6 address. Connection is unavailable.");
        return;
    }

    QMetaObject::invokeMethod(clientSocketsThread, [this, clientAddress, port](){
        socketClient->connectToPeer(clientAddress.toString(), port);
    }, Qt::QueuedConnection);

    connect(socketClient, &IPv6ChatClient::peerConnected, this, &MainWindow::onPeerConnected);
}

void MainWindow::on_port_input_returnPressed()
{
    on_start_server_button_clicked();
}

void MainWindow::on_client_address_input_returnPressed()
{
    on_write_to_button_clicked();
}

void MainWindow::on_client_port_input_returnPressed()
{
    on_write_to_button_clicked();
}

void MainWindow::on_send_message_button_clicked()
{
    QString message = ui->send_message_input->toPlainText();
    if (message.trimmed().isEmpty()){
        ui->send_message_input->setFocus();
        return;
    }

    QString clientID = ui->clientID_text->text();

    QMetaObject::invokeMethod(clientSocketsThread, [this, clientID, message](){
        socketClient->sendMessage(clientID, message.toUtf8());
    }, Qt::QueuedConnection);
}


// UI handlers
void MainWindow::on_splitter_splitterMoved(int pos, int index)
{
    int sidebarWidth = ui->sidebar->width();
    QGridLayout *profileGrid = qobject_cast<QGridLayout*>(ui->profile_panel->layout());
    QGridLayout *ipGrid = qobject_cast<QGridLayout*>(ui->ip_panel->layout());
    QGridLayout *startServerGrid = qobject_cast<QGridLayout*>(ui->start_server_panel->layout());
    if (sidebarWidth < 150){
        this->HideSidebarElements(profileGrid, ipGrid, startServerGrid);
    }else{
        this->ShowSidebarElements(profileGrid, ipGrid, startServerGrid);
    }
}

void MainWindow::HideSidebarElements(QGridLayout *profileGrid, QGridLayout *ipGrid, QGridLayout *startServerGrid)
{
    ui->search_line->setVisible(false);
    ui->your_ip_label->setVisible(false);
    ui->port_input->setVisible(false);
    ui->port_warning->setVisible(false);
    profileGrid->setColumnStretch(1, 0);
    profileGrid->update();
    ipGrid->setColumnStretch(0, 1);
    ipGrid->update();
    startServerGrid->setColumnStretch(1, 0);
    startServerGrid->update();

}

void MainWindow::ShowSidebarElements(QGridLayout *profileGrid, QGridLayout *ipGrid, QGridLayout *startServerGrid)
{
    ui->search_line->setVisible(true);
    ui->your_ip_label->setVisible(true);
    ui->port_input->setVisible(true);
    ui->port_warning->setVisible(true);
    profileGrid->setColumnStretch(1, 6);
    profileGrid->update();
    ipGrid->setColumnStretch(1, 6);
    ipGrid->update();
    startServerGrid->setColumnStretch(1, 6);
    startServerGrid->update();
}
