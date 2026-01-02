#include ".\includes\AISettingPage.h"
#include <QHBoxLayout>
#include <QGroupBox>
#include <QScrollBar>

AISettingPage::AISettingPage(QWidget *parent) : QWidget(parent)
{
    aiManager = AIModelManager::getInstance();
    connect(aiManager, &AIModelManager::aiResponseReceived, this, &AISettingPage::onAiResponseReceived);
    connect(aiManager, &AIModelManager::errorOccurred, this, &AISettingPage::onErrorOccurred);
    
    setupUI();
    
    // 加载保存的设置
    QString model, address, port;
    aiManager->getModelSettings(model, address, port);
    modelComboBox->setCurrentText(model);
    addressLineEdit->setText(address);
    portLineEdit->setText(port);
}

void AISettingPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 模型设置组
    QGroupBox *modelGroup = new QGroupBox("AI模型设置");
    QVBoxLayout *modelLayout = new QVBoxLayout(modelGroup);

    // 模型选择
    QHBoxLayout *modelSelectLayout = new QHBoxLayout();
    QLabel *modelLabel = new QLabel("选择模型:");
    modelComboBox = new QComboBox();
    modelComboBox->addItems({"deepseek-v3", "deepseek-v3.2", "qwen-plus", "deepseek-r1","本地模型"});
    modelSelectLayout->addWidget(modelLabel);
    modelSelectLayout->addWidget(modelComboBox);
    modelSelectLayout->addStretch();

    // 地址和端口设置
    QHBoxLayout *addressLayout = new QHBoxLayout();
    QLabel *addressLabel = new QLabel("模型地址:");
    addressLineEdit = new QLineEdit();
    addressLineEdit->setPlaceholderText("例如: 127.0.0.1 或 api.openai.com");
    
    QLabel *portLabel = new QLabel("端口号:");
    portLineEdit = new QLineEdit();
    portLineEdit->setPlaceholderText("例如: 8080");
    portLineEdit->setFixedWidth(80);

    addressLayout->addWidget(addressLabel);
    addressLayout->addWidget(addressLineEdit);
    addressLayout->addWidget(portLabel);
    addressLayout->addWidget(portLineEdit);

    modelLayout->addLayout(modelSelectLayout);
    modelLayout->addLayout(addressLayout);

    // 聊天对话框组
    QGroupBox *chatGroup = new QGroupBox("AI对话");
    QVBoxLayout *chatLayout = new QVBoxLayout(chatGroup);

    chatTextEdit = new QTextEdit();
    chatTextEdit->setReadOnly(true);
    chatTextEdit->setMinimumHeight(300);
    chatTextEdit->setStyleSheet("background-color: #2d2d2d; color: #ffffff; border: 1px solid #555;");

    // 消息输入区域
    QHBoxLayout *messageLayout = new QHBoxLayout();
    messageLineEdit = new QLineEdit();
    messageLineEdit->setPlaceholderText("输入您的问题...");
    sendButton = new QPushButton("发送");
    sendButton->setFixedWidth(80);

    messageLayout->addWidget(messageLineEdit);
    messageLayout->addWidget(sendButton);

    chatLayout->addWidget(chatTextEdit);
    chatLayout->addLayout(messageLayout);

    // 添加到主布局
    mainLayout->addWidget(modelGroup);
    mainLayout->addWidget(chatGroup);

    // 连接信号槽
    connect(sendButton, &QPushButton::clicked, this, &AISettingPage::onSendMessage);
    connect(messageLineEdit, &QLineEdit::returnPressed, this, &AISettingPage::onSendMessage);
    
    // 当设置改变时更新模型设置
    connect(modelComboBox, &QComboBox::currentTextChanged, this, &AISettingPage::updateModelSettings);
    connect(addressLineEdit, &QLineEdit::textChanged, this, &AISettingPage::updateModelSettings);
    connect(portLineEdit, &QLineEdit::textChanged, this, &AISettingPage::updateModelSettings);

    // 设置样式
    this->setStyleSheet("QGroupBox { color: #cccccc; font-weight: bold; }"
                       "QLabel { color: #cccccc; }"
                       "QLineEdit, QComboBox { background-color: #2d2d2d; color: #ffffff; border: 1px solid #555; padding: 5px; }"
                       "QPushButton { background-color: #007acc; color: white; border: none; padding: 8px 16px; }"
                       "QPushButton:hover { background-color: #005a9e; }"
                       "QPushButton:pressed { background-color: #004578; }"
                       "background-color: #1a1a1a;");
}

void AISettingPage::updateModelSettings()
{
    QString model = modelComboBox->currentText();
    QString address = addressLineEdit->text().trimmed();
    QString port = portLineEdit->text().trimmed();
    aiManager->setModelSettings(model, address, port);
}

void AISettingPage::onSendMessage()
{
    QString message = messageLineEdit->text().trimmed();
    if (message.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入消息内容");
        return;
    }

    // 添加用户消息到聊天框
    chatTextEdit->append("<div style='color: #4CAF50; margin: 5px 0;'><b>用户:</b> " + message + "</div>");
    messageLineEdit->clear();

    // 显示等待提示
    chatTextEdit->append("<div style='color: #FFA500; margin: 5px 0;'><b>系统:</b> 正在请求AI模型...</div>");

    // 发送请求到模型
    aiManager->sendMessageToModel(message);
    
    // 自动滚动到底部
    QScrollBar *scrollBar = chatTextEdit->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void AISettingPage::onAiResponseReceived(const QString& response)
{
    QString aiResponse = QString("<div style='color: #2196F3; margin: 5px 0;'>"
                               "<b>AI回复:</b><br>%1</div>").arg(response);
    
    chatTextEdit->append(aiResponse);
    
    // 自动滚动到底部
    QScrollBar *scrollBar = chatTextEdit->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void AISettingPage::onErrorOccurred(const QString& error)
{
    QString errorMsg = QString("<div style='color: #f44336; margin: 5px 0;'><b>错误:</b> %1</div>")
                      .arg(error);
    chatTextEdit->append(errorMsg);
    
    // 自动滚动到底部
    QScrollBar *scrollBar = chatTextEdit->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}
