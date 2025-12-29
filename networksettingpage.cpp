#include "networksettingpage.h"


NetworkSettingPage::NetworkSettingPage(QWidget *parent) : QWidget(parent)
{
    setupUI();
}

void NetworkSettingPage::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    
    QLabel *content = new QLabel("这是网络设置页面\n功能待实现...");
    content->setStyleSheet("font-size: 16px; color: #cccccc;");
    content->setAlignment(Qt::AlignCenter);
    
    layout->addStretch();

    layout->addWidget(content);
    layout->addStretch();
    
    this->setStyleSheet("background-color: #2a2a2a;");
}
