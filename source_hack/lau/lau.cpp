#include "lau.h"
#include "lua.hpp"
#include <io.h>
#include <Windows.h>

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

Lau::Lau(void)
{

	luadir = "C:\\aim-flex\\lua\\";

	L = luaL_newstate();

}

#define _(x) MessageBoxA(0,x,x,0)

long Lau::ReadLuaFile(const char *relpath, char **output)
{
	char temp[MAX_PATH + 1];
	sprintf_s(temp, "%s%s", luadir, relpath);
	_(temp);

	FILE *file;
	errno_t code;
	if ((code = fopen_s(&file, temp, "rb")) != 0)
		return 0;
	_("b");
	fseek(file, 0, SEEK_END);
	long length = ftell(file);
	fseek(file, 0, SEEK_SET);

	_("c");
	*output = new char[length];

	fread(*output, 1, length, file);

	fclose(file);
	_("d");

	return length;
}

int Lau::LoadBuffer(const char *contents, int content_length, const char *name)
{

	return luaL_loadbuffer(GetState(), contents, content_length, name);

}

int Lau::RunLuaFile(const char *relpath, bool safe)
{

	int top;
	char *contents;
	long length;
	int code;

	top = lua_gettop(L);

	if ((length = ReadLuaFile(relpath, &contents)) != 0)
	{
		code = LoadBuffer(contents, length, relpath);
		delete[] contents;
		if (code == 0)
		{
			if (safe)
				return lua_pcall(L, 0, 0, 0);
			else
				lua_call(L, 0, 0);
			return 0;
		}

		lua_error(L);

		return -1;
	}

	return -1;

}

extern luaL_Reg LuaAngleMetaTable[];
extern luaL_Reg LuaVectorMetaTable[];
extern luaL_Reg GlobalLibrary[];

void Lau::Init(void)
{
	luaL_openlibs(L);

	lua_newtable(L);
	{
		lua_newtable(L); // value
		{
			luaL_setfuncs(L, LuaAngleMetaTable, 0);
		}
		lua_setfield(L, -2, "Angle");

		lua_newtable(L); // value
		{
			luaL_setfuncs(L, LuaVectorMetaTable, 0);
		}
		lua_setfield(L, -2, "Vector");
	}
	metatables = luaL_ref(L, LUA_REGISTRYINDEX);
	

	lua_pushglobaltable(L);
	{
		luaL_setfuncs(L, GlobalLibrary, 0);
	}
	lua_pop(L, 1);

	if (RunLuaFile("init.lua", true) != 0)
	{
		MessageBoxA(0, lua_tostring(L, -1), "ERROR", 0);
		lua_pop(L, 1);
	}

}