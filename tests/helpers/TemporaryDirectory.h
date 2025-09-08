#ifndef TESTS_HELPERS_TEMPORARYDIRECTORY_H_
#define TESTS_HELPERS_TEMPORARYDIRECTORY_H_

#include <string>

#include "TemporaryPath.h"

///
/// \brief Создаёт временную директорию.
/// По выходу из области видимости удаляет директорию
///
class TemporaryDirectory : public TemporaryPath {
public:
    explicit TemporaryDirectory(const std::string &path);
    /// \brief Блокировать чтение директории
    bool blockRead() override;
};

#endif // TESTS_HELPERS_TEMPORARYDIRECTORY_H_
