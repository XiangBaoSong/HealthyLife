#include"../include/json/json.h"
#include"../DataBase/DataBase.h"

/*************************************************************************
 *说明: Proto作为所有业务协议的抽象接口,每个业务接口只要实现各自的接口即可
 *		线程模块和网络模块不需要做任何修改!不过需要在ProtoManager构造函数
 *		里面注册自己;
 * **********************************************************************/ 
class Proto {
	public:
								Proto(){};
		virtual					~Proto(){};

		/**************************************************************
		 *功能: 获取协议对象的指针,指向的对象就是本协议静态对象,这样可以
		 *		避免内存管理的麻烦
		 *
		 *返回: 返回静态成员协议对象的指针;
		 *
		 * *************************************************************/ 
		virtual  const Proto* 	getInstance() = 0;

		/**************************************************************
		 *功能: 获取协议对象的名称;每个类具体实现的时候都需要唯一指定自
		 *		名称,然后prottoManger会根据名称map一个协议对象,接收到请求
		 *		时直接根据消息里面的协议名称找到具体的协议对象;
		 *
		 *返回: 协议名称
		 * ***********************************************************/ 
		virtual const char* 	getName() = 0;

		/**************************************************************
		 *功能: 执行具体的业务;
		 *
		 *参数: Json:Vaule & jData:包含协议数据的json对象
		 *
		 *返回: 需要返回给客户端的数据,用json格式包装
		 * ***********************************************************/ 
		virtual Json::Value 	dispatch(const Json::Value& jData, DataBase* pDB) = 0;
	protected:
		char 					m_szName[128];
};
