#include "tchar.h"
#include <windows.h>
#include <string>
#include <iostream>
#include <syncstream>

struct Args {
	int id;
	int sleepTime;
	Args(int _id, int _sleepTime) : id(_id), sleepTime(_sleepTime) {}
};

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
	Args* args = static_cast<Args*>(lpParam);

	std::osyncstream(std::cout) << args->id << " Started, sleepTime: " << args->sleepTime << std::endl;
	Sleep(args->sleepTime);
	std::osyncstream(std::cout) << args->id << " End" << std::endl;

	delete args;
	ExitThread(0);
}

int _tmain(int argc, _TCHAR* argv[])
{
	int threadsCount;
	std::cout << "Threads count: ";
	std::cin >> threadsCount;

	HANDLE* handles = new HANDLE[threadsCount];
	for (int i = 0; i < threadsCount; i++) 
	{
		Args* args = static_cast<Args*>(std::malloc(sizeof(Args)));
		args = new(args) Args(i, i % 2 == 0 ? 3000 : 1000);
		handles[i] = CreateThread(NULL, 0, &ThreadProc, args, NULL, NULL);
	}

	WaitForMultipleObjects(threadsCount, handles, true, INFINITE);
	delete[] handles;
	return 0;
}
