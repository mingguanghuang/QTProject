#include ".\includes\AISettingPage.h"


AISettingPage::AISettingPage(QWidget *parent) : QWidget(parent)
{
    setupUI();
}

void AISettingPage::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *content = new QLabel("这是AI控制页面\n功能待实现...");
    content->setStyleSheet("font-size: 16px; color: #cccccc;");
    content->setAlignment(Qt::AlignCenter);

    layout->addStretch();

    layout->addWidget(content);
    layout->addStretch();

    this->setStyleSheet("background-color: #1a1a1a;");
}
