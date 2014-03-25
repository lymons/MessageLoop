MessageLoop
===========



##功能要求
1. 用于多线程间进行单向的消息传递，可以做到有多个发送线程向一个接收线程发送消息。
2. 该模块能让消息的接收者，发送者（线程）能够简单方便地使用
3. 接收者能够及时地收到消息，并立即对该消息做出反应（不需要把反应结果告诉发送者）
4. 该模块不能过多地占用cpu资源
5. 该模块能在Win，Unix/Linux平台下编译使用
6. 线程安全
7. 接收线程里可以有多个接收者

##消息的内容应该有如下的项目
1. 消息识别码，它是一种接收者和发送者都共识的代码，发送者通过这个代码可以让接收者明白它的意图
2. 欲发送的消息，包含下面两种消息：

* 字符串
* 自定义的数据结构 基本上接收者通过该消息的识别码可以明白该数据结构的数据类型


##实现原理

使用socket和I/O多路复用select来实现，因为socket是跨平台，且在等待消息的时候不占用cpu资源

1. 用select的fdset来模拟一个消息队列
2. 接收者创建一个服务端socket
3. 把连接到这个服务端socket的发送端socket放入到fdset中
4. 接收者可以使用select来监控这个fdset
5. fdset中一旦有可读项，则找出对应的发送端socket
6. 根据这个发送端的socket中找到相对应的Message
7. 让接收者处理这个Message。

##基于上述的原理，可以抽出如下的class：
1. `接收者类 Handler`
    因为在接收线程里可以有多个接收者，所以必须把接收者定义成一个类，
    该类的功能就是对收到的消息进行反应
2. `消息类 Message`
    存储消息内容，并把消息发送给消息循环类
3. `消息循环类 Looper`
    监听消息队列，一旦有消息来临时，就找到该消息的发送目标（Handler），然后送给他。
    并把该消息从消息队列中删除掉。
4. `消息队列类 Queue`
    就是存放所有未处理消息的队列

上面内容参照了Android的Handler+Looper的机制

# Legal

The source for MessageLoop is released under the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

