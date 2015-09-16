#include "ScriptClassBaseReg.h"

void TScriptBaseReg::Initial(lua_State* L)
{
	luaL_newmetatable(L, m_sClassTableName.c_str());
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	DoRegister(L);
	lua_pop(L, 1);
}

void TScriptBaseReg::Set(lua_State* L, void* pObject)
{
	//------
}