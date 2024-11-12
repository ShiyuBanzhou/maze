#ifndef MAPSELECTWINDOW_H
#define MAPSELECTWINDOW_H

#include <QWidget>

namespace Ui {
class MapSelectWindow;
}

class MapSelectWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MapSelectWindow(QWidget *parent = nullptr);
    ~MapSelectWindow();

signals:
    void levelSelected(int level); // 关卡选择信号
    void backBtnClicked(); // 返回按钮信号

private:
    Ui::MapSelectWindow *ui;
    void paintEvent(QPaintEvent *event);
};

#endif // MAPSELECTWINDOW_H
