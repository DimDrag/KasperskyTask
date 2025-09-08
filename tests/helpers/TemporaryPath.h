#ifndef TEMPORARY_PATH_H
#define TEMPORARY_PATH_H

#include <string>
#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif

///
/// \brief Базовый класс для временной сущности файловой системы (папка/файл)
/// По выходу из области видимости удаляет временную сущность
///
class TemporaryPath {
public:
    TemporaryPath(const std::string &path);
    TemporaryPath(const TemporaryPath &other) = delete;
    TemporaryPath& operator=(const TemporaryPath& other) = delete;
    virtual ~TemporaryPath();
    /// \brief Возвращает путь к сущности
    std::string getPath();
    /// \brief Блокирует чтение сущности
    virtual bool blockRead();
#if defined(_WIN32) || defined(_WIN64)
    /// \brief Задаёт HANDLE (только Windows)
    void setHandle(HANDLE handle);
    /// \brief Закрывает HANDLE (только Windows)
    void closeHandle();
#endif
private:
    std::string m_path; //!< путь к сущности
#if defined(_WIN32) || defined(_WIN64)
    HANDLE m_hPath{}; //!< виндовый дескриптор файла для блокирования чтения/записи
    bool m_handleOpened = false; //!< дескриптор инициализирован
#endif
};

#endif // TEMPORARY_PATH_H
