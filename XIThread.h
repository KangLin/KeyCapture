#ifndef XITHREAD_H
#define XITHREAD_H

#include <QThread>

// 为了在类声明中使用 Display* 类型，做前向声明（不包含 X11 头）
typedef struct _XDisplay Display;

class CXIThread : public QThread
{
    Q_OBJECT
public:
    explicit CXIThread(QObject *parent = nullptr);
    virtual ~CXIThread();

    // init 接受一个 Display* 指针（前向声明足够用于声明）
    bool init(Display *dpy = nullptr);
    void stopAndWait();

signals:
    void rawKey(bool pressed, int rawKeycode, int deviceid);

protected:
    void run() override;

private:
    std::atomic<bool> m_stop;
    Display *m_pDisplay;
    bool m_bOwnDisplay;
    int xi_opcode;
};

#endif // XITHREAD_H
