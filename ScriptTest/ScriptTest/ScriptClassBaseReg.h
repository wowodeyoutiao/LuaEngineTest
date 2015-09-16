/********************************************************************
*
*  名称：服务器对象的脚本注册基类
*  内容:
*
********************************************************************/
#ifndef __SCRIPT_CLASS_BASE_REG_H_
#define __SCRIPT_CLASS_BASE_REG_H_

extern "C"{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include <string>

class TScriptBaseReg
{
public:
	TScriptBaseReg(const std::string& sTableName) : m_sClassTableName(sTableName){};
	virtual ~TScriptBaseReg(){};
protected:
	void Initial(lua_State* L);
	void Set(lua_State* L, void* pObject);
	virtual void DoRegister(lua_State* L) abstract;
	std::string m_sClassTableName;
};

#endif  //__SCRIPT_CLASS_BASE_REG_H_