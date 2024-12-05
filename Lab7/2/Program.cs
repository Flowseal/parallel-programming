using System.Diagnostics;
using System.Net.Http.Json;

HttpClient client = new HttpClient();
const string apiUrl = "https://dog.ceo/api/breeds/image/random";
const int numberOfImages = 10;

Console.WriteLine("Asynchronous execution:");
await DownloadImagesAsync();

Console.WriteLine("\nSynchronous execution:");
DownloadImages();

async Task DownloadImagesAsync()
{
    Stopwatch stopwatch = Stopwatch.StartNew();
    Task[] tasks = new Task[numberOfImages];

    for (int i = 0; i < numberOfImages; i++)
    {
        tasks[i] = DownloadRandomImageAsync();
    }

    Console.WriteLine("Downloading images...");
    await Task.WhenAll(tasks);
    stopwatch.Stop();
    Console.WriteLine($"Asynchronous time: {stopwatch.ElapsedMilliseconds} ms");
}

async Task DownloadRandomImageAsync()
{
    try
    {
        ApiResponse response = await client.GetFromJsonAsync<ApiResponse>(apiUrl);
        if (response.Status == "success")
        {
            string imageUrl = response.Message;
            Console.WriteLine($"Starting downloading from url: {imageUrl}");

            await client.GetByteArrayAsync(imageUrl);
            Console.WriteLine($"Image from {imageUrl} downloaded successfully");
        }
        else
        {
            Console.WriteLine($"Failed to get image URL: {response}");
        }
    }
    catch (HttpRequestException e)
    {
        Console.WriteLine($"Request error: {e.Message}");
    }
}

void DownloadImages()
{
    var stopwatch = Stopwatch.StartNew();
    Console.WriteLine("Downloading images...");

    for (int i = 0; i < numberOfImages; i++)
    {
        DownloadRandomImage();
    }

    stopwatch.Stop();
    Console.WriteLine($"Synchronous time: {stopwatch.ElapsedMilliseconds} ms");
}

void DownloadRandomImage()
{
    try
    {
        var response = client.GetFromJsonAsync<ApiResponse>(apiUrl).Result;
        if (response?.Status == "success")
        {
            string imageUrl = response.Message;
            Console.WriteLine($"Starting downloading from url: {imageUrl}");

            client.GetByteArrayAsync(imageUrl);
            Console.WriteLine($"Image from {imageUrl} downloaded successfully");
        }
        else
        {
            Console.WriteLine($"Failed to get image URL: {response}");
        }
    }
    catch (AggregateException e)
    {
        Console.WriteLine($"Request error: {e.InnerException?.Message}");
    }
}

public class ApiResponse
{
    public string Message { get; set; }
    public string Status { get; set; }
}
