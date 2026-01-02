#include ".\includes\mqttclient.h"
#include <QDebug>

MQTTClient::MQTTClient(QObject *parent) : QObject(parent)
{
    m_client = new QMqttClient(this);
    m_reconnectTimer = new QTimer(this);
    m_reconnectTimer->setInterval(5000); // 5秒重连间隔
    
    // 连接信号槽
    connect(m_client, &QMqttClient::connected, this, &MQTTClient::onConnected);
    connect(m_client, &QMqttClient::disconnected, this, &MQTTClient::onDisconnected);
    connect(m_client, &QMqttClient::messageReceived, this, &MQTTClient::onMessageReceived);
    connect(m_client, &QMqttClient::errorChanged, this, &MQTTClient::onError);
    connect(m_reconnectTimer, &QTimer::timeout, this, [this]() {
        if (m_client->state() != QMqttClient::Connected) {
            qDebug() << "尝试重新连接MQTT服务器...";
            m_client->connectToHost();
        }
    });
}

MQTTClient::~MQTTClient()
{
    disconnectFromBroker();
}

bool MQTTClient::connectToBroker(const QString &host, quint16 port)
{
    m_host = host;
    m_port = port;
    
    m_client->setHostname(host);
    m_client->setPort(port);
    
    // 移除客户端ID设置，让MQTT客户端自动生成或使用空客户端ID
    // 这样只需要连接服务器就可以了
    m_client->setClientId("");  // 设置为空字符串，让服务器自动分配客户端ID
    
    m_client->connectToHost();
    m_reconnectTimer->start();
    
    qDebug() << "正在连接MQTT服务器:" << host << ":" << port;
    return true;
}

void MQTTClient::disconnectFromBroker()
{
    m_reconnectTimer->stop();
    if (m_client->state() == QMqttClient::Connected) {
        m_client->disconnectFromHost();
    }
}

void MQTTClient::publishMessage(const QString &topic, const QJsonObject &message)
{
    if (m_client->state() != QMqttClient::Connected) {
        emit errorOccurred("MQTT客户端未连接");
        return;
    }
    
    QJsonDocument doc(message);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    
    QMqttTopicName topicName(topic);
    m_client->publish(topicName, data);
    
    qDebug() << "发布消息到主题:" << topic << "内容:" << data;
}

void MQTTClient::subscribeToTopic(const QString &topic)
{
    // 直接订阅主题，不检查连接状态
    // MQTT客户端会在连接建立后自动完成订阅
    QMqttSubscription *subscription = m_client->subscribe(topic);
    if (subscription) {
        qDebug() << "订阅主题:" << topic;
        // 连接订阅成功信号
        connect(subscription, &QMqttSubscription::stateChanged, this, [this, topic](QMqttSubscription::SubscriptionState state) {
            switch (state) {
            case QMqttSubscription::Unsubscribed:
                qDebug() << "主题已取消订阅:" << topic;
                break;
            case QMqttSubscription::SubscriptionPending:
                qDebug() << "主题订阅中:" << topic;
                break;
            case QMqttSubscription::Subscribed:
                qDebug() << "主题订阅成功:" << topic;
                break;
            case QMqttSubscription::Error:
                qDebug() << "主题订阅失败:" << topic;
                break;
            default:
                break;
            }
        });
    } else {
        qDebug() << "订阅主题失败:" << topic;
    }
}

void MQTTClient::unsubscribeFromTopic(const QString &topic)
{
    // 直接取消订阅，不检查连接状态
    m_client->unsubscribe(topic);
    qDebug() << "取消订阅主题:" << topic;
}

bool MQTTClient::isConnected() const
{
    return m_client->state() == QMqttClient::Connected;
}

void MQTTClient::onConnected()
{
    qDebug() << "MQTT连接成功 - 服务器:" << m_host << ":" << m_port;
    m_reconnectTimer->stop();
    emit connectionStatusChanged(true);
}

void MQTTClient::onDisconnected()
{
    qDebug() << "MQTT连接断开";
    m_reconnectTimer->start();
    emit connectionStatusChanged(false);
}

void MQTTClient::onMessageReceived(const QByteArray &message, const QMqttTopicName &topic)
{
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(message, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "JSON解析错误:" << parseError.errorString();
        return;
    }
    
    if (doc.isObject()) {
        QJsonObject jsonObj = doc.object();
        qDebug() << "收到消息，主题:" << topic.name() << "内容:" << jsonObj;
        emit messageReceived(topic.name(), jsonObj);
    }
}

void MQTTClient::onError(QMqttClient::ClientError error)
{
    QString errorMsg;
    switch (error) {
    case QMqttClient::InvalidProtocolVersion:
        errorMsg = "无效的协议版本";
        break;
    case QMqttClient::IdRejected:
        errorMsg = "客户端ID被拒绝";
        break;
    case QMqttClient::ServerUnavailable:
        errorMsg = "服务器不可用";
        break;
    case QMqttClient::BadUsernameOrPassword:
        errorMsg = "用户名或密码错误";
        break;
    case QMqttClient::NotAuthorized:
        errorMsg = "未授权";
        break;
    default:
        errorMsg = "未知错误";
        break;
    }
    
    qDebug() << "MQTT错误:" << errorMsg;
    emit errorOccurred(errorMsg);
}