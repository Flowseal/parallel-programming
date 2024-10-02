#include "BitmapPlusPlus.hpp"
#include "Utils.hpp"

using namespace std;
using namespace bmp;

struct ThreadArgs
{
    Bitmap* image;
    int lineIndex;
    int lineSize;
};

DWORD WINAPI Blur(CONST LPVOID lpParam);

int main(int argc, char* argv[])
{
    auto args = ParseArgs(argc, argv);

    if (!args)
    {
        cout << "Wrong arguments: <input file name> <output file name> <cores count> <threads count>" << endl;
        return 1;
    }

    try
    {
        LimitCores(args->coresCount);
        auto timeEllapsed = Timer();

        Bitmap* image = new Bitmap(args->inputFileName);
        int lineHeight = image->height() / args->threadsCount;

        HANDLE* handles = new HANDLE[args->threadsCount];

        for (int i = 0; i < args->threadsCount; i++)
        {
            handles[i] = CreateThread(NULL, 0, &Blur, new ThreadArgs{image, i, lineHeight}, NULL, NULL);
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
    static const int BLUR_SIZE = 5;
    vector<Pixel> pixels{};

    for (int i = -BLUR_SIZE; i <= BLUR_SIZE; i++) {
        if (x + i < 0 || x + i >= img.width())
            continue;

        for (int j = -BLUR_SIZE; j <= BLUR_SIZE; j++) {
            if (y + j < 0 || y + j >= img.height())
                continue;

            pixels.push_back(img.get(x+i, y+j));
        }
    }

    return Average(pixels);
}

DWORD WINAPI Blur(CONST LPVOID lpParam)
{
    auto data = reinterpret_cast<ThreadArgs*>(lpParam);

    int startY = data->lineIndex * data->lineSize;
    int endY = startY + data->lineSize;

    auto image = data->image;
    int imageWidth = image->width();

    for (int y = startY; y < endY; ++y)
    {
        for (int x = 0; x < imageWidth; ++x)
        {
            image->set(x, y, GetAverageColor(*image, x, y));
        }
    }

    delete data;
    return 0;
}