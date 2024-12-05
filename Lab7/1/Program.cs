const string inputFilePath = "text.txt";
await RemoveCharactersFromFileAsync(inputFilePath);

async Task RemoveCharactersFromFileAsync(string filePath)
{
    if ( !File.Exists(filePath) )
    {
        Console.WriteLine("Файл не найден.");
        return;
    }

    Console.WriteLine("Введите символы, которые необходимо удалить из текста:");
    string charsToRemove = Console.ReadLine();

    string text = await File.ReadAllTextAsync(filePath);
    string updatedText = RemoveCharacters(text, charsToRemove);

    await SaveTextToFileAsync(filePath, updatedText);
}

string RemoveCharacters(string text, string charsToRemove)
{
    foreach ( char c in charsToRemove )
    {
        text = text.Replace(c.ToString(), string.Empty);
    }
    return text;
}

async Task SaveTextToFileAsync(string filePath, string text)
{
    await File.WriteAllTextAsync(filePath, text);
}
