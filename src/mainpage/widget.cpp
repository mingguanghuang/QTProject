// widget.cpp
#include ".\includes\widget.h"
#include ".\includes\ledpage.h"
#include ".\includes\voicecontrolpage.h"
#include ".\includes\voicesettingpage.h"
#include ".\includes\deviceaddpage.h"
#include ".\includes\networksettingpage.h"
#include ".\includes\securitysettingpage.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QGroupBox>
#include <QSpacerItem>
#include <QTextToSpeech>
#include <QStackedWidget>

// 在构造函数中简化样式
Widget::Widget(QWidget *parent)
    : QWidget(parent)

{
    // 设置窗口属性
    this->setWindowTitle("控制面板");
    this->resize(600, 500);
    // 设置窗口透明度（0.0-1.0，1.0为完全不透明）
    this->setWindowOpacity(0.9); // 90%不透明度

    // 构建界面
    setupUI();
    connect_to_signal();
}

void Widget::setupUI()
{
    // 设置背景图片
    this->setStyleSheet(
        "Widget {"
        "  background-position: center;"
        "  background-repeat: no-repeat;"
        "  background-attachment: fixed;"
        "  background-color: #1a1a1a;"
        "}"
        );

    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // 标题 - 添加半透明背景
    title = new QLabel("控制面板");
    title->setStyleSheet(
        "QLabel {"
        "  font-size: 24px;"
        "  font-weight: bold;"
        "  color: #ffffff;"
        "  margin-bottom: 5px;"
        "  border-radius: 10px;"
        "  padding: 5px;"
        "}"
        );
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);

    // 创建堆叠窗口用于页面切换
    stackedWidget = new QStackedWidget(this);

    // 创建各个页面
    ledPage = new LEDPage(this);
    voiceControlPage = new VoiceControlPage(this);
    voiceSettingPage = new VoiceSettingPage(this);
    deviceAddPage = new DeviceAddPage(this);
    networkSettingPage = new NetworkSettingPage(this);
    securitySettingPage = new SecuritySettingPage(this);

    // 添加页面到堆叠窗口
    stackedWidget->addWidget(ledPage);
    stackedWidget->addWidget(voiceControlPage);
    stackedWidget->addWidget(voiceSettingPage);
    stackedWidget->addWidget(deviceAddPage);
    stackedWidget->addWidget(networkSettingPage);
    stackedWidget->addWidget(securitySettingPage);

    // 创建主页面（按钮列表页面）
    QWidget *mainPage = new QWidget(this);
    QVBoxLayout *mainPageLayout = new QVBoxLayout(mainPage);

    // 创建按钮 - 从上到下垂直排列
    ledBtn = new QPushButton("LED 控制");
    voiceControlBtn = new QPushButton("语音控制");
    voiceSettingBtn = new QPushButton("语音设置");
    deviceAddBtn = new QPushButton("设备添加");
    networkSettingBtn = new QPushButton("网络设置");
    securitySettingBtn = new QPushButton("安全设置");

    // 设置按钮样式
    QString buttonStyle =
        "QPushButton {"
        "  height: 50px;"
        "  font-size: 16px;"
        "  border: none;"
        "  color:#ffffff;"
        "  background-color:#2b2b2b;"
        "  border-radius: 5px;"
        "  text-align: left;"
        "  padding-left: 20px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #3b3b3b;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #4b4b4b;"
        "}";

    ledBtn->setStyleSheet(buttonStyle);
    voiceControlBtn->setStyleSheet(buttonStyle);
    voiceSettingBtn->setStyleSheet(buttonStyle);
    deviceAddBtn->setStyleSheet(buttonStyle);
    networkSettingBtn->setStyleSheet(buttonStyle);
    securitySettingBtn->setStyleSheet(buttonStyle);

    // 将按钮添加到主页面布局（垂直排列）
    mainPageLayout->addWidget(ledBtn);
    mainPageLayout->addWidget(voiceControlBtn);
    mainPageLayout->addWidget(voiceSettingBtn);
    mainPageLayout->addWidget(deviceAddBtn);
    mainPageLayout->addWidget(networkSettingBtn);
    mainPageLayout->addWidget(securitySettingBtn);
    mainPageLayout->addStretch();

    // 将主页面添加到堆叠窗口（作为第一个页面）
    stackedWidget->insertWidget(0, mainPage);

    // 设置初始显示主页面（按钮列表）
    stackedWidget->setCurrentIndex(0);

    mainLayout->addWidget(stackedWidget);
    mainLayout->addStretch();

    // 底部按钮 - 添加半透明效果
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    LeftBtn = new QPushButton("召唤");
    RightBtn = new QPushButton("取消");

    // 设置底部按钮样式 - 增大按钮尺寸
    LeftBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: rgba(108, 117, 125, 0.8);"
        "  color: white;"
        "  padding: 12px 24px;"
        "  border-radius: 6px;"
        "  font-size: 16px;"
        "  min-width: 60px;"
        "  min-height: 25px;"
        "}"
        "QPushButton:hover {"
        "  background-color: rgba(168, 117, 125, 0.9);"
        "}"
        );

    RightBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: rgba(108, 117, 125, 0.8);"
        "  color: white;"
        "  padding: 12px 24px;"
        "  border-radius: 6px;"
        "  font-size: 16px;"
        "  width: 60px;"
        "  height: 25px;"
        "}"
        "QPushButton:hover {"
        "  background-color: rgba(168, 117, 125, 0.9);"
        "}"
        );

    // 重新布局：一左一右居中分布
    bottomLayout->addStretch();
    bottomLayout->addWidget(LeftBtn);
    bottomLayout->addSpacing(20);
    bottomLayout->addWidget(RightBtn);
    bottomLayout->addStretch();

    mainLayout->addLayout(bottomLayout);
}

void Widget::connect_to_signal(){
    bool connected_close = connect(RightBtn, &QPushButton::clicked, this, &Widget::close_page);
    qDebug() << "关闭连接:" << connected_close;

    bool connected_Left = connect(LeftBtn, &QPushButton::clicked, this, &Widget::Call);
    qDebug() << "召唤连接:" << connected_Left;

    // 连接页面切换信号
    connect(ledBtn, &QPushButton::clicked, this, &Widget::switchToLEDPage);
    connect(voiceControlBtn, &QPushButton::clicked, this, &Widget::switchToVoiceControlPage);
    connect(voiceSettingBtn, &QPushButton::clicked, this, &Widget::switchToVoiceSettingPage);
    connect(deviceAddBtn, &QPushButton::clicked, this, &Widget::switchToDeviceAddPage);
    connect(networkSettingBtn, &QPushButton::clicked, this, &Widget::switchToNetworkSettingPage);
    connect(securitySettingBtn, &QPushButton::clicked, this, &Widget::switchToSecuritySettingPage);
}

void Widget::close_page(){
    qDebug() << "页面已关闭";
    this->close();
}

void Widget::Call(){
    if(currentPageIndex == 0){
        qDebug() << "正在召唤中";
        QTextToSpeech* speech = new QTextToSpeech();
        speech->say("正在召唤小神龙");
    }else{
        switchToMainPage();
    }
}

// 页面切换函数 - 更新索引号（因为主页面现在是索引0）
void Widget::switchToLEDPage() {
    stackedWidget->setCurrentIndex(1);
    updateUI(1);
    qDebug() << "切换到LED控制页面";
}

void Widget::switchToVoiceControlPage() {
    stackedWidget->setCurrentIndex(2);
    updateUI(2);
    qDebug() << "切换到语音控制页面";
}

void Widget::switchToVoiceSettingPage() {
    stackedWidget->setCurrentIndex(3);
    updateUI(3);
    qDebug() << "切换到语音设置页面";
}

void Widget::switchToDeviceAddPage() {
    stackedWidget->setCurrentIndex(4);
    updateUI(4);
    qDebug() << "切换到设备添加页面";
}

void Widget::switchToNetworkSettingPage() {
    stackedWidget->setCurrentIndex(5);
    updateUI(5);
    qDebug() << "切换到网络设置页面";
}

void Widget::switchToSecuritySettingPage() {
    stackedWidget->setCurrentIndex(6);
    updateUI(6);
    qDebug() << "切换到安全设置页面";
}

// 添加返回主页面的函数
void Widget::switchToMainPage() {
    stackedWidget->setCurrentIndex(0);
    updateUI(0);
    qDebug() << "返回主页面";
}
void Widget::updateUI(int pageIndex){
    currentPageIndex = pageIndex;
    QString titleText;
    QString buttonText;
    if(pageIndex != 0){
        // 子页面
        QStringList pageTitles = {
            "LED控制", "语音控制", "语音设置",
            "设备添加", "网络设置", "安全设置"
        };
        titleText = pageTitles[pageIndex - 1] + "页面";
        buttonText = "返回";
    }else{
        titleText = "控制面板";
        buttonText = "召唤";
    }

    // 更新UI
    title->setText(titleText);
    LeftBtn->setText(buttonText);
}
Widget::~Widget()
{
}
