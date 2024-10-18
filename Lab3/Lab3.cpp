#include "tchar.h"
#include <windows.h>
#include <string>
#include <iostream>
#include <format>
#include <fstream>
#include <vector>
# pragma comment(lib, "winmm.lib")

struct Args {
	int id;
	int startTime;
	Args(int _id) : id(_id), startTime(timeGetTime()) {}
};

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
	Args* args = static_cast<Args*>(lpParam);
	std::string fileName = std::format("threadn n{}.txt", args->id);
	std::ofstream file(fileName);
	std::string textToWrite = "";

	for (int c = 0; c < 20; c++) {
		std::vector<int> data(200000, 1);
		for (int i = 0; i < 200000; ++i) {
			data[i] = data[i] * 2;
			for (int j = 0; j < 100; ++j) {
				data[i] += j;
			}
		}

		textToWrite += std::format("{}|{}\n", args->id, timeGetTime() - args->startTime);
	}

	file << textToWrite;

	delete args;
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	int threadsCount;
	std::cout << "Threads count: ";
	std::cin >> threadsCount;

	HANDLE* handles = new HANDLE[threadsCount];
	for (int i = 0; i < threadsCount; i++)
	{
		Args* args = new Args(i + 1);
		handles[i] = CreateThread(NULL, 0, &ThreadProc, args, NULL, NULL);
	}

	WaitForMultipleObjects(threadsCount, handles, true, INFINITE);

	for (int i = 0; i < threadsCount; i++)
	{
		if (handles[i])
			CloseHandle(handles[i]);
	}

	delete[] handles;
	return 0;
}
