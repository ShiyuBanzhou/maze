#ifndef IMGBUTTON_H
#define IMGBUTTON_H

#include <QPushButton>
#include <QSoundEffect>

class ImgButton : public QPushButton{
    Q_OBJECT

public:
    /*
     * 构造方法：
     * 1. 缺省downImgPath,只作出按下图片偏移效果
     * 2. 作出按下与弹起按钮图片不同的动画效果
    */
    // 1.
    ImgButton(QString upImgPath, QString soundPath);
    // 2.
    ImgButton(QString upImgPath, QString downImgPath, QString soundPath);

    /*
     * 动画效果：
    */
    void buttonUp(); // 按钮弹起动画
    void buttonDown(); // 按钮按下动画
    void mousePressEvent(QMouseEvent *e); // 重写鼠标按下事件
    void mouseReleaseEvent(QMouseEvent *e); // 重写鼠标释放事件

    /*
     *设置：
    */
    // 按钮设置 -1:异常 0:正常
    int setupButton(QString imgPath);
    // 声音设置
    void setSound(QString soundPath);

    /*
     *外部访问：
    */
    QString getUpImgPath();
    QString getDownImgPath();

private:
    QString upImgPath; // 未按下时按钮img路径
    QString downImgPath = ""; // 按下时按钮img路径
    QSoundEffect *sound = NULL;

signals:

};

#endif // IMGBUTTON_H
