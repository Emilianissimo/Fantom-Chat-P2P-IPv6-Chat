#include "mainwindow.h"
#include "src/headers/AwesomeGlobal.h"

#include "ui_mainwindow.h"
#include <QFont>
#include <QString>
#include <QSettings>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , settings(new QSettings("config.ini", QSettings::IniFormat))
{
    ui->setupUi(this);

    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 1);

    ui->burger_button->setIcon(awesome->icon(fa::fa_solid, fa::fa_bars));

    this->showMaximized();
    this->UploadConfig();
}

MainWindow::~MainWindow()
{
    QVariantList sizes;
    for (int size : ui->splitter->sizes()) {
        sizes.append(size);
    }

    settings->setValue("splitterSizes", sizes);
    settings->sync();
    delete settings;
    delete ui;
}

void MainWindow::UploadConfig()
{
    QVariantList  splitterSizes = settings->value("splitterSizes", QVariant::fromValue(ui->splitter->sizes())).toList();
    QList<int> sizes;
    for (const QVariant &size : splitterSizes) {
        sizes.append(size.toInt());
    }
    ui->splitter->setSizes(sizes);
    if (sizes[0] < 150){
        QGridLayout *profileGrid = qobject_cast<QGridLayout*>(ui->profile_panel->layout());
        QGridLayout *ipGrid = qobject_cast<QGridLayout*>(ui->ip_panel->layout());
        this->HideSidebarElements(profileGrid, ipGrid);
    }
}

void MainWindow::on_splitter_splitterMoved(int pos, int index)
{
    int sidebarWidth = ui->sidebar->width();
    QGridLayout *profileGrid = qobject_cast<QGridLayout*>(ui->profile_panel->layout());
    QGridLayout *ipGrid = qobject_cast<QGridLayout*>(ui->ip_panel->layout());
    if (sidebarWidth < 150){
        this->HideSidebarElements(profileGrid, ipGrid);
    }else{
        this->ShowSidebarElements(profileGrid, ipGrid);
    }
}

void MainWindow::HideSidebarElements(QGridLayout *profileGrid, QGridLayout *ipGrid){
    ui->search_line->setVisible(false);
    ui->your_ip_label->setVisible(false);
    profileGrid->setColumnStretch(1,0);
    profileGrid->update();
    ipGrid->setColumnStretch(0, 1);
    ipGrid->update();
}

void MainWindow::ShowSidebarElements(QGridLayout *profileGrid, QGridLayout *ipGrid){
    ui->search_line->setVisible(true);
    ui->your_ip_label->setVisible(true);
    profileGrid->setColumnStretch(1,6);
    profileGrid->update();
    ipGrid->setColumnStretch(1, 6);
    ipGrid->update();
}
