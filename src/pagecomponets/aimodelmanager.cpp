#include ".\includes\aimodelmanager.h"
#include <QMessageBox>

AIModelManager* AIModelManager::instance = nullptr;

AIModelManager* AIModelManager::getInstance()
{
    if (!instance) {
        instance = new AIModelManager();
    }
    return instance;
}

AIModelManager::AIModelManager(QObject *parent) : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);
    settings = new QSettings("HMG", "AI_Settings", this);
    
    connect(networkManager, &QNetworkAccessManager::finished, this, [this](QNetworkReply *reply) {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray response = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(response);
            QJsonObject json = doc.object();
            
            // 获取模型返回结果中的"result"字段
            QJsonObject resultObj = json.value("result").toObject();
            QString tool = resultObj.value("tool").toString();
            QString result = resultObj.value("result").toString();
            QString nextAction = resultObj.value("next_action").toString();
            QString suggestion = resultObj.value("suggestion").toString();
            
            // 格式化AI回复
            QString aiResponse = QString("工具: %1<br>结果: %2<br>下一步: %3<br>建议: %4")
                                   .arg(tool.isEmpty() ? "无" : tool,
                                        result.isEmpty() ? "无" : result,
                                        nextAction.isEmpty() ? "无" : nextAction,
                                        suggestion.isEmpty() ? "无" : suggestion);
            
            emit aiResponseReceived(aiResponse);
        } else {
            emit errorOccurred(reply->errorString());
        }
        reply->deleteLater();
    });
    
    // 加载保存的设置
    loadSettings();
}

AIModelManager::~AIModelManager()
{
    saveSettings();
}

void AIModelManager::setModelSettings(const QString& model, const QString& address, const QString& port)
{
    currentModel = model;
    serverAddress = address;
    serverPort = port;
    saveSettings();
}

void AIModelManager::getModelSettings(QString& model, QString& address, QString& port)
{
    model = currentModel;
    address = serverAddress;
    port = serverPort;
}

void AIModelManager::sendMessageToModel(const QString& message)
{
    if (serverAddress.isEmpty() || serverPort.isEmpty()) {
        emit errorOccurred("请先设置模型地址和端口号");
        return;
    }
    
    if (message.isEmpty()) {
        emit errorOccurred("消息内容不能为空");
        return;
    }
    
    // 构建请求URL
    QString urlString = QString("http://%1:%2/api/chat").arg(serverAddress, serverPort);
    QUrl url(urlString);
    
    // 创建JSON请求体
    QJsonObject requestBody;
    requestBody["message"] = message;
    requestBody["model"] = currentModel;
    
    QJsonDocument doc(requestBody);
    QByteArray data = doc.toJson();
    
    // 创建网络请求
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    // 发送POST请求
    networkManager->post(request, data);
}

void AIModelManager::saveSettings()
{
    settings->setValue("model", currentModel);
    settings->setValue("address", serverAddress);
    settings->setValue("port", serverPort);
}

void AIModelManager::loadSettings()
{
    currentModel = settings->value("model", "deepseek-v3").toString();
    serverAddress = settings->value("address", "").toString();
    serverPort = settings->value("port", "").toString();
}