#include "mapeditwindow.h"
#include "qlabel.h"
#include "qlineedit.h"
#include "qpainter.h"
#include "ui_mapeditwindow.h"
#include "imgbutton.h"
#include "mazegenerator.h"
#include "QIntValidator"

MapEditWindow::MapEditWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MapEditWindow)
{
    ui->setupUi(this);
    //初始化tiles数组
    for(int i = 0 ; i < 31;i++)
    {
        for(int j = 0 ; j < 31; j++)
        {
            tiles[i][j]=NULL;
        }
    }
    // 返回按钮
    ImgButton *backBtn = new ImgButton(":/res/backBtn_1.png",
                                       ":/res/backBtn_2.png",
                                       ":/res/ButtonSound.wav");
    backBtn->setParent(this);
    backBtn->move(630, this->height() * 0.8);
    backBtn->show();

    connect(backBtn, &ImgButton::clicked, this, [=](){
        emit backBtnClicked();
    });

    // 随机生成按钮
    ImgButton *randomBtn = new ImgButton(":/res/randomBtn.png", ":/res/ButtonSound.wav");
    randomBtn->setParent(this);
    randomBtn->move(640, this->height()*0.56);
    randomBtn->show();

    connect(randomBtn, &ImgButton::clicked, this, [=](){
        QVector<QVector<int>> mp = createMaze();
        /*
         * 调试地图随机生成
        for(QVector<int> v : mp)
            qDebug() << v;
        */
        for(int i = 0 ; i < tempSize;i++)
            for(int j = 0 ; j < tempSize; j++)
                tiles[i][j]->changeStatus(static_cast<TileStatus>(mp[i][j]));
    });

    // 创建 QLabel 和 QLineEdit 控件
    QLabel *tempSizeLabel = new QLabel("地图尺寸：");
    QLineEdit *tempSizeLE = new QLineEdit();

    // 设置 QLabel 属性
    tempSizeLabel->setAlignment(Qt::AlignRight);  // 设置文本右对齐
    tempSizeLabel->setStyleSheet("font-size: 14px; color: black;");  // 设置字体大小和颜色

    // 设置 QLineEdit 属性
    tempSizeLE->setPlaceholderText("请输入地图尺寸");  // 设置占位符文本
    tempSizeLE->setMaxLength(5);  // 设置最大字符数
    tempSizeLE->setValidator(new QIntValidator(1, 31, this));  // 设置输入验证器，限制输入的值范围为1到31
    tempSizeLE->setStyleSheet("font-size: 14px; color: black;");  // 设置字体大小和颜色

    // "-"号按钮
    ImgButton *minusBtn = new ImgButton(":/res/minusBtn_1.png",
                                        ":/res/minusBtn_2.png",
                                        ":/res/ButtonSound.wav");
    minusBtn->setParent(this);
    minusBtn->move(610, this->height() * 0.25);
    minusBtn->show();

    // 加号按钮
    ImgButton *plusBtn = new ImgButton(":/res/plusBtn_1.png",
                                       ":/res/plusBtn_2.png",
                                       ":/res/ButtonSound.wav");
    plusBtn->setParent(this);
    plusBtn->move(750, this->height() * 0.25);
    plusBtn->show();

    // 连接 "-" 按钮的点击事件
    connect(minusBtn, &ImgButton::clicked, this, [=]() {
        // 减小 tempSize，确保是奇数并循环在 1 到 31 之间
        tempSize -= 2;
        if (tempSize < 3) {
            tempSize = 29;  // 循环回 29
        }

        // 更新显示（可以根据需要更新界面或相关控件）
        qDebug() << "tempSize after decrease: " << tempSize;

        createTileTexture();
    });

    // 连接 "+" 按钮的点击事件
    connect(plusBtn, &ImgButton::clicked, this, [=]() {
        // 增大 tempSize，确保是奇数并循环在 1 到 31 之间
        tempSize += 2;
        if (tempSize > 29) {
            tempSize = 3;  // 循环回 3
        }

        // 更新显示（可以根据需要更新界面或相关控件）
        qDebug() << "tempSize after increase: " << tempSize;

        createTileTexture();
    });

    createTileTexture();
}

MapEditWindow::~MapEditWindow()
{
    delete ui;
}

// 背景图
void MapEditWindow::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, 600, 600, QPixmap(":/res/road.png"));
    painter.drawPixmap(600, 0, 200, 600, QPixmap(":/res/statusBackground.png"));
}

// 根据贴图绘制地图
void MapEditWindow::createTileTexture()
{
    for (int i = 0; i < 31; i++) {
        for (int j = 0; j < 31; j++) {
            if (tiles[i][j] != nullptr) {
                delete tiles[i][j];  // 删除之前的 TileTexture 对象
                tiles[i][j] = nullptr;  // 置空
            }
        }
    }
    for(int i = 0 ; i < tempSize;i++)
    {
        for(int j = 0 ; j < tempSize; j++)
        {
            //创建金币对象
            TileTexture * tile = new TileTexture(WALL);
            tile->setParent(this);
            tile->move(i*(600/tempSize), j*(600/tempSize));
            tile->setSize(600/tempSize);
            tiles[i][j] = tile;
            tile->show();
            connect(tile, &TileTexture::clicked, [=](){
                tile->changeStatus();
            });
        }
    }
}

// 随机生成地图
QVector<QVector<int> > MapEditWindow::createMaze()
{
    QVector<QVector<int>> mapEdit;
    MazeGenerator *mazeGenerator = new MazeGenerator(this->tempSize);
    mapEdit = mazeGenerator->getMazeMap();
    return mapEdit;
}
