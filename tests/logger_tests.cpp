#include <fstream>
#include <string_view>
#include <array>

#include "tests.h"

constexpr std::array<std::string_view, 6> EXPECTED_LOG_RESULT_1 = {
    "----------------Scanner log start----------------",
    "10:00:00",
    "File path: filepath.txt",
    "MD5 hash:  hash",
    "Verdict:   verdict",
    "-----------------Scanner log end-----------------"
};

constexpr std::array<std::string_view, 16> EXPECTED_LOG_RESULT_2 = {
    "----------------Scanner log start----------------",
    "10:00:01",
    "File path: filepath1.txt",
    "MD5 hash:  hash1",
    "Verdict:   verdict1",
    "-------------------------------------------------",
    "10:00:02",
    "File path: filepath2.txt",
    "MD5 hash:  hash2",
    "Verdict:   verdict2",
    "-------------------------------------------------",
    "10:00:03",
    "File path: filepath3.txt",
    "MD5 hash:  hash3",
    "Verdict:   verdict3",
    "-----------------Scanner log end-----------------"
};

constexpr std::array<std::string_view, 2> EXPECTED_LOG_RESULT_3 = {
    "----------------Scanner log start----------------",
    "-----------------Scanner log end-----------------"
};

TEST(Logger, Open) {
    Logger logger;

    logger.setLogPath("");
    EXPECT_EQ(logger.checkLogFile(), LogError::NoSuchPath);

    logger.setLogPath(DIR_WITH_TEST_FILES);
    EXPECT_EQ(logger.checkLogFile(), LogError::NotAFile);

    TemporaryFile noWrite(TEST_DATA_DIR"/test_no_write.log");
    noWrite.blockWrite();
    logger.setLogPath(noWrite.getPath());
    EXPECT_EQ(logger.checkLogFile(), LogError::NoPermissions);

    TemporaryFile logOk(TEST_DATA_DIR"/test_log_ok.log");
    logger.setLogPath(logOk.getPath());
    EXPECT_EQ(logger.checkLogFile(), LogError::Ok);
}

TEST(Logger, Log) {
    {
        Logger logger(LOG_PATH);
        logger.log("filepath.txt", "hash", "verdict", "10:00:00");
        logger.endLogging();

        std::ifstream logFile(LOG_PATH);
        std::string line;
        int i = 0;
        while (std::getline(logFile, line) && i < EXPECTED_LOG_RESULT_1.size()) {
            EXPECT_EQ(line, EXPECTED_LOG_RESULT_1[i++]);
        }
        EXPECT_EQ(i, EXPECTED_LOG_RESULT_1.size());
    }
    {
        Logger logger(LOG_PATH);
        logger.startLogging();
        logger.log("filepath1.txt", "hash1", "verdict1", "10:00:01");
        logger.log("filepath2.txt", "hash2", "verdict2", "10:00:02");
        logger.log("filepath3.txt", "hash3", "verdict3", "10:00:03");
        logger.endLogging();

        std::ifstream logFile(LOG_PATH);
        std::string line;
        int i = 0;
        while (std::getline(logFile, line) && i < EXPECTED_LOG_RESULT_2.size()) {
            EXPECT_EQ(line, EXPECTED_LOG_RESULT_2[i++]);
        }
        EXPECT_EQ(i, EXPECTED_LOG_RESULT_2.size());
    }
    {
        Logger logger(LOG_PATH);
        logger.startLogging();
        logger.endLogging();

        std::ifstream logFile(LOG_PATH);
        std::string line;
        int i = 0;
        while (std::getline(logFile, line) && i < EXPECTED_LOG_RESULT_3.size()) {
            EXPECT_EQ(line, EXPECTED_LOG_RESULT_3[i++]);
        }
        EXPECT_EQ(i, EXPECTED_LOG_RESULT_3.size());
    }
}
