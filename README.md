# ui-hmg项目的使用说明

## includes

- 包含项目运行所需的头文件：
- widget：主窗口布局
- deviceaddpage：设备添加功能
- ledpage：led灯控制
- networksettingpage：网络功能设置
- voicecontrolpage：语音控制系统
- voicesettingpage：语音设置（语音合成，语音输入）

## src

- mainpage文件夹：主窗口页面实现

- pagecomponets：子页面实现（注意：子页面是基于主窗口的基础进行页面更改的，即窗口大小和标题设置，以及最下方的按钮这三者的样式是直接从widget中继承过来的），目前子页面都只做了个框架，具体功能实现需要对照开发板进行修改

- main.cpp：程序主文件（启动文件）

- widget：

  ```
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
  ```

  这是主窗口，setupUI()：页面构建， connect_to_signal()：信号槽连接（这里我把所有的信号槽连接功能统一放在一个函数中方便后续进行管理）

- ```
  // 创建堆叠窗口用于页面切换
      stackedWidget = new QStackedWidget(this);
  ```

  使用stackWidget进行窗口堆叠，类似与将页面压入堆栈中

- ```
  void Widget::Call(){
      if(currentPageIndex == 0){
          qDebug() << "正在召唤中";
          QTextToSpeech* speech = new QTextToSpeech();
          speech->say("正在召唤小神龙");
      }else{
          switchToMainPage();
      }
  }
  ```

  Call函数是左边的按钮，他的功能和文字描述会根据页面的切换而切换

- ```
  // 页面切换函数 - 更新索引号（因为主页面现在是索引0）
  void Widget::switchToLEDPage() {
      stackedWidget->setCurrentIndex(1);
      updateUI(1);
      qDebug() << "切换到LED控制页面";
  }
  ```

  该函数连接在主页面呈现的那几个列表中，当触发点击效果时边进行页面切换（根据索引跳转到对应的页面中）