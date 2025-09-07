#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include <chrono>

#include "MalwareHashBase.h"
#include "Logger.h"

enum class ScannerError {
    Ok,
    NoSuchPath,
    NotADirectory,
    NoAccess
};

const std::map<ScannerError, std::string_view> SCANNER_ERROR_MESSAGES {
    {ScannerError::Ok,            "Folder is ok"},
    {ScannerError::NoSuchPath,    "The specified directory path does not exist"},
    {ScannerError::NotADirectory, "The specified path is not a directory"},
    {ScannerError::NoAccess,      "No access to directory"},
};

using Duration = std::chrono::duration<double, std::milli>;

class Scanner {
    struct Metrics {
        int processedFiles{};
        int analysisErrorFiles{};
        int malwareFiles{};
        double duration{};
    };
public:
    Scanner();
    Scanner(const std::string &folderPath, const std::string &hashBase, const std::string &logfile);
    void setFolderToExaminePath(std::string &folderPath);
    void setHashBasePath(std::string &hashBase);
    void setLogFile(std::string &logfile);
    ScannerError checkFolderToExamine() const noexcept;
    bool check();
    bool process();
    Metrics getProcessMetrics();
private:
    bool processFile(const std::string &path);
private:
    std::string     m_folderPathToExamine; //!< Путь к папке, в которой будут вычислятся хэши
    MalwareHashBase m_hashBase;            //!< Объект базы
    Logger          m_logger;              //!< Объект логгера

    int    m_processedFiles;     //!< Количество обработанных файлов
    int    m_analysisErrorFiles; //!< Количество ошибок анализа файлов
    int    m_malwareFiles;       //!< Количество обнаруженных вредоносных файлов
    double m_duration;           //!< Время выполнения
};

#endif // SCANNER_H
