#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "IPv6ChatServer.h"

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

private slots:
    void on_splitter_splitterMoved(int pos, int index);

    void on_start_server_button_clicked();

    void on_port_input_textChanged(const QString &arg1);

protected:
    void showEvent(QShowEvent *event) override;

private:
    Ui::MainWindow *ui;
    QSettings *settings;
    IPv6ChatServer *socketServer;
    QThread *socketServerThread;
    QHostAddress selfHostAddress;
    Requests *request;
};
#endif // MAINWINDOW_H
