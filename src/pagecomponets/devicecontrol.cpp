#include ".\includes\devicecontrol.h"
#include <QTextToSpeech>
#include <QPixmap>
#include <QHBoxLayout>
#include <QPushButton>

ControlPage::ControlPage(QWidget *parent) : QWidget(parent)
{
    setupUI();
}

void ControlPage::setupUI()
{
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 创建水平布局用于居中按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    // 创建图片按钮
    QPushButton *imageButton = new QPushButton(this);
    imageButton->setFixedSize(220, 220); // 设置固定大小
    imageButton->setStyleSheet("QPushButton { border: none; "
                               "background-color: transparent; "
                               "}"
                               "QPushButton:hover {"
                               "  background-color: #2a2a2a;"
                               "}"
                               "QPushButton:pressed {"
                               "  background-color: #3b3b3b;"
                               "}");
    
    // 设置初始状态为off，显示dark.png
    QPixmap darkPixmap(":/pre1/assets/dark.png");
    if (!darkPixmap.isNull()) {
        imageButton->setIcon(darkPixmap.scaled(180, 180, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        imageButton->setIconSize(QSize(200, 200));
    }
    
    // 连接点击信号到槽函数
    connect(imageButton, &QPushButton::clicked, this, &ControlPage::LEDController);
    
    // 将按钮添加到水平布局并居中
    buttonLayout->addStretch();
    buttonLayout->addWidget(imageButton);
    buttonLayout->addStretch();
    
    // 将水平布局添加到主布局
    mainLayout->addLayout(buttonLayout);
}

void ControlPage::LEDController()
{   // 获取发送信号的按钮
    QPushButton *button = qobject_cast<QPushButton*>(sender());

    QTextToSpeech* speech = new QTextToSpeech();
    if(is_open == false){
        // 当前为off状态，切换为on状态
        button->setStyleSheet("QPushButton { border: none; "
                              "background-color: yellow; "
                              "}"
                              "QPushButton:hover {"
                              "  background-color: rgb(250,250,210);"
                              "}"
                              "QPushButton:pressed {"
                              "  background-color: #3b3b3b;"
                              "}");
        QPixmap lightPixmap(":/pre1/assets/light.png");
        if (!lightPixmap.isNull()) {
            button->setIcon(lightPixmap.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        is_open = true;
        qDebug()<<"灯已打开";
        speech->say("灯已打开");
    }else{
        // 当前为on状态，切换为off状态
        button->setStyleSheet("QPushButton { border: none; "
                              "background-color: transparent; "
                              "}"
                              "QPushButton:hover {"
                              "  background-color: #2a2a2a;"
                              "}"
                              "QPushButton:pressed {"
                              "  background-color: #3b3b3b;"
                              "}"
                              );
        QPixmap darkPixmap(":/pre1/assets/dark.png");
        if (!darkPixmap.isNull()) {
            button->setIcon(darkPixmap.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        is_open = false;
        qDebug()<<"灯已关闭";
        speech->say("灯已关闭");
    }
}


