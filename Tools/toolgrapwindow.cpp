#include "toolgrapwindow.h"

ToolGrapWindow::ToolGrapWindow(QObject *parent) : QObject(parent)
{
    timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,&ToolGrapWindow::running);

}

QPixmap ToolGrapWindow::grapWithOutMouse(WId window, int x, int y, int width, int height)
{
    //QPixmap::grabWindow(window, x, y, width, height);

    return QPixmap::grabWindow(window);
}


QPixmap ToolGrapWindow::grapWithMouse(HWND winId, int x, int y, int w, int h)
{

        QPixmap pixmap;
        RECT r;
        GetClientRect(winId, &r);

        if (w < 0) w = r.right - r.left;
        if (h < 0) h = r.bottom - r.top;

        HDC display_dc = GetDC(winId);
        HDC bitmap_dc = CreateCompatibleDC(display_dc);
        HBITMAP bitmap = CreateCompatibleBitmap(display_dc, w, h);
        HGDIOBJ null_bitmap = SelectObject(bitmap_dc, bitmap);

        BitBlt(bitmap_dc, 0, 0, w, h, display_dc, x, y, SRCCOPY | CAPTUREBLT);

        CURSORINFO ci;
        ci.cbSize = sizeof(CURSORINFO);
        GetCursorInfo(&ci);
        if((ci.ptScreenPos.x > x) && (ci.ptScreenPos.y > y) && (ci.ptScreenPos.x < (x+w)) && (ci.ptScreenPos.y < (y+h)))
            DrawIcon(bitmap_dc, ci.ptScreenPos.x-x, ci.ptScreenPos.y-y, ci.hCursor);

        // clean up all but bitmap
        ReleaseDC(winId, display_dc);
        SelectObject(bitmap_dc, null_bitmap);
        DeleteDC(bitmap_dc);

        pixmap = QtWin::fromHBITMAP(bitmap);

        DeleteObject(bitmap);

        return pixmap;
}

void ToolGrapWindow::start(int interval)
{
    timer->start(interval);
}

void ToolGrapWindow::changeInterval(int interval)
{
    timer->setInterval(interval);
}

void ToolGrapWindow::stop()
{
    timer->stop();
}

void ToolGrapWindow::running()
{
    //QImage img = this->grapWithOutMouse().toImage();
    QImage img = this->grapWithMouse().toImage();
    emit signalPushSreen(img);
}
