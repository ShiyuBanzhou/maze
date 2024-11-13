#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QPainter>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QWidget {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void paintEvent(QPaintEvent *event); // 重载绘制主背景
signals:
    void beginGame(); // 开始游戏按钮信号
    void mapCreate(); // 制作地图按钮信号

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
