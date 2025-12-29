#include "securitysettingpage.h"


SecuritySettingPage::SecuritySettingPage(QWidget *parent) : QWidget(parent)
{
    setupUI();
}

void SecuritySettingPage::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    

    
    QLabel *content = new QLabel("这是安全设置页面\n功能待实现...");
    content->setStyleSheet("font-size: 16px; color: #cccccc;");
    content->setAlignment(Qt::AlignCenter);
    
    layout->addStretch();

    layout->addWidget(content);
    layout->addStretch();
    
    this->setStyleSheet("background-color: #2a2a2a;");
}
