#include "LuaScriptEngn.h"
#include "ScriptCommonReg.h"
#include <fstream>
#include <iostream>
#include <string.h>

const std::string SCRIPT_EXT = "lua";
const int FILE_MAX_SIZE = 5 * 1024 * 1024;

static void l_message(const char *pname, const char *msg, const char * func) {
	std::cout << pname << " " << msg << " " << func << std::endl;
}

static int report(lua_State *L, int status, const char* sFuncName) {
	if (status && !lua_isnil(L, -1)) {
		const char *msg = lua_tostring(L, -1);
		if (msg == NULL) msg = "(error object is not a string)";
		l_message("lua", msg, sFuncName);
		lua_pop(L, 1);
		/* force a complete garbage collection in case of errors */
		lua_gc(L, LUA_GCCOLLECT, 0);
	}
	return status;
}

static int traceback(lua_State *L) {
	if (!lua_isstring(L, 1))  /* 'message' not a string? */
		return 1;  /* keep it intact */
	lua_getfield(L, LUA_GLOBALSINDEX, "debug");
	if (!lua_istable(L, -1)) {
		lua_pop(L, 1);
		return 1;
	}
	lua_getfield(L, -1, "traceback");
	if (!lua_isfunction(L, -1)) {
		lua_pop(L, 2);
		return 1;
	}
	lua_pushvalue(L, 1);  /* pass error message */
	lua_pushinteger(L, 2);  /* skip this function and traceback */
	lua_call(L, 2, 1);  /* call debug.traceback */
	return 1;
}

static int docall(lua_State *L, int narg, int nres) {
	int status;
	int base = lua_gettop(L) - narg;  /* function index */
	lua_pushcfunction(L, traceback);  /* push traceback function */
	lua_insert(L, base);  /* put it under chunk and args */
	//globalL = L;  /* to be available to 'laction' */
	//signal(SIGINT, laction);
	status = lua_pcall(L, narg, nres, base);
	//signal(SIGINT, SIG_DFL);
	lua_remove(L, base);  /* remove traceback function */
	return status;
}

static int lua_dofile(lua_State* L, std::string& sFileName)
{
	int status = luaL_loadfile(L, sFileName.c_str()) || docall(L, 0, 1);
	return report(L, status, "");
}

typedef struct _MyLoadFlag
{
	const char* buffer;
	size_t pos;
	size_t size;
}MyLoadFlag;

static const char* My_Reader(lua_State* L, void* pUserData, size_t* size)
{
	MyLoadFlag* ml = (MyLoadFlag*)pUserData;
	*size = ml->size - ml->pos;
	size_t oldPos = ml->pos;
	ml->pos += ml->size;
	return ml->buffer + oldPos;
}

/*
* 加载指定文件，如果是文件夹，则加载文件夹下的init.lua文件
*/
static int My_lua_loadfile(lua_State* L, std::string& sFileName)
{
	for (size_t i = 0; i < sFileName.length(); i++)
	{
		if ('.' == sFileName[i])
			sFileName[i] = '/';
	}
	std::string sTempName = "script/" + sFileName + "." + SCRIPT_EXT;
	MyLoadFlag lf;
	lf.buffer = TLuaScriptEngn::GetInstance()->LoadFile(sTempName, lf.size);
	lf.pos = 0;
	if (nullptr == lf.buffer)
	{
		sTempName = "script/" + sFileName + "/init." + SCRIPT_EXT;
		lf.buffer = TLuaScriptEngn::GetInstance()->LoadFile(sTempName, lf.size);
		lf.pos = 0;
	}
	int status = LUA_ERRERR;
	if (lf.buffer != nullptr && lf.size > 0)
		status = lua_load(L, My_Reader, &lf, sTempName.c_str());
	return status;
}

static int My_lua_dofile(lua_State* L, std::string& sFileName)
{
	int status = My_lua_loadfile(L, sFileName) || docall(L, 0, 1);
	return report(L, status, "");
}

/*
* lua 在 require 的时候会调用该函数加载单元
*/
static int My_LuaLoader(lua_State* L)
{
	std::string sFileName = luaL_checkstring(L, 1);
	int status = My_lua_loadfile(L, sFileName);
	if (0 == status)
		return 1;
	else
	{
		report(L, status, "");
		return 0;
	}
}

static void My_LuaFindFunction(lua_State* L, int idx, const char* sFunc)
{
	const char* c;
	lua_pushvalue(L, idx);
	do
	{
		c = strchr(sFunc, '.');
		if (nullptr == c)
			c = sFunc + strlen(sFunc);

		lua_pushlstring(L, sFunc, c - sFunc);
		lua_rawget(L, -2);
		if (lua_isnil(L, -1))
		{
			lua_remove(L, -2);
			return;
		}
		else if (lua_istable(L, -1))
		{
			lua_remove(L, -2);
			sFunc = c + 1;
		}
		else if (lua_isfunction(L, -1))
		{
			lua_remove(L, -2);
			return;
		}
	} while (*c == '.');
}

/*******************************Start Of LuaScriptEngn*************************************************/
static TLuaScriptEngn* G_LuaScriptEngn = nullptr;
TLuaScriptEngn* TLuaScriptEngn::GetInstance()
{
	if (nullptr == G_LuaScriptEngn)
		G_LuaScriptEngn = new TLuaScriptEngn();
	return G_LuaScriptEngn;
}

TLuaScriptEngn::TLuaScriptEngn() : m_lua(nullptr), m_sAppRootPath("")
{
	m_pTempFileBuffer = (char*)malloc(FILE_MAX_SIZE);
}

TLuaScriptEngn::~TLuaScriptEngn()
{
	if (m_pTempFileBuffer != nullptr)
		free(m_pTempFileBuffer);
}

bool TLuaScriptEngn::Initial(const std::string& sPath)
{
	m_sAppRootPath = sPath;
	lua_State* l = LoadAllScript();
	if (l != nullptr)
	{
		SetLuaState(l);
		return true;
	}
	return false;
}

bool TLuaScriptEngn::ReloadAllScript()
{
	lua_State* l = LoadAllScript();
	if (l != nullptr)
	{
		SetLuaState(l);
		return true;
	}
	return false;
}

lua_State* TLuaScriptEngn::LoadAllScript()
{
	lua_State* L = luaL_newstate();
	lua_gc(L, LUA_GCSTOP, 0);
	luaL_openlibs(L);
	LuaOpen_ServerCommonLib(L);
	//----注册服务器类对象方法
	lua_gc(L, LUA_GCRESTART, 0);

	lua_getglobal(L, "package");
	lua_pushstring(L, "");
	lua_setfield(L, -2, "path");
	lua_pop(L, 1);

	lua_getglobal(L, "package");
	lua_getfield(L, -1, "loaders");
	lua_remove(L, -2);
	lua_pushinteger(L, 2);
	lua_pushcfunction(L, My_LuaLoader);
	lua_rawset(L, -3);
	lua_pop(L, 1);

	std::string sInitFileName = "init";
	if (My_lua_dofile(L, sInitFileName) != 0)
	{
		lua_close(L);
		L = nullptr;
	}
	return L;
}

char* TLuaScriptEngn::LoadFile(const std::string& sFileName, size_t& iSize)
{
	iSize = 0;
	std::string sPath(m_sAppRootPath + sFileName);
	std::fstream file;
	file.open(sPath, std::ios::in | std::ios::ate | std::ios::binary);
	if (file.good())
	{		
		iSize = (size_t)file.tellg();
		file.seekg(0, std::ios::beg);
		file.read(m_pTempFileBuffer, FILE_MAX_SIZE);
		file.close();
		return m_pTempFileBuffer;
	}
	else
	{
		//Failed to load file!!!
	}
	return nullptr;
}

void TLuaScriptEngn::SetLuaState(lua_State* l)
{
	if (nullptr == l)
		return;

	lua_State* old_l = m_lua;
	m_lua = l;
	if (old_l != nullptr)
		lua_close(old_l);

	DoCommonScriptCall("OnInitialize");
}

void TLuaScriptEngn::DoCommonScriptCall(const char* sFuncName)
{
	try
	{
		My_LuaFindFunction(m_lua, LUA_GLOBALSINDEX, sFuncName);
		if (lua_isfunction(m_lua, -1))
			report(m_lua, lua_pcall(m_lua, 0, 0, 0), sFuncName);
		else
			lua_pop(m_lua, 1);
	}
	catch (...)
	{

	}
}

void TLuaScriptEngn::DoCommonScriptCall(const char* sFuncName, int iParam, const std::string& sParam)
{
	try
	{
		My_LuaFindFunction(m_lua, LUA_GLOBALSINDEX, sFuncName);
		if (lua_isfunction(m_lua, -1))
		{
			lua_pushinteger(m_lua, iParam);
			lua_pushstring(m_lua, sParam.c_str());
			report(m_lua, lua_pcall(m_lua, 2, 0, 0), sFuncName);
		}
		else
			lua_pop(m_lua, 1);
	}
	catch (...)
	{

	}
}

/*******************************End Of LuaScriptEngn*************************************************/