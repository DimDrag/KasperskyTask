#include "tests.h"
#include <vector>

const std::vector<std::pair<std::string, std::string>> TO_FIND = {
    {"babadbf09a7f2244e5c6fce978ae7d04", "Verdict1"},
    {"85ae166ba48f1e7792960d5598686279", "Verdict2"},
    {"01c9407137cf3d37d329c30c74934504", "Verdict3"},
    {"23bea5327c302ac05cfa231422804cbb", "Verdict4"},
    {"6f6c4849e78bfd101d6671075592fc8c", "Verdict5"}
};

TEST(Base, Open) {
    MalwareHashBase hashBase;

    hashBase.setBasePath("");
    EXPECT_EQ(hashBase.checkBaseFile(), BaseError::NoSuchPath);

    hashBase.setBasePath(DIR_WITH_TEST_FILES);
    EXPECT_EQ(hashBase.checkBaseFile(), BaseError::NotAFile);

    TemporaryFile wrongExtension(TEST_DATA_DIR"/test_wrong_extension.txt");
    hashBase.setBasePath(wrongExtension.getPath());
    EXPECT_EQ(hashBase.checkBaseFile(), BaseError::WrongExtension);

    TemporaryFile wrongExtension2(TEST_DATA_DIR"/test_wrong_extension.csv.txt");
    hashBase.setBasePath(wrongExtension2.getPath());
    EXPECT_EQ(hashBase.checkBaseFile(), BaseError::WrongExtension);

    TemporaryFile noRead(TEST_DATA_DIR"/test_no_read.csv");
    noRead.makeInaccessible();
    hashBase.setBasePath(noRead.getPath());
    EXPECT_EQ(hashBase.checkBaseFile(), BaseError::NoPermissions);

    TemporaryFile wrongFormat(TEST_DATA_DIR"/test_wrong_format.csv");
    hashBase.setBasePath(wrongFormat.getPath());
    wrongFormat.rewriteContent(";");
    EXPECT_EQ(hashBase.checkBaseFile(), BaseError::WrongFormat);
    wrongFormat.rewriteContent(";Verdict");
    EXPECT_EQ(hashBase.checkBaseFile(), BaseError::WrongFormat);
    wrongFormat.rewriteContent("hash;Verdict");
    EXPECT_EQ(hashBase.checkBaseFile(), BaseError::WrongFormat);
    wrongFormat.rewriteContent("hash;");
    EXPECT_EQ(hashBase.checkBaseFile(), BaseError::WrongFormat);
    wrongFormat.rewriteContent("0000000000000000000000000000000;Verdict");
    EXPECT_EQ(hashBase.checkBaseFile(), BaseError::WrongFormat);
    wrongFormat.rewriteContent("000000000000000000000000000000000;Verdict");
    EXPECT_EQ(hashBase.checkBaseFile(), BaseError::WrongFormat);
    wrongFormat.rewriteContent("00000000000000000000000000000000;");
    EXPECT_EQ(hashBase.checkBaseFile(), BaseError::WrongFormat);
    wrongFormat.rewriteContent("00000000000000000000000000000000;Verdict\n"
                               "hash;verdict");
    EXPECT_EQ(hashBase.checkBaseFile(), BaseError::WrongFormat);

    TemporaryFile baseOk(TEST_DATA_DIR"/test_base_ok.csv");
    hashBase.setBasePath(baseOk.getPath());
    baseOk.rewriteContent("00000000000000000000000000000000;Verdict");
    EXPECT_EQ(hashBase.checkBaseFile(), BaseError::Ok);
    baseOk.rewriteContent("00000000000000000000000000000000;Verdict1\n"
                               "00000000000000000000000000000001;Verdict2\n");
    EXPECT_EQ(hashBase.checkBaseFile(), BaseError::Ok);
    baseOk.rewriteContent("00000000000000000000000000000000;Verdict1\n"
                               "00000000000000000000000000000001;Verdict2");
    EXPECT_EQ(hashBase.checkBaseFile(), BaseError::Ok);
}

TEST(Base, FindHash) {
    MalwareHashBase hashBase(BASE_PATH);
    hashBase.indexBaseFileContent();

    for (auto &[hash, verdict] : TO_FIND) {
        auto findResult = hashBase.findIndexedHashVerdict(hash);
        EXPECT_EQ(findResult.value_or("<not found>"), verdict);
    }
    auto findResult = hashBase.findIndexedHashVerdict("00000000000000000000000000000000");
    EXPECT_EQ(findResult, std::nullopt);
}
