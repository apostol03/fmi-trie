#include "catch2/catch_all.hpp"
#include "Dictionary.h"

TEST_CASE("Node::containsKey() returns false for a non-present character")
{
    Node node;
    node.put('a', new Node());
    REQUIRE_FALSE(node.containsKey('b'));
}

TEST_CASE("Node::containsKey() returns true for a present character")
{
    Node node;
    node.put('a', new Node());
    REQUIRE(node.containsKey('a'));
    REQUIRE(node.containsKey('A'));
}

TEST_CASE("Node::get() returns nullptr at a non-present character")
{
    Node node;
    REQUIRE(node.get('b') == nullptr);
}

TEST_CASE("Node::get() returns the correct node")
{
    Node node;
    Node inner;
    node.put('a', &inner);
    REQUIRE(node.get('a') == &inner);
}

TEST_CASE("Node::hasChildren() returns true when a node has children")
{
    Node node;
    node.put('a', new Node());
    REQUIRE(node.hasChildren());
}

TEST_CASE("Node::hasChildren() returns false when a node has no children")
{
    Node node;
    REQUIRE_FALSE(node.hasChildren());
}