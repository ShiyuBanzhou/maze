#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPainter>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void paintEvent(QPaintEvent *event); // 重载绘制主背景

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
