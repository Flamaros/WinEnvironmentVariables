#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Environment.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadVariables();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void    MainWindow::loadVariables()
{
    QList<Environment::Variable>    variables;

    variables = mEnvironment.simpleVariables();
    ui->variablesTable->setRowCount(variables.size());
    for (int i = 0; i < variables.size(); i++)
    {
        QTableWidgetItem*   newItem;

        newItem = new QTableWidgetItem(variables[i].name);
        ui->variablesTable->setItem(i, 0, newItem);

        newItem = new QTableWidgetItem(variables[i].owner);
        ui->variablesTable->setItem(i, 1, newItem);

        newItem = new QTableWidgetItem(variables[i].value);
        ui->variablesTable->setItem(i, 2, newItem);
    }

    ui->pathsTable->resizeColumnsToContents();
    ui->pathValuesTable->resizeColumnsToContents();
    ui->variablesTable->resizeColumnsToContents();
}
