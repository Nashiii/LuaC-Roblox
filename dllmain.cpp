// DLLEntry.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <iostream>
#include "LuaCEnvironment.hpp"
#include "SignatureScanner.hpp"

auto setThreadIdentity(INT rbx_L, unsigned __int8 identity) noexcept -> void
{
	/*
		Author: Azurillex
		Link: https://github.com/Azurilex/Set-Thread-Identity
		Description: Changes the identity for the rbx_L thread
	*/
	DWORD_PTR* loc1 = reinterpret_cast<DWORD_PTR*>(rbx_L - 40);
	*loc1 ^= (identity ^ static_cast<unsigned __int8>(*loc1)) & 0x1F; 
}

auto EntryPoint() noexcept -> void
{
	std::string coin;
	// string for input (stdin)
	std::unique_ptr<LuaC> Parser = std::make_unique<LuaC>();
	// create shared pointer for the parser class (LuaCEnvironment.hpp)
	
	{
		std::unique_ptr<Memory> sigscan = std::make_unique<Memory>();
		// create unique pointer for the sigscan class (SignatureScanner.hpp)

		const uint32_t SCVFT = 0x1E2D9D8;
		const uint32_t ScriptContext = sigscan->scan(reinterpret_cast<char*>(SCVFT));
		// RBX::ScriptContext location

		auto newthread = reinterpret_cast<DWORD_PTR(__cdecl*)(DWORD_PTR)>(Retcheck::unprotectfunction<DWORD_PTR>(ReBase(0x849810))); 
		// function that calls newthread
		auto _lua_state = *(DWORD_PTR*)(ScriptContext + 56 * 0 + 164) - (ScriptContext + 56 * 0 + 164);
		// lua_State location

		Parser->set_lua_state(newthread(_lua_state));
		// set the lua state to Roblox's state
	}
	
	std::cout << "=> "; // output '=>' to the console
	std::getline(std::cin, coin); // puts the input in to coin
	std::cout << "\n"; // make a new line

	Parser->parse_to_words(coin); // set the vector to the parsed command
	Parser->execute(); // execute the parsed input
}

auto __stdcall DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) -> bool
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(0, 0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(EntryPoint), 0, 0); 
		// Call EntryPoint in a new thread
	}
	return true;
}

