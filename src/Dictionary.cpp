#include "Dictionary.h"

#include <cctype> // Contains std::isalpha()

Node::Node() : end(false), childrenCount(0)
{
  for (int i = 0; i < ALPHABET_SIZE; i++)
  {
    successors[i] = nullptr;
  }
}

bool Node::containsKey(char c) const
{
  return successors[c - 'a'] != nullptr;
}

void Node::put(char c, Node *node)
{
  int index = c - 'a';
  if (successors[index] == nullptr && node != nullptr)
  {
    ++childrenCount;
  }
  else if (successors[index] != nullptr && node == nullptr)
  {
    --childrenCount;
  }

  successors[index] = node;
}

Node *Node::get(char c) const
{
  return successors[c - 'a'];
}

bool Node::hasChildren() const
{
  return childrenCount > 0;
}

Dictionary::Dictionary() : root(new Node()), sz(0) {}

Dictionary::Dictionary(const Dictionary &other) : root(nullptr)
{
  copy(other);
}

Dictionary &Dictionary::operator=(const Dictionary &other)
{
  if (this != &other)
  {
    clear();
    copy(other);
  }
  return *this;
}

Dictionary::~Dictionary()
{
  clear();
}

void Dictionary::copy(const Dictionary &other)
{
  if (other.root)
  {
    root = new Node(*other.root);
  }
  sz = other.sz;
}

void Dictionary::deleteSubTrie(Node *node)
{
  for (size_t i = 0; i < ALPHABET_SIZE; i++)
  {
    if (node->successors[i])
    {
      deleteSubTrie(node->successors[i]);
    }
  }
  delete node;
}

void Dictionary::clear()
{
  if (root)
  {
    deleteSubTrie(root);
    root = nullptr;
  }
}

void Dictionary::insert(const char *word)
{
  if (!isCorrectWord(word))
  {
    throw incorrect_word_exception();
  }

  try
  {
    Node *node = root;
    int strl = strlen(word);

    for (size_t i = 0; i < strl; i++)
    {
      char curr = tolower(word[i]);
      if (!node->containsKey(curr))
      {
        node->put(curr, new Node());
      }
      node = node->get(curr);
    }

    node->end = true;
    sz++;
  }
  catch (...)
  {
    throw std::invalid_argument("Something is wrong with this word! Failed to insert...");
  }
}

void Dictionary::erase(const char *word) noexcept
{
  if (isCorrectWord(word) && contains(word))
  {
    root = eraseHelper(root, word, 0);
    sz--;
  }
}

Node *Dictionary::eraseHelper(Node *node, const char *word, int depth)
{
  if (!node)
  {
    return nullptr;
  }

  if (depth == strlen(word))
  {
    if (node->end)
    {
      node->end = false;
    }

    if (!node->hasChildren())
    {
      delete node;
      node = nullptr;
    }

    return node;
  }

  char curr = tolower(word[depth]);
  Node *nextNode = node->get(curr);

  node->put(curr, eraseHelper(nextNode, word, depth + 1));

  if (!node->hasChildren() && !node->end)
  {
    delete node;
    node = nullptr;
  }

  return node;
}

bool Dictionary::contains(const char *word) const noexcept
{
  if (word == nullptr || !root)
  {
    return false;
  }

  Node *node = root;
  int strl = strlen(word);
  for (size_t i = 0; i < strl; i++)
  {
    char curr = tolower(word[i]);
    if (!node->containsKey(curr))
    {
      return false;
    }
    node = node->get(curr);
  }

  return node != nullptr && node->end;
}

size_t Dictionary::size() const noexcept
{
  return sz;
}

bool Dictionary::isCorrectWord(const char *word) noexcept
{
  return word != nullptr && strlen(word) > 0 && isValidWord(word);
}

bool Dictionary::isValidWord(const char *word)
{
  while (*word != '\0')
  {
    if (*word == ' ' || !std::isalpha(static_cast<unsigned char>(*word)))
    {
      return false;
    }

    word++;
  }

  return true;
}
