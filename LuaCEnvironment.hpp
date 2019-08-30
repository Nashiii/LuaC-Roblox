#pragma once
#include "ReturnCheckBypass.hpp" // Bypass for the return check
#include <iostream> //
#include <vector>
#include <istream>
#include <sstream>
#include <iterator>

auto ReBase = [](unsigned long offset) -> auto { return reinterpret_cast<BYTE*>(offset - 0x400000 + reinterpret_cast<DWORD_PTR*>(GetModuleHandleA(0))); };

class LuaC
{
private:
	std::vector<std::string> args; // The vector used to store all of the arguments 
	DWORD_PTR lua_state; // The Roblox lua state
	const enum CFunctions // index for each function that we use in the parser
	{
		emptystack,
		getfield,
		getglobal,
		pushvalue,
		pushstring,
		pushnumber,
		pushboolean,
		pcall,
		setfield,
		settop
	}; // Sorted alphabetically

	auto function_to_enum(std::string method) noexcept -> CFunctions
	{
		if (method == "emptystack")
			return CFunctions::emptystack;
		else if (method == "getfield")
			return CFunctions::getfield;
		else if (method == "getglobal")
			return CFunctions::getglobal;
		else if (method == "pushvalue")
			return CFunctions::pushvalue;
		else if (method == "pushstring")
			return CFunctions::pushstring;
		else if (method == "pushnumber")
			return CFunctions::pushnumber;
		else if (method == "pushboolean")
			return CFunctions::pushboolean;
		else if (method == "pcall")
			return CFunctions::pcall;
		else if (method == "setfield")
			return CFunctions::setfield;
		else if (method == "settop")
			return CFunctions::settop;

		// converts method to its enum
	}
public:
	auto execute() noexcept -> void
	{
		const auto method = function_to_enum(args.at(0));
		const auto method_str = args.at(0);

		switch (method)
		{
		case CFunctions::emptystack:
			reinterpret_cast<void(__stdcall*)(DWORD_PTR, int)>(Retcheck::unprotectfunction<DWORD_PTR>(ReBase(0x84b390)))(lua_state, 0);
			// lua_settop(lua_state, 0) - resets the stack
		case CFunctions::getfield:
			reinterpret_cast<int(__cdecl*)(DWORD_PTR, int, const char*)>(Retcheck::unprotectfunction<DWORD_PTR>(ReBase(0x848BC0)))(lua_state, std::stoi(args.at(1)), args.at(2).c_str());
			// lua_getfield(lua_state, args[1], args[2]) - gets the table and places it on the stack
		case CFunctions::getglobal:
			reinterpret_cast<int(__cdecl*)(DWORD_PTR, int, const char*)>(Retcheck::unprotectfunction<DWORD_PTR>(ReBase(0x848BC0)))(lua_state, -10002, args.at(1).c_str());
			// lua_getglobal(lua_state, args[1]) - gets the table on the globals index and places it on the stack
		case CFunctions::pushvalue:
			reinterpret_cast<void(__cdecl*)(DWORD_PTR, int)>(Retcheck::unprotectfunction<DWORD_PTR>(ReBase(0x84A2C0)))(lua_state, std::stoi(args.at(1)));
			// lua_pushvalue(lua_state, args[1]) - pushes the value args[1] to the stack
		case CFunctions::pushstring:
			reinterpret_cast<void(__stdcall*)(DWORD_PTR, const char*)>(ReBase(0x84A150))(lua_state, args.at(1).c_str());
			// lua_pushstring(lua_state, args[1]) - pushes string args[1] to the top of the stack
		case CFunctions::pushnumber:
			reinterpret_cast<void(__thiscall*)(DWORD_PTR, double)>(Retcheck::unprotectfunction<DWORD_PTR>(ReBase(0x84A0C0)))(lua_state, std::stoi(args.at(1)));
			// lua_pushnumber(lua_state, args[1]) - pushes integer args[1] on to the top of the stack
		case CFunctions::pushboolean:
			reinterpret_cast<void(__cdecl*)(DWORD_PTR, bool)>(Retcheck::unprotectfunction<DWORD_PTR>(ReBase(0x849c50)))(lua_state, std::stoi(args.at(1)));
			// lua_pushboolean(lua_state, args[1]) - pushes bool args[1] to the top of the stack
		case CFunctions::pcall:
			reinterpret_cast<int(__cdecl*)(DWORD_PTR, int, int, int)>(Retcheck::unprotectfunction<DWORD_PTR>(ReBase(0x849b70)))(lua_state, std::stoi(args.at(1)), std::stoi(args.at(2)), std::stoi(args.at(3)));
			// lua_pcall(lua_state, args[1], args[2], args[3]) - calls the function on the top of the stack
		case CFunctions::setfield:
			reinterpret_cast<void(__cdecl*)(DWORD_PTR, int, const char*)>(Retcheck::unprotectfunction<DWORD_PTR>(ReBase(0x84ADA0)))(lua_state, std::stoi(args.at(1)), args.at(2).c_str());
			// lua_setfield(lua_state, args[1], args[2]) - sets a value's property (args[1]) to args[2] 
		case CFunctions::settop:
			reinterpret_cast<void(__stdcall*)(DWORD_PTR, int)>(Retcheck::unprotectfunction<DWORD_PTR>(ReBase(0x84b390)))(lua_state, std::stoi(args.at(1)));
			// lua_settop(lua_state, args[1]) - sets the stack's size to the number which is the second argument
		}
	}

	auto parse_to_words(std::string text) -> void
	{
		std::istringstream iss(text);
		std::vector<std::string> results(std::istream_iterator<std::string>{iss},
			std::istream_iterator<std::string>()); // parse string to words and put it in the iss stream

		args = results; // set the args
	}

	auto set_lua_state(DWORD_PTR lua_State) noexcept -> void
	{
		lua_state = lua_State;
		// set the parser's lua_state to the passed lua_State
	}
};