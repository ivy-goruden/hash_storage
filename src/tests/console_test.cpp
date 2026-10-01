#include "classes/hash_map.hpp"
#include "classes/red_black_tree.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <chrono>
#include <map>
#include <random>
#include <set>
#include <string>
#include <vector>

namespace {

template <typename StorageType>
void expectCommonOperations(StorageType& storage) {
    ASSERT_TRUE(storage.set("alpha", 10));
    EXPECT_FALSE(storage.set("alpha", 99));
    ASSERT_TRUE(storage.set("beta", 20));

    auto value = storage.get("alpha");
    ASSERT_TRUE(value.ok());
    EXPECT_EQ(value.value(), 10);
    EXPECT_TRUE(storage.exists("beta"));
    EXPECT_FALSE(storage.exists("missing"));

    EXPECT_TRUE(storage.update("alpha", 15));
    EXPECT_FALSE(storage.update("missing", 0));
    EXPECT_EQ(storage.get("alpha").value(), 15);

    auto renamed = storage.rename("beta", "gamma");
    ASSERT_TRUE(renamed.ok());
    EXPECT_TRUE(renamed.value());
    EXPECT_FALSE(storage.exists("beta"));
    EXPECT_EQ(storage.get("gamma").value(), 20);
    EXPECT_FALSE(storage.rename("alpha", "gamma").ok());
    EXPECT_TRUE(storage.rename("alpha", "alpha").ok());

    auto matches = storage.find(15);
    ASSERT_EQ(matches.size(), 1u);
    EXPECT_EQ(matches.front(), "alpha");

    EXPECT_TRUE(storage.del("gamma"));
    EXPECT_FALSE(storage.del("gamma"));
    EXPECT_FALSE(storage.get("gamma").ok());

    const auto keyList = storage.keys();
    const std::set<std::string> keys(keyList.begin(), keyList.end());
    EXPECT_EQ(keys, (std::set<std::string>{"alpha"}));
}

template <typename StorageType>
void expectExpirationOperations(StorageType& storage) {
    const auto now = s21::TimePoint::clock::now();
    ASSERT_TRUE(storage.set("expired", 1, now - std::chrono::seconds(1)));
    ASSERT_TRUE(storage.set("live", 2, now + std::chrono::hours(1)));
    ASSERT_TRUE(storage.set("persistent", 3));

    EXPECT_TRUE(storage.TTL("live").ok());
    EXPECT_FALSE(storage.TTL("persistent").ok());

    storage.PurgeExpired();
    EXPECT_FALSE(storage.exists("expired"));
    EXPECT_TRUE(storage.exists("live"));
    EXPECT_TRUE(storage.exists("persistent"));
}

TEST(HashMapTest, SupportsCommonStorageOperations) {
    Hash_Map<int> storage;
    expectCommonOperations(storage);
}

TEST(RedBlackTreeTest, SupportsCommonStorageOperations) {
    s21::RedBlackTree<int> storage;
    expectCommonOperations(storage);
}

TEST(HashMapTest, PurgesExpiredRecordsAndRetainsLiveRecords) {
    Hash_Map<int> storage;
    expectExpirationOperations(storage);
}

TEST(RedBlackTreeTest, PurgesExpiredRecordsAndRetainsLiveRecords) {
    s21::RedBlackTree<int> storage;
    expectExpirationOperations(storage);
}

TEST(HashMapTest, RehashPreservesAllEntries) {
    Hash_Map<int> storage;
    constexpr int keyCount = 250;

    for (int key = 0; key < keyCount; ++key) {
        EXPECT_TRUE(storage.set(std::to_string(key), key));
    }
    for (int key = 0; key < keyCount; ++key) {
        auto value = storage.get(std::to_string(key));
        ASSERT_TRUE(value.ok());
        EXPECT_EQ(value.value(), key);
    }
}

TEST(RedBlackTreeTest, RandomizedInsertionsAndDeletionsPreserveContents) {
    s21::RedBlackTree<int> storage;
    std::map<std::string, int> expected;
    std::vector<int> keys(300);
    for (int key = 0; key < static_cast<int>(keys.size()); ++key) keys[key] = key;

    std::mt19937 random(2026);
    std::shuffle(keys.begin(), keys.end(), random);
    for (int key : keys) {
        const auto text = std::to_string(key);
        ASSERT_TRUE(storage.set(text, key));
        expected.emplace(text, key);
    }

    std::shuffle(keys.begin(), keys.end(), random);
    for (int key : keys) {
        const auto text = std::to_string(key);
        ASSERT_TRUE(storage.del(text));
        expected.erase(text);

        std::map<std::string, int> actual;
        storage.ForEach([&](s21::Node<int>& node) {
            actual.emplace(node.getKey(), node.getValue());
        });
        EXPECT_EQ(actual, expected);
    }
    EXPECT_FALSE(storage.del("missing"));
}

}  // namespace