// ScriptTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "LuaScriptEngn.h"
#include <iostream>

int _tmain(int argc, _TCHAR* argv[])
{
	TLuaScriptEngn* pScriptEngn = TLuaScriptEngn::GetInstance();
	pScriptEngn->Initial("D:/GitHubProject/LuaEngineTest/ScriptTest/Debug/");
	pScriptEngn->DoCommonScriptCall("a.HelloWorldA");
	pScriptEngn->DoCommonScriptCall("a.HelloWorldA2", 10, "haha");
	char c;
	std::cin >> c;
	return 0;
}

