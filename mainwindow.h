#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ffuser.h"
using namespace FF;

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
    void on_pushButton_load_clicked();

    void on_pushButton_encode_clicked();

    void on_pushButton_decode_clicked();

private:
    Ui::MainWindow *ui;
    FFUser *decode,  *encode;
};

#endif // MAINWINDOW_H
