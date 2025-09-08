#ifndef TESTS_HELPERS_TEMPORARYFILE_H_
#define TESTS_HELPERS_TEMPORARYFILE_H_

#include <string>

#include "TemporaryPath.h"

///
/// \brief Создаёт временный файл.
/// По выходу из области видимости удаляет файл
///
class TemporaryFile : public TemporaryPath {
public:
    explicit TemporaryFile(const std::string &path, const std::string &content = "smth");
    /// \brief Блокировать чтение файла
    bool blockRead() override;
    /// \brief Блокировать запись в файл
    bool blockWrite();
    /// \brief Перезаписывает содержимое файла
    void rewriteContent(const std::string &content);
private:
    bool m_specialModeOn = false; //!< специальный режим установлен для этого файла
};

#endif // TESTS_HELPERS_TEMPORARYFILE_H_
