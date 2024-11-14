#include "tiletexture.h"

TileTexture::TileTexture(TileStatus status)
{
    this->status = status;
    this->statusToPath[WALL] = QString(":/res/wall.png");
    this->statusToPath[PATH] = QString(":/res/road.png");
    this->statusToPath[STARTING] = QString(":/res/start.png");
    this->statusToPath[ENDING] = QString(":/res/end.png");
    this->statusToPath[TASK] = QString(":/res/destination.png");

    // 加载贴图
    QPixmap pix;
    if(!pix.load(statusToPath[status]))
    {
        qDebug() << "图片加载失败";
        return;
    }

    //设置按钮大小为图片大小
    setFixedSize(40, 40);
    //设置按钮为不规则样式
    setStyleSheet("QPushButton{border:0px;}");

    //设置图标
    setIcon(QIcon(pix));
    setIconSize(QSize(40, 40));
}

// 设置大小
void TileTexture::setSize(int Size)
{
    setFixedSize(Size, Size);
    setIconSize(QSize(Size, Size));
}

void TileTexture::changeStatus()
{
    status = static_cast<TileStatus>((static_cast<int>(status) + 1) % TILE_STATUS_COUNT);
    //设置图标
    QPixmap pix;
    pix.load(statusToPath[status]);
    setIcon(QIcon(pix));
}

void TileTexture::changeStatus(TileStatus status)
{
    this->status = status;
    //设置图标
    QPixmap pix;
    pix.load(statusToPath[status]);
    setIcon(QIcon(pix));
}
