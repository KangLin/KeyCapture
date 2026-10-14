#include <QLoggingCategory>

#include "mainwindow.h"
#include "ui_mainwindow.h"

static Q_LOGGING_CATEGORY(log, "MainWindow")

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_pXIThread(nullptr)
{
    ui->setupUi(this);

    setWindowTitle("XI2 Raw Key Capture (Xorg)");
    ui->lbLabel->setText("Press 'Start' to begin listening for XI2 raw events");
    ui->pbStopCapture->setDisabled(true);

    m_pXIThread = new CXIThread(this);
    bool check = connect(m_pXIThread, &CXIThread::rawKey, [&](bool pressed, int rawKeycode, int deviceid) {
        int approx_scancode = (rawKeycode > 8) ? (rawKeycode - 8) : rawKeycode;
        qDebug(log) << (pressed ? "PRESS" : "RELEASE")
                    << "Xkeycode:" << rawKeycode
                    << "approx_linux_scancode:" << approx_scancode
                    << "deviceid:" << deviceid;
    });
    Q_ASSERT(check);
}

MainWindow::~MainWindow()
{
    if(m_pXIThread) {
        m_pXIThread->stopAndWait();
        delete m_pXIThread;
    }

    delete ui;
}

void MainWindow::on_pbStartCapture_clicked()
{
    if (!m_pXIThread->isRunning()) {
        if (!m_pXIThread->init()) {
            qWarning() << "XIThread init failed";
            return;
        }
        m_pXIThread->start();
        ui->lbLabel->setText("Listening (XI_RawKeyPress/XI_RawKeyRelease). Check console output.");
        ui->pbStopCapture->setEnabled(true);
        ui->pbStartCapture->setEnabled(false);
    }
}

void MainWindow::on_pbStopCapture_clicked()
{
    m_pXIThread->stopAndWait();
    ui->lbLabel->setText("Press 'Start' to begin listening for XI2 raw events");
    ui->pbStopCapture->setEnabled(false);
    ui->pbStartCapture->setEnabled(true);
}
