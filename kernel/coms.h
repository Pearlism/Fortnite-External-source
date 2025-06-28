#pragma once
#include "Windows.h"
#include <iostream>
#include <TlHelp32.h>
#include <winioctl.h> 
#include <random>
#include <thread>
#include <winternl.h>       

uintptr_t addy;

extern "C" __int64 direct_device_control(
	HANDLE FileHandle,
	HANDLE Event,
	PIO_APC_ROUTINE ApcRoutine,
	PVOID ApcContext,
	PIO_STATUS_BLOCK IoStatusBlock,
	std::uint32_t IoControlCode,
	PVOID InputBuffer,
	std::uint32_t InputBufferLength,
	PVOID OutputBuffer,
	std::uint32_t OutputBufferLength);

#define CODE_GET_BASE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1655, METHOD_BUFFERED, FILE_SPECIAL_ACCESS) 
#define CODE_WRITE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x9651, METHOD_BUFFERED, FILE_SPECIAL_ACCESS) 
#define CODE_READ CTL_CODE(FILE_DEVICE_UNKNOWN, 0x16513, METHOD_BUFFERED, FILE_SPECIAL_ACCESS) 
#define CODE_CR3 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x9623, METHOD_BUFFERED, FILE_SPECIAL_ACCESS) 


// customer codes
//#define CODE_GET_BASE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2655, METHOD_BUFFERED, FILE_SPECIAL_ACCESS) 
//#define CODE_WRITE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x96514, METHOD_BUFFERED, FILE_SPECIAL_ACCESS) 
//#define CODE_READ CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1212, METHOD_BUFFERED, FILE_SPECIAL_ACCESS) 
//#define CODE_CR3 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x5413, METHOD_BUFFERED, FILE_SPECIAL_ACCESS) 


typedef struct Fetch {
	INT32 processId;
	std::uintptr_t* address;
} Fetch, * PFetch;


typedef struct Bypass {
	INT32 processId;
	std::uintptr_t* address;
} Bypass, * PBypass;

typedef struct Read {
	INT32 process_id;
	ULONGLONG address;
	ULONGLONG buffer;
	ULONGLONG size;
} Read, * PRead;

typedef struct Write {
	INT32 process_id;
	ULONGLONG address;
	ULONGLONG buffer;
	ULONGLONG size;
} Write, * PWrite;

// customer handle *notbypassingeactrust*

bool Valid(const uint64_t adress)
{
	if (adress <= 0x400000 || adress == 0xCCCCCCCCCCCCCCCC || reinterpret_cast<void*>(adress) == nullptr || adress >
		0x7FFFFFFFFFFFFFFF) {
		return false;
	}
	return true;
}

struct Kernel {
	HANDLE Driver;
	INT32 ProcessId;
	uintptr_t ProcessBase;
	uintptr_t cr3;
	std::uint64_t GameAssembly = 0x0;
	std::uint64_t UnityPlayer = 0x0;

	bool Init() {
		auto DriverName = "\\\\.\\*{IOS_MANAGE}*";

		Driver = CreateFileA(DriverName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

		if (!Driver || (Driver == INVALID_HANDLE_VALUE))
			return false;

		return true;
	}

	bool ReadPhysical(PVOID address, PVOID buffer, DWORD size) {

		IO_STATUS_BLOCK block;
		Read arguments = { 0 };

		arguments.address = (ULONGLONG)address;
		arguments.buffer = (ULONGLONG)buffer;
		arguments.size = size;
		arguments.process_id = ProcessId;


		return 	direct_device_control(Driver, nullptr, nullptr, nullptr, &block, CODE_READ, &arguments, sizeof(arguments), &arguments, sizeof(arguments));



	}

	inline std::uintptr_t get_module(const wchar_t* name)
	{
		const auto handle = OpenProcess(PROCESS_QUERY_INFORMATION, 0, ProcessId);
		auto current = 0ull;
		auto mbi = MEMORY_BASIC_INFORMATION();
		while (VirtualQueryEx(handle, reinterpret_cast<void*>(current), &mbi, sizeof(MEMORY_BASIC_INFORMATION)))
		{
			if (mbi.Type == MEM_MAPPED || mbi.Type == MEM_IMAGE)
			{
				const auto buffer = malloc(1024);
				auto bytes = std::size_t();
				const static auto ntdll = GetModuleHandleA(("ntdll"));
				const static auto nt_query_virtual_memory_fn =
					reinterpret_cast<NTSTATUS(__stdcall*)(HANDLE, void*, std::int32_t, void*, std::size_t, std::size_t*)> (
						GetProcAddress(ntdll, ("NtQueryVirtualMemory")));

				if (nt_query_virtual_memory_fn(handle, mbi.BaseAddress, 2, buffer, 1024, &bytes) != 0 ||
					!wcsstr(static_cast<UNICODE_STRING*>(buffer)->Buffer, name) ||
					wcsstr(static_cast<UNICODE_STRING*>(buffer)->Buffer, (L".mui")))
				{
					free(buffer);
					goto skip;
				}
				free(buffer);
				CloseHandle(handle);

				return reinterpret_cast<std::uintptr_t>(mbi.BaseAddress);
			}
		skip:
			current = reinterpret_cast<std::uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
		}
		CloseHandle(handle);
		return 0ull;
	}



	bool WritePhysical(PVOID address, PVOID buffer, DWORD size) {
		if (Valid((ULONGLONG)address))
		{
			IO_STATUS_BLOCK block;
			Write arguments = { 0 };

			arguments.address = (ULONGLONG)address;
			arguments.buffer = (ULONGLONG)buffer;
			arguments.size = size;
			arguments.process_id = ProcessId;

			return 	direct_device_control(Driver, nullptr, nullptr, nullptr, &block, CODE_WRITE, &arguments, sizeof(arguments), &arguments, sizeof(arguments));
		}
		else
		{
			return false;
		}

	}

	void Base() {
		IO_STATUS_BLOCK block;

		uintptr_t image_address = { NULL };
		Fetch arguments = { NULL };

		arguments.processId = ProcessId;
		arguments.address = (ULONGLONG*)&image_address;

		direct_device_control(Driver, nullptr, nullptr, nullptr, &block, CODE_GET_BASE, &arguments, sizeof(arguments), &arguments, sizeof(arguments));

		ProcessBase = image_address;
		//return image_address;
	}


	bool Cr3() {
		IO_STATUS_BLOCK block;

		uintptr_t image_address = { NULL };
		Bypass arguments = { NULL };

		arguments.processId = ProcessId;
		arguments.address = &image_address;

		cr3 = image_address;

		return direct_device_control(Driver, nullptr, nullptr, nullptr, &block, CODE_CR3, &arguments, sizeof(arguments), &arguments, sizeof(arguments));

	}

	void Attach(LPCTSTR process_name) {
		PROCESSENTRY32 pt;
		HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		pt.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hsnap, &pt)) {
			do {
				if (!lstrcmpi(pt.szExeFile, process_name)) {
					CloseHandle(hsnap);
					ProcessId = pt.th32ProcessID;
				}
			} while (Process32Next(hsnap, &pt));
		}
		//CloseHandle(hsnap);
	}
}; static Kernel* kernel = new Kernel();

template <typename T>
T read(uint64_t address) {
	T buffer{ };

	/*if (!Valid(address) || !address)
		return buffer;*/

	kernel->ReadPhysical((PVOID)address, &buffer, sizeof(T));

	return buffer;
}

template<typename Type>
Type ReadChain(const std::uint64_t& Address, std::vector<std::uint64_t> Offsets)
{

	if (!Valid(Address) || !Address)
		return 0;

	// Initilizing Value
	std::uint64_t Value = Address;

	// Reading The Offsets Into The Value
	for (int i = 0; i < Offsets.size() - 1; i++)
	{
		const std::uint64_t& Offset = Offsets[i];

		Value = read<std::uint64_t>(Value + Offset);
	}

	// Returning The Value + Final Offset
	return read<Type>(Value + Offsets[Offsets.size() - 1]);
}
std::string ReadWString(const std::uint64_t& Address)
{
	if (!Valid(Address) || !Address)
		return "";


	std::uint64_t NewAddress = read<std::uint64_t>(Address);
	if (!NewAddress)
		return "";

	int Length = read<int>(NewAddress + 0x10);
	if (Length <= 0)
		return "";

	std::vector<wchar_t> Buffer(Length + 1, L'\0');

	kernel->ReadPhysical(reinterpret_cast<PVOID>(NewAddress + 0x14), Buffer.data(), Length * sizeof(wchar_t));

	std::wstring TempString(Buffer.data(), Length);

	for (auto& ch : TempString) {
		if (ch < 32 || ch > 126) {
			ch = L'?';
		}
	}

	return std::string(TempString.begin(), TempString.end());
}

inline std::string read_wstr(uintptr_t address)
{
	if (!Valid(address) || !address)
		return "";

	wchar_t buffer[1024];
	kernel->ReadPhysical(reinterpret_cast<PVOID>(address), buffer, sizeof(buffer));

	buffer[1023] = L'\0';

	std::wstring wstr(buffer);
	for (auto& ch : wstr) {
		if (ch < 32 || ch > 126) {
			ch = L'?';
		}
	}

	return std::string(wstr.begin(), wstr.end());
}

inline std::string readstring(uint64_t Address)
{
	if (!Valid(Address) || !Address)
		return "";

	std::unique_ptr<char[]> buffer(new char[64]);

	kernel->ReadPhysical((PVOID)Address, buffer.get(), 64);

	return buffer.get();

}

template <typename T>
T tWrite(uint64_t address, T buffer) {
	if (!Valid(address) || !address)
		return buffer;

	kernel->WritePhysical((PVOID)address, &buffer, sizeof(T));

	return buffer;
}
