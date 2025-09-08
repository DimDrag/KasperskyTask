#include "Logger.h"

#include <filesystem>

constexpr std::string_view LOG_HEADER    = "----------------Scanner log start----------------\n";
constexpr std::string_view LOG_BOTTOM    = "-----------------Scanner log end-----------------\n";
constexpr std::string_view LOG_SEPARATOR = "-------------------------------------------------\n";

Logger::Logger() {}

Logger::Logger(const std::string& logPath) {
    m_logPath = logPath;
}

Logger::~Logger() {
    endLogging();
}

void Logger::setLogPath(const std::string& logPath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    endLogging();
    m_logPath = logPath;
}

LogError Logger::checkLogFile() const noexcept {
    if (!std::filesystem::exists(m_logPath))
        return LogError::NoSuchPath;
    if (std::filesystem::is_directory(m_logPath))
        return LogError::NotAFile;
    if (std::ofstream logFile(m_logPath, std::ios::out);
            !logFile.is_open())
        return LogError::NoPermissions;
    return LogError::Ok;
}

void Logger::printLogHeader() {
    writeString(LOG_HEADER);
}

void Logger::printLogBottom() {
    writeString(LOG_BOTTOM);
}

void Logger::startLogging() {
    if (!m_startedLogging) {
        m_logFile.open(m_logPath, std::ios::out);
        m_startedLogging = true;
        printLogHeader();
    }
}

void Logger::endLogging() {
    if (m_startedLogging) {
        printLogBottom();
        m_logFile.close();
        m_startedLogging = false;
        m_loggedSmth = false;
    }
}

void Logger::log(std::string_view pathToFile, std::string_view hash,
                 std::string_view verdict, std::string_view time) {
    std::lock_guard<std::mutex> lock(m_mutex);

    std::string strToWrite;
    if (m_loggedSmth) {
        strToWrite += LOG_SEPARATOR;
    } else {
        m_loggedSmth = true;
    }
    strToWrite +=
        std::string(time) + "\n" +
        "File path: " + std::string(pathToFile) + "\n" +
        "MD5 hash:  " + std::string(hash)       + "\n" +
        "Verdict:   " + std::string(verdict)    + "\n";
    writeString(strToWrite);
}

void Logger::writeString(std::string_view str) {
    if (!m_startedLogging)
        startLogging();
    m_logFile << str;
}
