#pragma once
#include <iostream>
#include <optional>
#include <string>
#include <functional>
#include <chrono>
#include <Windows.h>


struct Args
{
    std::string inputFileName;
    std::string outputFileName;
    int coresCount;
    int threadsCount;
};

std::optional<Args> ParseArgs(int argc, char* argv[])
{
    if (argc != 5)
    {
        return std::nullopt;
    }

    Args result;

    result.inputFileName = argv[1];
    result.outputFileName = argv[2];

    try
    {
        result.coresCount = std::stoi(argv[3]);
        result.threadsCount = std::stoi(argv[4]);
    }
    catch (...)
    {
        return std::nullopt;
    }

    return result;
}

void LimitCores(int coresAmount)
{
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);

    int count = min(max(1, coresAmount), sysinfo.dwNumberOfProcessors);

    auto procHandle = GetCurrentProcess();
    SetProcessAffinityMask(procHandle, (1 << count) - 1);
}

std::function<std::chrono::duration<double>()> Timer()
{
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

    return [start]()
    {
        return std::chrono::steady_clock::now() - start;
    };
}