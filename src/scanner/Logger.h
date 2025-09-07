#ifndef LOGGER_H
#define LOGGER_H

#include <map>
#include <string>
#include <fstream>
#include <string_view>

#include "utils/Clock.h"

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

class Logger {
public:
    Logger();
    explicit Logger(const std::string& logPath);
    ~Logger();
    void setLogPath(const std::string& logPath);
    LogError checkLogFile() const noexcept;
    void startLogging();
    void endLogging();
    void log(std::string_view pathToFile, std::string_view hash,
             std::string_view verdict,
             std::string_view time = Kaspersky::Clock::getTimeNow());
private:
    void printLogHeader();
    void printLogBottom();
    void writeString(std::string_view str);
private:
    std::string   m_logPath;       //!< путь к файлу лога
    std::ofstream m_logFile;       //!< файл лога
    bool m_startedLogging = false; //!< логирование началось
    bool m_loggedSmth     = false; //!< что-то залогировалось
};

#endif // LOGGER_H
