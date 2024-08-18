#pragma once

#include <unordered_set>

#include <cstring>
#include <iostream>

static const int ALPHABET_SIZE = 26;

class incorrect_word_exception : public std::runtime_error
{
public:
  incorrect_word_exception()
      : runtime_error("incorrect word")
  {
    // Nothing to do here
  }
};

struct EntryCounter
{
  int correct;
  int filtered;
  int incorrect;

  EntryCounter(int correct, int filtered, int incorrect) : correct(correct), filtered(filtered), incorrect(incorrect) {}
};

struct Node
{
  Node *successors[ALPHABET_SIZE];
  bool end;
  int childrenCount;

  Node();
  bool containsKey(char c) const;
  void put(char c, Node *node);
  Node *get(char c) const;
  bool hasChildren() const;
};

class Dictionary
{
public:
  Dictionary();
  Dictionary(const Dictionary &other);
  Dictionary &operator=(const Dictionary &other);
  ~Dictionary();

public:
  void insert(const char *word);
  void erase(const char *word) noexcept;
  bool contains(const char *word) const noexcept;
  size_t size() const noexcept;
  static bool isCorrectWord(const char *word) noexcept;

private:
  void copy(const Dictionary &other);
  void clear();
  void deleteSubTrie(Node *node);

private:
  static bool isValidWord(const char *word);
  Node *eraseHelper(Node *node, const char *word, int depth);

private:
  Node *root;
  size_t sz;
};