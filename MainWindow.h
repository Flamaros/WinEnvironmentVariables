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

private:
    void    loadVariables();

    Ui::MainWindow *ui;

    Environment mEnvironment;
};
