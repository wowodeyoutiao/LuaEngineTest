// ScriptTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "LuaScriptEngn.h"
#include <iostream>

int _tmain(int argc, _TCHAR* argv[])
{
	TLuaScriptEngn* pScriptEngn = TLuaScriptEngn::GetInstance();
	pScriptEngn->Initial("D:/GitHubProject/LuaEngnTest/ScriptTest/Debug/");
	//pScriptEngn->DoCommonScriptCall("a.HelloWorldA");
	char c;
	std::cin >> c;
	return 0;
}

