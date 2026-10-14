#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "XIThread.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_pbStartCapture_clicked();
    void on_pbStopCapture_clicked();

private:
    Ui::MainWindow *ui;

    CXIThread *m_pXIThread;
};
#endif // MAINWINDOW_H
