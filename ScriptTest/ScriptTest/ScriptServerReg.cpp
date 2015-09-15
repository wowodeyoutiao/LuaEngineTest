#include "ScriptServerReg.h"

int _MyTestFunc(lua_State* l)
{
	return 1;
}

static const luaL_Reg Server_Common_Lib[] = {
	{ "MyTestFunc", _MyTestFunc },
	{ nullptr, nullptr }
};

LUA_API int LuaOpen_ServerCommonLib(lua_State* l)
{
	luaL_newlib(l, Server_Common_Lib);
	return 1;
}