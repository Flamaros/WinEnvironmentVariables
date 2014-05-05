#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Registry.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadVariablesFromRegistry();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void    MainWindow::loadVariablesFromRegistry()
{
    Registry::Key   path;

    path.open("HKEY_LOCAL_MACHINE/SYSTEM/CurrentControlSet/Control/Session Manager/Environment");
}
