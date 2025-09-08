#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include <chrono>
#include <vector>
#include <map>
#include <optional>
#include <filesystem>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>

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

///
/// \brief Класс для нахождения соответствия хэшей файлов в папке базе "вредоносных" хэшэй
///
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
    /// \brief Задание пути к папке для анализа
    void setFolderToExaminePath(const std::string &folderPath);
    /// \brief Задание пути к файлу базы
    void setHashBasePath(const std::string &hashBase);
    /// \brief Задание пути к файлу лога
    void setLogFile(const std::string &logfile);
    /// \brief Проверка папки для анализа
    /// \return Код ошибки
    FolderToExamineError checkFolderToExamine() const noexcept;
    /// \brief Проверка конфигурации сканера перед запуском
    bool check();
    /// \brief Возвращает строковые ошибки последней проверки
    std::vector<std::string> getLastCheckErrors() const;
    /// \brief Возвращает строковые ошибки последнего сканирования
    std::vector<std::string> getLastScanErrors() const;
    /// \brief Возвращает пары путь-вердикт, определённые во время последнего сканирования
    std::vector<std::pair<std::string, std::optional<std::string>>> getLastScanResults() const;
    /// \brief Запуск сканирования
    /// \return Код результата
    ProcessResult process();
    /// \brief Возвращает метрики последнего сканирования
    Metrics getProcessMetrics() const;
private:
    /// \brief Добавляет файлы содержащиеся в указанной папке в очередь обработки.
    /// Заходит во вложенные папки.
    /// \return Возвращает false, если возникла ошибка во время обхода (например, нет доступа к папке)
    bool addFilesInDirectoryToProcessQueue(const std::filesystem::path& directoryPath);
    /// \brief Добавляет файлв очередь обработки
    void addFileToProcessQueue(const std::filesystem::path& filePath);
    /// \brief Рабочий поток, обрабатывающий файлы в очереди
    void workerThread();
    /// \brief Обрабатывает файл потокобезопасно
    /// \return Возвращает false, если не удалось вычислить хэш файла
    bool processFileThreadSafe(const std::filesystem::path& filePath);
    /// \brief Обрабатывает файлы в указанной директории в многопоточном режиме
    /// \return Возвращает false, если возникла ошибка во время обхода (например, нет доступа к папке)
    bool processDirectoryMultiThread(const std::filesystem::path& directoryPath);
private:
    std::string     m_folderPathToExamine; //!< путь к папке, в которой будут вычислятся хэши
    MalwareHashBase m_hashBase;            //!< объект базы
    Logger          m_logger;              //!< объект логгера
    Metrics         m_metrics{};           //!< краткая информация о проведённом сканировании
    std::vector<std::string> m_lastCheckErrors; //!< список ошибок последней проверки
    std::vector<std::string> m_lastScanErrors;  //!< список ошибок последнего сканирования
    std::vector<std::pair<std::string, std::optional<std::string>>> m_lastScanResults; //!< результаты последнего
                                                                                       //!< сканирования (путь-вердикт)
    // организация многопоточного сканирования файлов
    std::condition_variable m_cv; //!< condition variable для организации пробуждения потока для обработки
    std::queue<std::filesystem::path> m_fileQueue; //!< очередь файлов на обработку
    std::atomic<bool> m_queueFormingDone{false}; //!< атомик для отслеживания окончания формирования
                                                 //!< очереди файлов на обработку
    std::mutex m_queueMutex;   //!< мьютекс для очереди
    std::mutex m_metricsMutex; //!< мьютекс для метрик
    std::mutex m_resultsMutex; //!< мьютекс для результатов сканирования
    std::mutex m_errorsMutex;  //!< мьютекс для ошибок сканирования
};

#endif // SCANNER_H
