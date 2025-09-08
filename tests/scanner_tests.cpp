#include <map>
#include <filesystem>

#include "tests.h"

namespace fs = std::filesystem;

const std::string DIR_WITH_TEST_FILES_STR = std::string(DIR_WITH_TEST_FILES);

const std::map<fs::path, std::optional<std::string>> EXPECTED_SCAN_RESULT = {
    std::make_pair(fs::path(DIR_WITH_TEST_FILES_STR + "dummy1.txt").make_preferred(), "Verdict8"),
    std::make_pair(fs::path(DIR_WITH_TEST_FILES_STR + "dummy1_copy.txt").make_preferred(), "Verdict8"),
    std::make_pair(fs::path(DIR_WITH_TEST_FILES_STR + "dummy2.txt").make_preferred(), std::nullopt),
    std::make_pair(fs::path(DIR_WITH_TEST_FILES_STR + "dummy3.txt").make_preferred(), std::nullopt),
    std::make_pair(fs::path(DIR_WITH_TEST_FILES_STR + "dummy4.txt").make_preferred(), std::nullopt),
    std::make_pair(fs::path(DIR_WITH_TEST_FILES_STR + "subdir1/subdir1dummy1.txt").make_preferred(), std::nullopt),
    std::make_pair(fs::path(DIR_WITH_TEST_FILES_STR + "subdir1/subdir1dummy2.txt").make_preferred(), std::nullopt),
    std::make_pair(fs::path(DIR_WITH_TEST_FILES_STR + "subdir1/subdir1dummy3.txt").make_preferred(), std::nullopt),
    std::make_pair(fs::path(DIR_WITH_TEST_FILES_STR + "subdir1/subdir1dummy4.txt").make_preferred(), std::nullopt),
    std::make_pair(fs::path(DIR_WITH_TEST_FILES_STR + "subdir1/subsubdir1/subsubdir1dummy1.txt").make_preferred(), "Verdict2"),
    std::make_pair(fs::path(DIR_WITH_TEST_FILES_STR + "subdir1/subsubdir1/subsubdir1dummy2.txt").make_preferred(), std::nullopt),
    std::make_pair(fs::path(DIR_WITH_TEST_FILES_STR + "subdir2/subdir2dummy1.txt").make_preferred(), std::nullopt),
    std::make_pair(fs::path(DIR_WITH_TEST_FILES_STR + "subdir2/subdir2dummy2.txt").make_preferred(), std::nullopt),
    std::make_pair(fs::path(DIR_WITH_TEST_FILES_STR + "subdir2/subdir2dummy3.txt").make_preferred(), "Verdict3"),
    std::make_pair(fs::path(DIR_WITH_TEST_FILES_STR + "subdir2/subdir2dummy4.txt").make_preferred(), std::nullopt),
};

TEST(Scanner, OpenPath) {
    Scanner scanner;

    scanner.setFolderToExaminePath("");
    EXPECT_EQ(scanner.checkFolderToExamine(), FolderToExamineError::NoSuchPath);

    scanner.setFolderToExaminePath(BASE_PATH);
    EXPECT_EQ(scanner.checkFolderToExamine(), FolderToExamineError::NotADirectory);

    TemporaryDirectory noRead(TEST_DATA_DIR"/test_no_read_dir");
    noRead.blockRead();
    scanner.setFolderToExaminePath(noRead.getPath());
    EXPECT_EQ(scanner.checkFolderToExamine(), FolderToExamineError::NoAccess);

    TemporaryDirectory dirOk(TEST_DATA_DIR"/test_dir_ok");
    scanner.setFolderToExaminePath(dirOk.getPath());
    EXPECT_EQ(scanner.checkFolderToExamine(), FolderToExamineError::Ok);
}


TEST(Scanner, ScanCheckFailed) {
    Scanner scanner("", "", "");

    ProcessResult res = scanner.process();
    EXPECT_EQ(res, ProcessResult::CheckFailed);
}

TEST(Scanner, ScanWithErrors) {
    Scanner scanner(DIR_WITH_TEST_FILES,
                    BASE_PATH,
                    LOG_PATH);

    TemporaryFile noReadFile1(DIR_WITH_TEST_FILES_STR + "no_read_file1.txt");
    noReadFile1.blockRead();
    TemporaryFile noReadFile2(DIR_WITH_TEST_FILES_STR + "no_read_file2.txt");
    noReadFile2.blockRead();
    TemporaryDirectory noReadDir(DIR_WITH_TEST_FILES_STR  + "test_no_read_dir");
    noReadDir.blockRead();

    ProcessResult res = scanner.process();
    EXPECT_EQ(res, ProcessResult::ScanCompletedWithErrors);
    auto metrics = scanner.getProcessMetrics();
    EXPECT_EQ(metrics.processedFiles, 17);
    EXPECT_EQ(metrics.malwareFiles, 4);
    EXPECT_EQ(metrics.analysisErrorFiles, 2);
    auto scanResults = scanner.getLastScanResults();
    EXPECT_EQ(scanResults.size(), EXPECTED_SCAN_RESULT.size());
    for (const auto &[path, verdict] : scanResults) {
        EXPECT_EQ(EXPECTED_SCAN_RESULT.at(fs::path(path).make_preferred()), verdict);
    }
}

TEST(Scanner, ScanOk) {
    Scanner scanner(DIR_WITH_TEST_FILES,
                    BASE_PATH,
                    LOG_PATH);

    TemporaryFile noReadFile1(DIR_WITH_TEST_FILES_STR + "no_read_file1.txt");
    noReadFile1.blockRead();
    TemporaryFile noReadFile2(DIR_WITH_TEST_FILES_STR + "no_read_file2.txt");
    noReadFile2.blockRead();

    ProcessResult res = scanner.process();
    EXPECT_EQ(res, ProcessResult::Ok);
    auto metrics = scanner.getProcessMetrics();
    EXPECT_EQ(metrics.processedFiles, 17);
    EXPECT_EQ(metrics.malwareFiles, 4);
    EXPECT_EQ(metrics.analysisErrorFiles, 2);
    auto scanResults = scanner.getLastScanResults();
    EXPECT_EQ(scanResults.size(), EXPECTED_SCAN_RESULT.size());
    for (const auto &[path, verdict] : scanResults) {
        EXPECT_EQ(EXPECTED_SCAN_RESULT.at(fs::path(path).make_preferred()), verdict);
    }
}
