#include ".\includes\AISettingPage.h"
#include <QHBoxLayout>
#include <QGroupBox>
#include <QScrollBar>

AISettingPage::AISettingPage(QWidget *parent) : QWidget(parent)
{
    networkManager = new QNetworkAccessManager(this);
    connect(networkManager, &QNetworkAccessManager::finished, this, &AISettingPage::onReplyFinished);
    setupUI();
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

    // 设置样式
    this->setStyleSheet("QGroupBox { color: #cccccc; font-weight: bold; }"
                       "QLabel { color: #cccccc; }"
                       "QLineEdit, QComboBox { background-color: #2d2d2d; color: #ffffff; border: 1px solid #555; padding: 5px; }"
                       "QPushButton { background-color: #007acc; color: white; border: none; padding: 8px 16px; }"
                       "QPushButton:hover { background-color: #005a9e; }"
                       "QPushButton:pressed { background-color: #004578; }"
                       "background-color: #1a1a1a;");
}

void AISettingPage::onSendMessage()
{
    QString message = messageLineEdit->text().trimmed();
    if (message.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入消息内容");
        return;
    }

    QString address = addressLineEdit->text().trimmed();
    QString port = portLineEdit->text().trimmed();
    
    if (address.isEmpty() || port.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先设置模型地址和端口号");
        return;
    }

    // 添加用户消息到聊天框
    chatTextEdit->append("<div style='color: #4CAF50; margin: 5px 0;'><b>用户:</b> " + message + "</div>");
    messageLineEdit->clear();

    // 发送请求到模型
    sendRequestToModel(message);
}

void AISettingPage::sendRequestToModel(const QString &message)
{
    QString address = addressLineEdit->text().trimmed();
    QString port = portLineEdit->text().trimmed();
    
    // 构建请求URL
    QString urlString = QString("http://%1:%2/api/chat").arg(address,port);
    QUrl url(urlString);
    
    // 创建JSON请求体
    QJsonObject requestBody;
    requestBody["message"] = message;
    requestBody["model"] = modelComboBox->currentText();
    
    QJsonDocument doc(requestBody);
    QByteArray data = doc.toJson();
    
    // 创建网络请求
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    // 发送POST请求
    networkManager->post(request, data);
    
    // 显示等待提示
    chatTextEdit->append("<div style='color: #FFA500; margin: 5px 0;'><b>系统:</b> 正在请求AI模型...</div>");
}

void AISettingPage::onReplyFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(response);
        QJsonObject json = doc.object();
        //获取模型返回结果中的"result"字段
        QJsonObject resultObj = json.value("result").toObject();
        // 解析JSON响应
        QString tool = resultObj.value("tool").toString();
        QString result = resultObj.value("result").toString();
        QString nextAction = resultObj.value("next_action").toString();
        QString suggestion = resultObj.value("suggestion").toString();
        
        // 格式化显示AI回复 - 使用多参数arg()方法
        QString aiResponse = QString("<div style='color: #2196F3; margin: 5px 0;'>"
                                   "<b>AI回复:</b><br>"
                                   "<b>工具:</b> %1<br>"
                                   "<b>结果:</b> %2<br>"
                                   "<b>下一步:</b> %3<br>"
                                   "<b>建议:</b> %4</div>")
                                   .arg(tool.isEmpty() ? "无" : tool,
                                        result.isEmpty() ? "无" : result,
                                        nextAction.isEmpty() ? "无" : nextAction,
                                        suggestion.isEmpty() ? "无" : suggestion);
        
        chatTextEdit->append(aiResponse);
    } else {
        QString errorMsg = QString("<div style='color: #f44336; margin: 5px 0;'><b>错误:</b> %1</div>")
                          .arg(reply->errorString());
        chatTextEdit->append(errorMsg);
    }
    
    reply->deleteLater();
    
    // 自动滚动到底部
    QScrollBar *scrollBar = chatTextEdit->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}
