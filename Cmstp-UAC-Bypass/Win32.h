#pragma once

#include <Windows.h>
#include <winternl.h>
#include <stdio.h>

#define PRINT(x, ...) printf(x, ##__VA_ARGS__) 

typedef NTSTATUS (NTAPI* NTQIP)(
    IN HANDLE ProcessHandle,
    IN PROCESSINFOCLASS ProcessInformationClass,
    OUT PVOID ProcessInformation,
    IN ULONG ProcessInformationLength,
    OUT PULONG ReturnLength OPTIONAL
);


PVOID  GetProcessImageBase(IN HANDLE hProcess);