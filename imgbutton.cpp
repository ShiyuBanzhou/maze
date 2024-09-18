#include "imgbutton.h"

#include <QDebug>
#include <QMessageBox>
#include <QCoreApplication>
#include <QPropertyAnimation>

// 构造函数
ImgButton::ImgButton(QString upImgPath, QString soundPath)
{
    this->upImgPath = upImgPath;
    setSound(soundPath);

    // 初始化按钮，并进行异常捕获
    if(setupButton(this->upImgPath) == -1){
        // 捕获异常后，弹出警告框提示
        QMessageBox::critical(nullptr, "错误", "图片加载失败，程序即将退出");

        // 结束进程，QCoreApplication::exit()
        QCoreApplication::exit(-1); // 退出Qt应用程序
    }
}

ImgButton::ImgButton(QString upImgPath, QString downImgPath, QString soundPath)
{
    this->upImgPath = upImgPath;
    this->downImgPath = downImgPath;
    setSound(soundPath);

    // 初始化按钮，并进行异常捕获
    if(setupButton(this->upImgPath) == -1){
        // 捕获异常后，弹出警告框提示
        QMessageBox::critical(nullptr, "错误", "图片加载失败，程序即将退出");

        // 结束进程，QCoreApplication::exit()
        QCoreApplication::exit(-1); // 退出Qt应用程序
    }
}

// 按钮弹起
void ImgButton::buttonDown()
{
    // 创建动画对象
    QPropertyAnimation *anima = new QPropertyAnimation(this, "geometry"); // 控制对象的矩形几何属性（位置和大小）
    // 设置时间间隔，?ms
    anima->setDuration(150);
    // 创建起始位置
    anima->setStartValue(QRect(this->x(), this->y(), this->width(), this->height()));
    // 创建结束位置
    anima->setEndValue(QRect(this->x(), this->y()+6, this->width(), this->height()));
    // 设置缓和曲线(弹跳效果)
    anima->setEasingCurve(QEasingCurve::OutBounce);
    // 开始执行动画
    anima->start();
}

// 鼠标按下事件
void ImgButton::mousePressEvent(QMouseEvent *e)
{
    // 若按下后图片路径存在，则显示
    if(downImgPath != ""){
        // 设置按钮为按下后样式
        if(setupButton(this->downImgPath) == -1){
            // 捕获异常后，弹出警告框提示
            QMessageBox::critical(nullptr, "错误", "图片加载失败，程序即将退出");

            // 结束进程，QCoreApplication::exit()
            QCoreApplication::exit(-1); // 退出Qt应用程序
        }
    }
    // 播放音效
    this->sound->play();

    // 父类执行按下事件
    QPushButton::mousePressEvent(e);
}

// 按钮弹起
void ImgButton::mouseReleaseEvent(QMouseEvent *e)
{
    // 设置按钮为弹起样式
    if(setupButton(this->upImgPath) == -1){
        // 捕获异常后，弹出警告框提示
        QMessageBox::critical(nullptr, "错误", "图片加载失败，程序即将退出");

        // 结束进程，QCoreApplication::exit()
        QCoreApplication::exit(-1); // 退出Qt应用程序
    }

    // 父类执行弹起事件
    QPushButton::mouseReleaseEvent(e);
}

// 按钮按下
void ImgButton::buttonUp()
{
    // 创建动画对象
    QPropertyAnimation *anima = new QPropertyAnimation(this, "geometry"); // 控制对象的矩形几何属性（位置和大小）
    // 设置时间间隔，?ms
    anima->setDuration(150);
    // 创建起始位置
    anima->setStartValue(QRect(this->x(), this->y()+6, this->width(), this->height()));
    // 创建结束位置
    anima->setEndValue(QRect(this->x(), this->y(), this->width(), this->height()));
    // 设置缓和曲线(弹跳效果)
    anima->setEasingCurve(QEasingCurve::OutBounce);
    // 开始执行动画
    anima->start();
}

// 用于设置按钮
int ImgButton::setupButton(QString imgPath)
{
    QPixmap pix;
    if(!pix.load(imgPath))
    {
        qDebug() << "图片加载失败";
        return -1; // 异常返回
    }

    // 设置按钮大小为图片大小
    this->setFixedSize(pix.width(), pix.height());

    // 设置按钮为不规则样式
    this->setStyleSheet("QPushButton{border:0px;}");

    // 设置图标
    this->setIcon(QIcon(pix));
    this->setIconSize(QSize(pix.width(), pix.height()));

    // 正常设置
    return 0;
}

void ImgButton::setSound(QString soundPath)
{
    this->sound = new QSoundEffect;
    sound->setSource(QUrl::fromLocalFile(soundPath));
    sound->setLoopCount(1); // 循环次数
    sound->setVolume(0.5f); // 音量 0~1之间
}

// 获取upImgPath
QString ImgButton::getUpImgPath()
{
    return this->upImgPath;
}

// 获取downImgPath
QString ImgButton::getDownImgPath()
{
    return this->downImgPath;
}
