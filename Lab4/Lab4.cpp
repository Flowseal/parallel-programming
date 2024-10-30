#include "BitmapPlusPlus.hpp"
#include "Utils.hpp"
#include <format>
# pragma comment(lib, "winmm.lib")

using namespace std;
using namespace bmp;

struct ThreadArgs
{
	Bitmap* image;
	int lineIndex;
	int lineSize;
	unsigned long startTime;
};

DWORD WINAPI Blur(CONST LPVOID lpParam);

int main(int argc, char* argv[])
{
	auto args = ParseArgs(argc, argv);

	if (!args)
	{
		cout << "Wrong arguments: <inputFile> <outputFile> <cores> <threads> [threadsMode]" << endl;
		return 1;
	}

	int x;
	cin >> x;
	cout << "Cores: " << args->coresCount << endl;
	cout << "Threads: " << args->threadsCount << endl;
	cout << "Mode: " << args->threadsMode << endl;

	try
	{
		LimitCores(args->coresCount);
		auto timeEllapsed = Timer();

		Bitmap* image = new Bitmap(args->inputFileName);
		int lineHeight = image->height() / args->threadsCount;

		HANDLE* handles = new HANDLE[args->threadsCount];
		std::vector<int> modes
			= { THREAD_PRIORITY_NORMAL, THREAD_PRIORITY_NORMAL, THREAD_PRIORITY_NORMAL };

		if (args->threadsMode == "above")
		{
			modes = { THREAD_PRIORITY_ABOVE_NORMAL, THREAD_PRIORITY_NORMAL, THREAD_PRIORITY_NORMAL };
		}
		else if (args->threadsMode == "diff")
		{
			modes = { THREAD_PRIORITY_ABOVE_NORMAL, THREAD_PRIORITY_NORMAL, THREAD_PRIORITY_BELOW_NORMAL };
		}

		for (int i = 0; i < args->threadsCount; i++)
		{
			handles[i] = CreateThread(NULL, 0, &Blur, new ThreadArgs{ image, i, lineHeight, timeGetTime() }, NULL, NULL);
			SetThreadPriority(handles[i], i < 3 ? modes[i] : THREAD_PRIORITY_NORMAL);
		}

		WaitForMultipleObjects(args->threadsCount, handles, true, INFINITE);

		image->save(args->outputFileName);
		cout << "Time: " << timeEllapsed() << endl;
	}
	catch (const Exception& e)
	{
		cout << e.what() << endl;
		return 1;
	}

	return 0;
}

Pixel Average(vector<Pixel> const& v)
{
	auto const count = v.size();
	int sumR = 0;
	int sumG = 0;
	int sumB = 0;

	for (auto const& pixel : v)
	{
		sumR += pixel.r;
		sumG += pixel.g;
		sumB += pixel.b;
	}

	return Pixel(sumR / count, sumG / count, sumB / count);
}


Pixel GetAverageColor(Bitmap const& img, int x, int y)
{
	static const int BLUR_SIZE = 1;
	vector<Pixel> pixels{};

	for (int i = -BLUR_SIZE; i <= BLUR_SIZE; i++) {
		if (x + i < 0 || x + i >= img.width())
			continue;

		for (int j = -BLUR_SIZE; j <= BLUR_SIZE; j++) {
			if (y + j < 0 || y + j >= img.height())
				continue;

			pixels.push_back(img.get(x + i, y + j));
		}
	}

	return Average(pixels);
}

DWORD WINAPI Blur(CONST LPVOID lpParam)
{
	auto data = reinterpret_cast<ThreadArgs*>(lpParam);

	std::string fileName = std::format("threadn n{}.txt", data->lineIndex + 1);
	std::ofstream file(fileName);
	std::string textToWrite = "";

	int startY = data->lineIndex * data->lineSize;
	int endY = startY + data->lineSize;

	auto image = data->image;
	int imageWidth = image->width();

	for (int y = startY; y < endY; ++y)
	{
		for (int x = 0; x < imageWidth; ++x)
		{
			for (int times = 0; times < 2748; times++)
			{
				image->set(x, y, GetAverageColor(*image, x, y));
			}
			textToWrite += std::format("{}|{}\n", data->lineIndex + 1, timeGetTime() - data->startTime);
		}
	}

	file << textToWrite;

	delete data;
	return 0;
}