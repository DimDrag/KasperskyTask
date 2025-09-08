#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include <chrono>
#include <vector>
#include <map>
#include <optional>
#include <filesystem>

#include "MalwareHashBase.h"
#include "Logger.h"

enum class FolderToExamineError {
    Ok,
    NoSuchPath,
    NotADirectory,
    NoAccess
};

enum class ProcessResult {
    Ok,
    CheckFailed,
    ScanCompletedWithErrors
};

const std::map<FolderToExamineError, std::string_view> SCANNER_ERROR_MESSAGES {
    {FolderToExamineError::Ok,            "Folder is ok"},
    {FolderToExamineError::NoSuchPath,    "The specified directory path does not exist"},
    {FolderToExamineError::NotADirectory, "The specified path is not a directory"},
    {FolderToExamineError::NoAccess,      "No access to directory"},
};

using Duration = std::chrono::duration<double, std::milli>;

class Scanner {
public:
    struct Metrics {
        int      processedFiles;     //!< количество обработанных файлов
        int      analysisErrorFiles; //!< количество ошибок анализа файлов
        int      malwareFiles;       //!< количество обнаруженных вредоносных файлов
        Duration duration;           //!< время выполнения
    };
    Scanner();
    Scanner(const std::string &folderPath, const std::string &hashBase, const std::string &logfile);
    void setFolderToExaminePath(const std::string &folderPath);
    void setHashBasePath(const std::string &hashBase);
    void setLogFile(const std::string &logfile);
    FolderToExamineError checkFolderToExamine() const noexcept;
    bool check();
    std::vector<std::string> getLastCheckErrors() const;
    std::vector<std::string> getLastScanErrors() const;
    std::vector<std::pair<std::string, std::optional<std::string>>> getLastScanResults() const;
    ProcessResult process();
    Metrics getProcessMetrics() const;
private:
    bool processDirectory(const std::filesystem::path& directoryPath);
    bool processFile(const std::filesystem::path &filePath);
private:
    std::string     m_folderPathToExamine; //!< путь к папке, в которой будут вычислятся хэши
    MalwareHashBase m_hashBase;            //!< объект базы
    Logger          m_logger;              //!< объект логгера
    Metrics         m_metrics{};           //!< краткая информация о проведённом сканировании
    std::vector<std::string> m_lastCheckErrors; //!< список ошибок последней проверки
    std::vector<std::string> m_lastScanErrors;  //!< список ошибок последнего сканирования
    std::vector<std::pair<std::string, std::optional<std::string>>> m_lastScanResults; //!< результаты последнего сканирования (путь-вердикт)
};

#endif // SCANNER_H
