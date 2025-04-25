#include "mainwindow.h"
#include "src/headers/AwesomeGlobal.h"

#include "ui_mainwindow.h"
#include <QFont>
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
{
    ui->setupUi(this);

    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 1);

    ui->burger_button->setIcon(awesome->icon(fa::fa_solid, fa::fa_bars));

    ui->port_input->setText(QString::number(DEFAULT_SERVER_PORT));

    this->showMaximized();
    this->UploadConfig();

    PastInit();
}

void MainWindow::PastInit(){
    QString externalIP = Requests::get("https://api64.ipify.org", true);
    QHostAddress addr;
    QString protocolName = "";
    if (!addr.setAddress(externalIP) || addr.protocol() != QAbstractSocket::IPv6Protocol){
        QMessageBox::warning(this, "Error", "Your connection does not provide IPv6 address. Connection is unavailable.");
        // return;
    }
    if (addr.setAddress(externalIP) && addr.protocol() == QAbstractSocket::IPv4Protocol){
        protocolName = "/IPv4";
    }else if(addr.setAddress(externalIP) && addr.protocol() == QAbstractSocket::IPv6Protocol){
        protocolName = "/IPv6";
    }
    ui->ip_text->setText(externalIP + protocolName);
}

void MainWindow::InitServer(int serverPort)
{
    // Move server to Thread
    socketServerThread->start();

    // Run server into the thread by invoking
    QMetaObject::invokeMethod(this, [this, serverPort](){
        socketServer = new IPv6ChatServer(serverPort);
        socketServer->run();
    }, Qt::QueuedConnection);
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
    if (socketServer){
        socketServer->deleteLater();
        socketServer = nullptr;
    }

    if (socketServerThread && socketServerThread->isRunning()){
        socketServerThread->quit();
        socketServerThread->wait();
        delete socketServerThread;
        socketServerThread = nullptr;
    }

    delete settings;
    delete ui;
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

void MainWindow::HideSidebarElements(QGridLayout *profileGrid, QGridLayout *ipGrid, QGridLayout *startServerGrid){
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

void MainWindow::ShowSidebarElements(QGridLayout *profileGrid, QGridLayout *ipGrid, QGridLayout *startServerGrid){
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
    }
}


void MainWindow::on_port_input_textChanged(const QString &arg1)
{
    ui->port_input->setStyleSheet("border: 1px solid white");
}

