#include <gtest/gtest.h>
#include <filesystem>
#include <string>
#include <thread>
#include <chrono>

#include "bitbarrel.h"

// Test fixture to handle setup and teardown of the test directory
class BitBarrelTest : public ::testing::Test {
protected:
    const std::string test_dir = "test_data_dir";

    void SetUp() override {
        // Ensure a clean slate before each test
        if (std::filesystem::exists(test_dir)) {
            std::filesystem::remove_all(test_dir);
        }
    }

    void TearDown() override {
        // Clean up the test directory after each test
        if (std::filesystem::exists(test_dir)) {
            std::filesystem::remove_all(test_dir);
        }
    }
};

// 1. Test basic Set and Get operations
TEST_F(BitBarrelTest, BasicSetGet) {
    BitBarrel db(test_dir);

    EXPECT_EQ(db.set("key1", "value1"), Status::Ok);
    EXPECT_EQ(db.set("key2", "value2"), Status::Ok);

    auto res1 = db.get("key1");
    ASSERT_TRUE(res1.isOk());
    EXPECT_EQ(res1.value.value(), "value1");

    auto res2 = db.get("key2");
    ASSERT_TRUE(res2.isOk());
    EXPECT_EQ(res2.value.value(), "value2");
}

// 2. Test updating an existing key
TEST_F(BitBarrelTest, UpdateExistingKey) {
    BitBarrel db(test_dir);

    db.set("update_key", "old_value");
    
    auto res_old = db.get("update_key");
    ASSERT_TRUE(res_old.isOk());
    EXPECT_EQ(res_old.value.value(), "old_value");

    // Overwrite the key
    db.set("update_key", "new_value");
    
    auto res_new = db.get("update_key");
    ASSERT_TRUE(res_new.isOk());
    EXPECT_EQ(res_new.value.value(), "new_value");
}

// 3. Test retrieving a non-existent key
TEST_F(BitBarrelTest, GetNonExistentKey) {
    BitBarrel db(test_dir);

    auto res = db.get("missing_key");
    EXPECT_FALSE(res.isOk());
    // Assuming Status::NotFound is an enum available for comparison
    // EXPECT_EQ(res.status, Status::NotFound); 
}

// 4. Test Persistence and Crash Recovery (Rebuilding the KeyDir)
TEST_F(BitBarrelTest, PersistenceAndRecovery) {
    // Scope limits the lifetime of db1
    {
        BitBarrel db1(test_dir);
        db1.set("persist_1", "data_A");
        db1.set("persist_2", "data_B");
        db1.set("persist_1", "data_A_updated"); // Update to test timestamp logic
    } 
    // db1 goes out of scope, files are closed, threads joined

    // Create a new instance pointing to the same directory
    BitBarrel db2(test_dir);

    auto res1 = db2.get("persist_1");
    ASSERT_TRUE(res1.isOk());
    EXPECT_EQ(res1.value.value(), "data_A_updated"); // Should read the newest value

    auto res2 = db2.get("persist_2");
    ASSERT_TRUE(res2.isOk());
    EXPECT_EQ(res2.value.value(), "data_B");
}

// 5. Test manual Compaction functionality
TEST_F(BitBarrelTest, CompactionLogic) {
    BitBarrel db(test_dir);

    // Write a lot of stale data
    for (int i = 0; i < 50; ++i) {
        db.set("counter", std::to_string(i));
    }

    // Force a write to a new key to ensure we have valid data mixed in
    db.set("steady_key", "steady_value");

    // Give the active segment a moment to register, then trigger compaction
    db.compact();

    // Verify data integrity post-compaction
    auto res1 = db.get("counter");
    ASSERT_TRUE(res1.isOk());
    EXPECT_EQ(res1.value.value(), "49"); // Only the last iteration should survive

    auto res2 = db.get("steady_key");
    ASSERT_TRUE(res2.isOk());
    EXPECT_EQ(res2.value.value(), "steady_value");
}

// 6. Test high volume inserts (Testing segment rotation)
TEST_F(BitBarrelTest, HighVolumeInserts) {
    BitBarrel db(test_dir);

    const int num_keys = 10000;
    
    // Insert
    for (int i = 0; i < num_keys; ++i) {
        db.set("key_" + std::to_string(i), "val_" + std::to_string(i));
    }

    // Retrieve a sample of them
    auto res_first = db.get("key_0");
    ASSERT_TRUE(res_first.isOk());
    EXPECT_EQ(res_first.value.value(), "val_0");

    auto res_mid = db.get("key_5000");
    ASSERT_TRUE(res_mid.isOk());
    EXPECT_EQ(res_mid.value.value(), "val_5000");

    auto res_last = db.get("key_9999");
    ASSERT_TRUE(res_last.isOk());
    EXPECT_EQ(res_last.value.value(), "val_9999");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}