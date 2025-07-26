#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "IPv6ChatServer.h"
#include "IPv6ChatClient.h"
#include "MessageListModel.h"

#include <QMainWindow>
#include <QSettings>
#include <QGridLayout>
#include <Requests.h>

const int DEFAULT_SERVER_PORT = 31488;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void PastInit();
    void UploadConfig();
    void HideSidebarElements(QGridLayout *profileGrid, QGridLayout *ipGrid, QGridLayout *startServerGrid);
    void ShowSidebarElements(QGridLayout *profileGrid, QGridLayout *ipGrid, QGridLayout *startServerGrid);
    void InitServer(int serverPort = DEFAULT_SERVER_PORT);
    void InitClient();

protected:
    void showEvent(QShowEvent *event) override;

private:
    Ui::MainWindow *ui;
    QSettings *settings;
    IPv6ChatServer *socketServer;
    QThread *socketServerThread;
    IPv6ChatClient *socketClient;
    QThread *clientSocketsThread;
    QHostAddress selfHostAddress;
    Requests *request;
    QString currentChatClientID;
    QMap<QString, QList<Message>> messages;
    bool isCurrentChatClientOnline = false;
    MessageListModel* currentMessageModel = nullptr;

    void openChatPage(const QString& clientID);
    void setUpMessagesForChat(const QString& clientID);
    QString getLocalIPv6Address();

private slots:
    void on_splitter_splitterMoved(int pos, int index);
    void on_start_server_button_clicked();
    void on_port_input_textChanged();
    void on_write_to_button_clicked();
    void on_port_input_returnPressed();
    void on_client_address_input_returnPressed();
    void on_client_port_input_returnPressed();
    void on_send_message_button_clicked();

    // Client
    void onPeerConnected(const QString& clientID);
    void onMessageSent(const QString& clientID, const QByteArray& message);

    // Server
    void onServerClientConnected(const QString& clientID);
    void onServerClientDisconnected(const QString& clientID);
    void onMessageArrived(const QString& clientID, const QByteArray& message);
};
#endif // MAINWINDOW_H
