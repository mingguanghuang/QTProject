#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QDebug>
#include <QStackedWidget>
#include <QLabel>

// 前向声明页面类
class LEDPage;
class VoiceControlPage;
class VoiceSettingPage;
class DeviceAddPage;
class NetworkSettingPage;
class SecuritySettingPage;

class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    // 初始化界面
    void setupUI();
    void connect_to_signal();
    void updateUI(int pageIndex);

    QPushButton *LeftBtn;
    QPushButton *RightBtn;
    QLabel *title;
    
    // 页面按钮
    QPushButton *ledBtn;
    QPushButton *voiceControlBtn;
    QPushButton *voiceSettingBtn;
    QPushButton *deviceAddBtn;
    QPushButton *networkSettingBtn;
    QPushButton *securitySettingBtn;
    
    // 页面容器
    QStackedWidget *stackedWidget;
    
    // 各个页面
    LEDPage *ledPage;
    VoiceControlPage *voiceControlPage;
    VoiceSettingPage *voiceSettingPage;
    DeviceAddPage *deviceAddPage;
    NetworkSettingPage *networkSettingPage;
    SecuritySettingPage *securitySettingPage;

private slots:
    void close_page(void);
    void Call(void);
    void switchToLEDPage(void);
    void switchToVoiceControlPage(void);
    void switchToVoiceSettingPage(void);
    void switchToDeviceAddPage(void);
    void switchToNetworkSettingPage(void);
    void switchToSecuritySettingPage(void);
    void switchToMainPage(void);

};
#endif // WIDGET_H
