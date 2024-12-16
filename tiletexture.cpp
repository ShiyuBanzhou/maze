#include "tiletexture.h"
#include "qrandom.h"

TileTexture::TileTexture(TileStatus status)
{
    QRandomGenerator *generator = QRandomGenerator::global();
    this->status = status;
    this->statusToPath[WALL] = QString(":/res/wall.png");
    this->statusToPath[PATH] = QString(":/res/road.png");
    this->statusToPath[STARTING] = QString(":/res/start.png");
    this->statusToPath[ENDING] = QString(":/res/end.png");
    this->statusToPath[TASK] = QString(":/res/destination.png");
    this->statusToPath[STAR] = QString(":/res/star") + QString::number(generator->bounded(1, 3)) + QString(".png");
    this->statusToPath[LASTLAYER] = QString(":/res/lastLayer.png");
    this->statusToPath[ROAD] = QString(":/res/path.png");
    this->statusToPath[RETURN] = QString(":/res/return.png");

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

void TileTexture::setFog(bool fogged) {
    isFogged = fogged;

    if (isFogged) {
        // 模糊效果
        QGraphicsBlurEffect *blurEffect = new QGraphicsBlurEffect(this);
        blurEffect->setBlurRadius(8); // 设置模糊半径
        this->setGraphicsEffect(blurEffect);

        // 添加半透明覆盖
        this->setStyleSheet("background-color: rgba(0, 0, 0, 0.7);");
    } else {
        // 清晰显示
        this->setGraphicsEffect(nullptr);
        this->setStyleSheet(""); // 清除样式
    }
}
