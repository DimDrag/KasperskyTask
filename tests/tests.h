#ifndef TESTS_H
#define TESTS_H

#include <gtest/gtest.h>

#include "scanner/MalwareHashBase.h"
#include "scanner/Logger.h"
#include "scanner/Scanner.h"
#include "helpers/TemporaryFile.h"
#include "helpers/TemporaryDirectory.h"

constexpr auto DIR_WITH_TEST_FILES = TEST_DATA_DIR"/dir_to_test/";
constexpr auto BASE_PATH = TEST_DATA_DIR"/base.csv";
constexpr auto LOG_PATH = TEST_DATA_DIR"/report.log";

#endif  // TESTS_H
