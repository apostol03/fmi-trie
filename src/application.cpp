#include "Dictionary.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

/// Display how to use the program from the command line
void displayUsage(const char *executablePath)
{
  try
  {
    fs::path ep(executablePath);

    std::cout
        << "Usage:\n\t"
        << ep.filename()
        << " <dictionary> <filter> <text>"
        << std::endl;
  }
  catch (...)
  {
    std::cout << "Cannot parse path from argv[0]";
  }
}

/**
 * Opens a file for reading.
 * 
 * @param filePath the path to the file
 * 
 * @return The file for reading.
 * @throws std::runtime_error - when the file fails to open
 */
std::ifstream openFile(const std::string &filePath)
{
  std::ifstream file(filePath);
  if (!file.is_open())
  {
    throw std::runtime_error("File couldn't be opened or is corrupted: " + filePath);
  }

  return file;
}

/**
 * Checks if a line should be ignored.
 * 
 * @param line line of a file
 * 
 * @return true if the line should be skipped
 */
bool shouldSkipLine(const std::string &line)
{
  return line.empty() || line[0] == '#';
}

/**
 * Reads the content of a file into a dictionary object.
 * 
 * @param dictionary dictionary object
 * @param filePath path to the file to read from
 * 
 * @return EntryCounter structure containing the count of correct, filtered and incorrect words 
 */
EntryCounter readDictionary(Dictionary &dictionary, const std::string &filePath)
{
  std::ifstream file = openFile(filePath);

  std::cout << "Loading dictionary from " << filePath << "..." << std::endl;

  std::string line;
  int correctWords = 0, incorrectWords = 0;
  int row = 0;
  while (std::getline(file, line))
  {
    if (shouldSkipLine(line))
    {
      continue;
    }

    row++;
    if (!dictionary.isCorrectWord(line.c_str()))
    {
      incorrectWords++;
      std::cerr << "ERROR: incorrect entry \"" << line << "\"" << " on line " << row << std::endl;
      continue;
    }

    correctWords++;
    dictionary.insert(line.c_str());
  }

  file.close();
  std::cout << std::endl;

  return EntryCounter(correctWords, 0, incorrectWords);
}

/**
 * Removes words from the dictionary that are present in the file.
 * 
 * @param dictionary dictionary object
 * @param filePath path to the filter file
 * 
 * @return EntryCounter structure containing the count of correct, filtered and incorrect words 
 */
EntryCounter filterDictionary(Dictionary &dictionary, const std::string &filePath)
{
  std::ifstream file = openFile(filePath);

  std::cout << "Removing the words listed at " << filePath << "..." << std::endl;

  std::string line;
  int correctWords = 0, filteredWords = 0, incorrectWords = 0;
  int row = 0;
  while (std::getline(file, line))
  {
    if (line.empty() || line[0] == '#')
    {
      continue;
    }

    row++;
    if (!dictionary.isCorrectWord(line.c_str()))
    {
      incorrectWords++;
      std::cerr << "ERROR: incorrect entry \"" << line << "\"" << " on line " << row << std::endl;
      continue;
    }

    int currSz = dictionary.size();
    dictionary.erase(line.c_str());
    filteredWords = dictionary.size() < currSz ? filteredWords + 1 : filteredWords;

    correctWords++;
  }

  file.close();
  std::cout << std::endl;

  return EntryCounter(correctWords, filteredWords, incorrectWords);
}

/**
 * Checks if the words in the file are present in the dictionary.
 * 
 * @param dictionary dictionary object
 * @param filePath path to the filter file
 * 
 * @return EntryCounter structure containing the count of correct, filtered and incorrect words 
 */
EntryCounter verifyText(Dictionary &dictionary, const std::string &filePath)
{
  std::ifstream file = openFile(filePath);

  std::cout << "Verifying the contents of " << filePath << "..." << std::endl;

  std::string line;
  int correctWords = 0, incorrectWords = 0;
  int row = 0;
  while (std::getline(file, line))
  {
    row++;
    std::istringstream lineStream(line);
    std::string word;

    while (lineStream >> word)
    {
      if (!dictionary.contains(word.c_str()))
      {
        incorrectWords++;
        std::cerr << "SPELLING ERROR: " << word << " on line " << row << std::endl;
        continue;
      }
      correctWords++;
    }
  }

  file.close();
  std::cout << std::endl;

  return EntryCounter(correctWords, 0, incorrectWords);
}

/**
 * Outputs information about the dictionary entries.
 * 
 * @param dictionary dictionary to check
 * @param dictEntries correct, filtered and incorrect words from the dictionary file
 * @param filterEntries correct, filtered and incorrect words from the filter file
 * @param txtEntries correct, filtered and incorrect words from the text file
 */
void showStatistics(Dictionary &dictionary, EntryCounter dictEntries, EntryCounter filterEntries, EntryCounter txtEntries)
{
  std::cout << "Statistics: \n";
  std::cout << "\tDictionary entries: " << dictEntries.correct << " correct, " << dictEntries.incorrect << " incorrect" << std::endl;
  std::cout << "\tFilter entries: " << filterEntries.correct << " correct, " << filterEntries.incorrect << " incorrect" << std::endl;
  std::cout << "\tCount of filtered entries: " << filterEntries.filtered << std::endl;
  std::cout << "Resultant dictionary: " << dictionary.size() << std::endl;
  std::cout << "\tWords in text: " << txtEntries.correct << " correct, " << txtEntries.incorrect << " incorrect" << std::endl;
}

/**
 * Steps to run easily:
 * 1) cd to /src
 * 2) g++ *.cpp
 * 3) ./a.exe ..\sample-inputs\dictionary.txt ..\sample-inputs\filter.txt ..\sample-inputs\text.txt
 */
int main(int argc, char *argv[])
{
  if (argc < 4)
  {
    displayUsage(argv[0]);
    return 1;
  }

  Dictionary dictionary;
  std::string dictionaryPath = argv[1];
  std::string filterPath = argv[2];
  std::string textPath = argv[3];

  EntryCounter dictEntries = readDictionary(dictionary, dictionaryPath);
  EntryCounter filterEntries = filterDictionary(dictionary, filterPath);
  EntryCounter txtEntries = verifyText(dictionary, textPath);

  showStatistics(dictionary, dictEntries, filterEntries, txtEntries);

  return 0;
}
