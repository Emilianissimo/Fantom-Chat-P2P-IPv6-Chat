#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "IPv6ChatServer.h"

#include <QMainWindow>
#include <QSettings>
#include <QGridLayout>

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

    void UploadConfig();
    void HideSidebarElements(QGridLayout *profileGrid, QGridLayout *ipGrid);
    void ShowSidebarElements(QGridLayout *profileGrid, QGridLayout *ipGrid);
    void PastInit();

private slots:
    void on_splitter_splitterMoved(int pos, int index);

private:
    Ui::MainWindow *ui;
    QSettings *settings;
    IPv6ChatServer *socketServer;
    QThread *socketServerThread;
};
#endif // MAINWINDOW_H
