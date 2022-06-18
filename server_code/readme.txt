编译指令：
g++ -o server main.cpp threadpool.h qt_conn.h qt_conn.cpp locker.h sql_connection_pool.cpp sql_connection_pool.h -lpthread -lmysqlclient

请求包说明：
	基本格式：
	Type:
		0:注册
		1:登录
		2:发消息给好友
		3:添加好友
		4:发消息给群聊
		5:删除好友
	UserName:
		Type为0或者1：表示注册或者登陆的账号
		Type为2：表示发送好友消息的账号
		Type为3：表示发送好友请求的账号
		Type为4：表示发送群聊消息的账号
		Type为5：表示发送删除信息的账号
	UserPassword:
		Type为0或者1：表示注册或者登陆的密码
		Type为2：表示接受消息的账号
		Type为3：表示接受好友请求的账号
		Type为4：空
		Type为5：表示需要删除的好友的账号
	Message:
		Type为0或者1：空
		Type为2：表示需要发送好友的消息
		Type为3：空
		Type为4：表示需要发送群聊的消息
		Type为5：空
响应包说明：
	基本格式：
	Type:
		0：注册回应
		1：登录回应
		2：发消息给好友回应、将消息转发给好友
		3：添加好友回应、将好友请求转发给用户
		4：发消息给群聊回应、将消息转发给所有在线用户
		5：删除好友回应
	Result:
		0：操作失败
		1：操作成功
	Sname:
		server：服务端
		others：发送源用户名
	Message：
		Type为0或者1：注册或登录的提示信息
		Type为2：发消息回应提示信息、发送的消息
		Type为3：添加好友回应提示信息、用户收到好友请求的提示信息
		Type为4：发消息的用户名的回应提示信息、群聊的在线用户需要接收的信息
		Type为5：空

	
	
