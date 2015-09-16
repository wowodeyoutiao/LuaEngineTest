#include "ScriptCommonReg.h"
#include <iostream>

int _MyTestFunc1(lua_State* l)
{
	std::cout << "Show MyTestFunc1" << std::endl;
	return 1;
}

int _MyTestFunc2(lua_State* l)
{
	std::cout << "Show MyTestFunc2" << std::endl;
	return 1;
}

int _MyTestFunc3(lua_State* l)
{
	std::cout << "Show MyTestFunc3" << std::endl;
	return 1;
}

static const luaL_Reg Server_Common_Lib[] = {
	{ "MyTestFunc1", _MyTestFunc1 },
	{ "MyTestFunc2", _MyTestFunc2 },
	{ "MyTestFunc3", _MyTestFunc3 },
	{ nullptr, nullptr }
};

LUA_API int LuaOpen_ServerCommonLib(lua_State* l)
{
	luaL_register(l, "ServerCommon", Server_Common_Lib);
	return 1;
}