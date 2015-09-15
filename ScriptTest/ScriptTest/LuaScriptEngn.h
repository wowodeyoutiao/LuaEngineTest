/********************************************************************
*
*  名称：lua5.2.4 脚本引擎
*  内容: 处理lua脚本的加载，lua调用c++类和函数，c++调用lua脚本函数
*
********************************************************************/
#ifndef __LUA_SCRIPT_ENGN_H_
#define __LUA_SCRIPT_ENGN_H_

extern "C"{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include <string>

class TLuaScriptEngn
{
public:
	static TLuaScriptEngn* GetInstance();
	virtual ~TLuaScriptEngn();
	bool Initial(const std::string& sPath);
	bool ReloadAllScript();	
	char* LoadFile(const std::string& sFileName, size_t& iSize);
	void DoCommonScriptCall(const char* sFuncName);
	std::string GetAppPath() { return m_sAppRootPath; }
private:
	TLuaScriptEngn();
	lua_State* LoadAllScript();
	void SetLuaState(lua_State* l);
	char* m_pTempFileBuffer;
	lua_State* m_lua;
	std::string m_sAppRootPath;
};

#endif