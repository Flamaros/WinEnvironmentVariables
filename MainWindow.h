#pragma once

#include <QMainWindow>

#include "Environment.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pathsTable_itemSelectionChanged();

private:
    void    loadVariables();

    Ui::MainWindow *ui;

    Environment mEnvironment;
};
