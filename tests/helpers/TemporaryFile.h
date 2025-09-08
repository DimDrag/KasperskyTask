#ifndef TEMPORARY_FILE_H
#define TEMPORARY_FILE_H

#include "TemporaryPath.h"

///
/// \brief Создаёт временный файл.
/// По выходу из области видимости удаляет файл
///
class TemporaryFile : public TemporaryPath {
public:
    TemporaryFile(const std::string &path, const std::string &content = "smth");
    /// \brief Блокировать чтение файла
    bool blockRead() override;
    /// \brief Блокировать запись в файл
    bool blockWrite();
    /// \brief Перезаписывает содержимое файла
    void rewriteContent(const std::string &content);
private:
    bool m_specialModeOn = false; //!< специальный режим установлен для этого файла
};

#endif // TEMPORARY_FILE_H
