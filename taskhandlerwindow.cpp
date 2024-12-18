#include "taskhandlerwindow.h"
#include "qboxlayout.h"
#include "qlabel.h"
#include "qlineedit.h"
#include "qpushbutton.h"
#include "qvalidator.h"
#include "qdebug.h"
#include "QTimer.h"

TaskHandlerWindow::TaskHandlerWindow(QWidget *parent)
    : QWidget(parent)
{
    // 创建 QVBoxLayout 布局
    QVBoxLayout *layout = new QVBoxLayout(this);

    // 创建并设置问题显示的 QLabel
    questionLabel = new QLabel("Question: 0 + 0 = ?", this);
    layout->addWidget(questionLabel);

    // 创建并设置用户输入答案的 QLineEdit
    answerInput = new QLineEdit(this);
    answerInput->setValidator(new QIntValidator(this));  // 设置只能输入整数
    layout->addWidget(answerInput);

    // 创建并设置提交按钮
    submitButton = new QPushButton("Submit", this);
    layout->addWidget(submitButton);

    // 创建并设置结果显示的 QLabel
    resultLabel = new QLabel("Result: ", this);
    layout->addWidget(resultLabel);

    // 连接按钮点击事件到槽函数
    connect(submitButton, &QPushButton::clicked, this, &TaskHandlerWindow::onSubmitButtonClicked);

    // 生成并显示一道题目
    generateNewTask();
}

TaskHandlerWindow::~TaskHandlerWindow()
{
}

void TaskHandlerWindow::generateNewTask()
{
    // 生成随机的数学题目
    QString question;
    correctAnswer = generateMathQuestion(question);
    questionLabel->setText("Question: " + question + " = ?");
    answerInput->clear();
    resultLabel->setText("Result: ");
    resultLabel->repaint();  // 强制刷新 UI
}

void TaskHandlerWindow::onSubmitButtonClicked()
{
    // 获取用户输入的答案
    bool ok;
    int userAnswer = answerInput->text().toInt(&ok);

    if (!ok) {
        resultLabel->setText("Result: Invalid input! Please enter a number.");
        resultLabel->repaint();
        return;
    }

    // 验证答案
    if (checkAnswer(userAnswer, correctAnswer)) {
        resultLabel->setText("Result: Correct!");
        score += 100;
        resultLabel->repaint();
        // 启动一个定时器，在1秒后抛出信号
        // QTimer::singleShot(1000, this, &TaskHandlerWindow::generateNewTask);
        emit correct();
    } else {
        resultLabel->setText("Result: Incorrect! Try again.");
        resultLabel->repaint();

        // 启动一个定时器，在1秒后重新生成题目
        QTimer::singleShot(1000, this, &TaskHandlerWindow::generateNewTask);
    }
}

int TaskHandlerWindow::generateMathQuestion(QString& question)
{
    // 随机生成两个数和运算符
    QRandomGenerator *generator = QRandomGenerator::global();

    num1 = generator->bounded(1, 10); // 随机生成 1 到 10 的数
    num2 = generator->bounded(1, 10); // 随机生成 1 到 10 的数
    int operationType = generator->bounded(4); // 0:加 1:减 2:乘 3:除

    switch (operationType) {
    case 0:
        operation = "+";
        correctAnswer = num1 + num2;
        break;
    case 1:
        operation = "-";
        correctAnswer = num1 - num2;
        break;
    case 2:
        operation = "*";
        correctAnswer = num1 * num2;
        break;
    case 3:
        operation = "/";
        // 确保除法结果是整数：num1 必须是 num2 的倍数
        if (num2 == 0 || num1 % num2 != 0) {
            num1 = num2 * (generator->bounded(1, 10));  // 确保被除数是除数的倍数
        }
        correctAnswer = num1 / num2;
        break;
    }

    question = QString::number(num1) + " " + operation + " " + QString::number(num2);
    return correctAnswer;
}

bool TaskHandlerWindow::checkAnswer(int userAnswer, int correctAnswer)
{
    return userAnswer == correctAnswer;
}

int TaskHandlerWindow::getScore(){
    return this->score;
}
