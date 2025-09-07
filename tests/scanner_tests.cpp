#include "tests.h"

TEST(Scanner, OpenPath) {
    Scanner scanner;

    scanner.setFolderToExaminePath("");
    EXPECT_EQ(scanner.checkFolderToExamine(), FolderToExamineError::NoSuchPath);

    scanner.setFolderToExaminePath(BASE_PATH);
    EXPECT_EQ(scanner.checkFolderToExamine(), FolderToExamineError::NotADirectory);

    TemporaryDirectory noRead(TEST_DATA_DIR"/test_no_read_dir");
    noRead.makeInaccessible();
    scanner.setFolderToExaminePath(noRead.getPath());
    EXPECT_EQ(scanner.checkFolderToExamine(), FolderToExamineError::NoAccess);

    TemporaryDirectory dirOk(TEST_DATA_DIR"/test_dir_ok");
    scanner.setFolderToExaminePath(dirOk.getPath());
    EXPECT_EQ(scanner.checkFolderToExamine(), FolderToExamineError::Ok);
}

TEST(Scanner, Scan) {
    Scanner scanner(DIR_WITH_TEST_FILES,
                    BASE_PATH,
                    LOG_PATH);

    TemporaryFile noReadFile1(std::string(DIR_WITH_TEST_FILES) + "no_read_file1.txt");
    noReadFile1.makeInaccessible();
    TemporaryFile noReadFile2(std::string(DIR_WITH_TEST_FILES) + "no_read_file2.txt");
    noReadFile2.makeInaccessible();

    bool ok = scanner.process();
    EXPECT_EQ(ok, true);
    if (ok) {
        auto metrics = scanner.getProcessMetrics();
        EXPECT_EQ(metrics.processedFiles, 17);
        EXPECT_EQ(metrics.malwareFiles, 4);
        EXPECT_EQ(metrics.analysisErrorFiles, 2);
    }
}
