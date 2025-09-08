#ifndef SRC_SCANNER_LOGGER_H_
#define SRC_SCANNER_LOGGER_H_

#include <map>
#include <string>
#include <fstream>
#include <string_view>
#include <mutex>

#include "Clock.h"

enum class LogError {
    Ok,
    NoSuchPath,
    NotAFile,
    NoPermissions
};

const std::map<LogError, std::string_view> LOG_ERROR_MESSAGES {
    {LogError::Ok,            "Log file is ok"},
    {LogError::NoSuchPath,    "The specified log file path does not exist"},
    {LogError::NotAFile,      "The specified log file path is not a file"},
    {LogError::NoPermissions, "No permissions to write the log file"}
};

///
/// \brief Класс, отвечающий за логирование
///
class Logger {
public:
    Logger();
    explicit Logger(const std::string& logPath);
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    /// \brief Установка пути к файлу лога
    void setLogPath(const std::string& logPath);
    /// \brief Проверка файла лога
    /// \return Код ошибки
    LogError checkLogFile() const noexcept;
    /// \brief Начать логгирование
    /// Автоматически вызывается при первом логе
    void startLogging();
    /// \brief Закончить логгирование
    /// Автоматически вызывается при уничтожении объекта
    void endLogging();
    /// \brief Записать лог
    void log(std::string_view pathToFile, std::string_view hash,
             std::string_view verdict,
             std::string_view time = Kaspersky::Clock::getTimeNow());
private:
    /// \brief Печать шапки в файл лога
    void printLogHeader();
    /// \brief Печать закрывашки в файл лога
    void printLogBottom();
    /// \brief Печать строки в файл лога
    void writeString(std::string_view str);
private:
    std::string   m_logPath;       //!< путь к файлу лога
    std::ofstream m_logFile;       //!< файл лога
    bool m_startedLogging = false; //!< логирование началось
    bool m_loggedSmth     = false; //!< что-то залогировалось
    mutable std::mutex m_mutex;    //!< мьютекс для синхронизации
};

#endif // SRC_SCANNER_LOGGER_H_
