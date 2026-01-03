#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QDebug>
#include <QStackedWidget>
#include <QLabel>
#include ".\includes\devicecontrol.h"
#include ".\includes\voicecontrolpage.h"
#include ".\includes\voicesettingpage.h"
#include ".\includes\deviceaddpage.h"
#include ".\includes\networksettingpage.h"
#include ".\includes\securitysettingpage.h"
#include ".\includes\AISettingPage.h"
// 前向声明页面类
class ControlPage;
class VoiceControlPage;
class VoiceSettingPage;
class DeviceAddPage;
class NetworkSettingPage;
class SecuritySettingPage;
class AISettingPage;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    // 添加公共访问方法
    ControlPage* getControlPage() { return controlPage; }

private:
    // 初始化界面
    int currentPageIndex = 0;
    void setupUI();
    void connect_to_signal();
    void updateUI(int pageIndex);

    QPushButton *LeftBtn;
    QPushButton *RightBtn;
    QLabel *title;

    // 页面按钮
    QPushButton *ControlBtn;
    QPushButton *voiceControlBtn;
    QPushButton *voiceSettingBtn;
    QPushButton *deviceAddBtn;
    QPushButton *networkSettingBtn;
    QPushButton *securitySettingBtn;
    QPushButton *aiSettingBtn;

    // 页面容器
    QStackedWidget *stackedWidget;
    
    // 各个页面
    ControlPage *controlPage;
    VoiceControlPage *voiceControlPage;
    VoiceSettingPage *voiceSettingPage;
    DeviceAddPage *deviceAddPage;
    NetworkSettingPage *networkSettingPage;
    SecuritySettingPage *securitySettingPage;
    AISettingPage *aiSettingPage;

private slots:
    void close_page(void);
    void Call(void);
    void switchToControlPage(void);
    void switchToVoiceControlPage(void);
    void switchToVoiceSettingPage(void);
    void switchToDeviceAddPage(void);
    void switchToNetworkSettingPage(void);
    void switchToSecuritySettingPage(void);
    void switchToAISettingPage(void);
    void switchToMainPage(void);

};
#endif // WIDGET_H