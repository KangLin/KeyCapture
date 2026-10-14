#include <QLoggingCategory>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XInput2.h>

#include "XIThread.h"

static Q_LOGGING_CATEGORY(log, "XIThread")
CXIThread::CXIThread(QObject *parent)
    : QThread(parent)
    , m_stop(false)
    , m_pDisplay(nullptr)
    , m_bOwnDisplay(false)
    , xi_opcode(-1)
{
}

CXIThread::~CXIThread()
{
    if(m_pDisplay && m_bOwnDisplay)
        XCloseDisplay(m_pDisplay);
}

bool CXIThread::init(Display *dpy)
{
    if(!dpy) {
        m_pDisplay = XOpenDisplay(nullptr);
        if (!m_pDisplay) {
            qFatal("Cannot open X display");
            return false;
        }
        m_bOwnDisplay = true;
    } else
        m_pDisplay = dpy;

    if(!m_pDisplay) {
        qFatal("X display is null");
        return false;
    }

    int event, error;
    if (!XQueryExtension(m_pDisplay, "XInputExtension", &xi_opcode, &event, &error)) {
        qCritical(log) << "XInput extension not available";
        return false;
    }
    int major = 2, minor = 2;
    if (XIQueryVersion(m_pDisplay, &major, &minor) != Success) {
        qCritical(log) << "XIQueryVersion failed";
        return false;
    }

    Window root = DefaultRootWindow(m_pDisplay);
    unsigned char mask[(XI_LASTEVENT + 7) / 8];
    memset(mask, 0, sizeof(mask));
    XIEventMask evmask;
    evmask.deviceid = XIAllMasterDevices;
    evmask.mask_len = sizeof(mask);
    evmask.mask = mask;
    XISetMask(mask, XI_RawKeyPress);
    XISetMask(mask, XI_RawKeyRelease);
    XISelectEvents(m_pDisplay, root, &evmask, 1);
    XFlush(m_pDisplay);
    return true;
}

void CXIThread::stopAndWait()
{
    m_stop.store(true);
    if (isRunning()) wait();
}

void CXIThread::run()
{
    if (!m_pDisplay) {
        qWarning() << "Display not initialized in XIThread";
        return;
    }

    while (!m_stop.load()) {
        if (XPending(m_pDisplay) == 0) {
            msleep(10);
            continue;
        }

        XEvent ev;
        XNextEvent(m_pDisplay, &ev);

        if (ev.type == GenericEvent) {
            XGenericEventCookie *cookie = &ev.xcookie;
            if (cookie->type == GenericEvent && cookie->extension == xi_opcode) {
                if (XGetEventData(m_pDisplay, cookie)) {
                    if (cookie->evtype == XI_RawKeyPress || cookie->evtype == XI_RawKeyRelease) {
                        XIRawEvent *re = reinterpret_cast<XIRawEvent*>(cookie->data);
                        bool pressed = (cookie->evtype == XI_RawKeyPress);
                        int detail = re->detail;
                        int deviceid = re->deviceid;
                        emit rawKey(pressed, detail, deviceid);
                    }
                    XFreeEventData(m_pDisplay, cookie);
                }
            }
        }
    }
}