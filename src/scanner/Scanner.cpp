#include "Scanner.h"

#include <filesystem>

#include "MD5FileHasher.h"
#include "Hash.h"

Scanner::Scanner() { }

Scanner::Scanner(const std::string &folderPath,
                 const std::string &hashBase,
                 const std::string &logfile) :
    m_folderPathToExamine(folderPath),
    m_hashBase(hashBase),
    m_logger(logfile) { }

void Scanner::setFolderToExaminePath(const std::string &folderPath) {
    m_folderPathToExamine = folderPath;
}

void Scanner::setHashBasePath(const std::string &hashBase) {
    m_hashBase.setBasePath(hashBase);
}

void Scanner::setLogFile(const std::string &logfile) {
    m_logger.setLogPath(logfile);
}

FolderToExamineError Scanner::checkFolderToExamine() const noexcept {
    if (!std::filesystem::exists(m_folderPathToExamine))
        return FolderToExamineError::NoSuchPath;
    if (!std::filesystem::is_directory(m_folderPathToExamine))
        return FolderToExamineError::NotADirectory;
    try {
        std::filesystem::directory_iterator it(m_folderPathToExamine);
    } catch (const std::filesystem::filesystem_error& e) {
        return FolderToExamineError::NoAccess;
    }
    return FolderToExamineError::Ok;
}

bool Scanner::check() {
    m_lastCheckErrors.clear();
    bool errorOccured = false;
    if (auto err = m_hashBase.checkBaseFile(); err != BaseError::Ok) {
        m_lastCheckErrors.push_back(std::string(BASE_ERROR_MESSAGES.at(err)));
        errorOccured = true;
    }
    if (auto err = m_logger.checkLogFile(); err != LogError::Ok) {
        m_lastCheckErrors.push_back(std::string(LOG_ERROR_MESSAGES.at(err)));
        errorOccured = true;
    }
    if (auto err = checkFolderToExamine(); err != FolderToExamineError::Ok) {
        m_lastCheckErrors.push_back(std::string(SCANNER_ERROR_MESSAGES.at(err)));
        errorOccured = true;
    }
    return !errorOccured;
}

std::vector<std::string> Scanner::getLastCheckErrors() const {
    return m_lastCheckErrors;
}

std::vector<std::string> Scanner::getLastScanErrors() const {
    return m_lastScanErrors;
}

std::vector<std::pair<std::string, std::optional<std::string>>> Scanner::getLastScanResults() const {
    return m_lastScanResults;
}

Scanner::Metrics Scanner::getProcessMetrics() const {
    return m_metrics;
}

ProcessResult Scanner::process() {
    m_metrics = {};
    m_lastScanResults.clear();
    m_lastScanErrors.clear();
    const auto timeStart = std::chrono::high_resolution_clock::now();

    if (!check())
        return ProcessResult::CheckFailed;
    m_hashBase.indexBaseFileContent();
    bool processedSuccesfully = processDirectoryMultiThread(std::filesystem::path(m_folderPathToExamine));

    const auto timeEnd = std::chrono::high_resolution_clock::now();
    m_metrics.duration = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart);

    if (!processedSuccesfully)
        return ProcessResult::ScanCompletedWithErrors;

    return ProcessResult::Ok;
}

void Scanner::workerThread() {
    while (true) {
        std::filesystem::path filePath;
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_cv.wait(lock, [&] {
                return m_queueFormingDone || !m_fileQueue.empty();
            });

            if (m_queueFormingDone && m_fileQueue.empty()) {
                return; // очередь пуста и больше нет задач
            }

            filePath = std::move(m_fileQueue.front());
            m_fileQueue.pop();
        }
        processFileThreadSafe(filePath);
    }
}

bool Scanner::processFileThreadSafe(const std::filesystem::path &filePath) {
    if (auto hash = MD5FileHasher::fromFile(filePath.string()); hash) {
        auto verdict = m_hashBase.findIndexedHashVerdict(Kaspersky::Hash::fromString(hash.value()));
        {
            std::lock_guard<std::mutex> lock(m_resultsMutex);
            m_lastScanResults.push_back({filePath.string(), verdict});
        }

        if (verdict) {
            m_logger.log(filePath.string(), hash.value(), verdict.value());
        }

        {
            std::lock_guard<std::mutex> lock(m_metricsMutex);
            m_metrics.processedFiles++;
            if (verdict) {
                m_metrics.malwareFiles++;
            }
        }
        return true;
    }

    {
        std::lock_guard<std::mutex> lock(m_metricsMutex);
        m_metrics.processedFiles++;
        m_metrics.analysisErrorFiles++;
    }
    return false;
}

void Scanner::addFileToProcessQueue(const std::filesystem::path& filePath) {
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_fileQueue.push(filePath);
    }
    m_cv.notify_one();
}

bool Scanner::addFilesInDirectoryToProcessQueue(const std::filesystem::path& directoryPath) {
    // проверяем, доступна ли директория для чтения
    std::filesystem::directory_iterator it;
    try {
        it = std::filesystem::directory_iterator(directoryPath);
    } catch (const std::filesystem::filesystem_error& e) {
        std::lock_guard<std::mutex> lock(m_errorsMutex);
        m_lastScanErrors.push_back(directoryPath.string() + " is not readable");
        return false;
    }
    // рекурсивно обходим все элементы в директории
    bool ok = true;
    for (const auto& entry : it) {
        if (std::filesystem::is_directory(entry)) {
            ok &= addFilesInDirectoryToProcessQueue(entry.path());
        } else {
            addFileToProcessQueue(entry.path().lexically_normal().string());
        }
    }
    return ok;
}

bool Scanner::processDirectoryMultiThread(const std::filesystem::path& directoryPath) {
    // запуск потоков обработки
    const unsigned int threadCount = std::max(1u, std::thread::hardware_concurrency());
    std::vector<std::thread> workers;
    workers.reserve(threadCount);
    for (unsigned int i = 0; i < threadCount; ++i) {
        workers.emplace_back(&Scanner::workerThread, this);
    }

    // рекурсивный обход
    bool errorsOccured = addFilesInDirectoryToProcessQueue(directoryPath);

    // сигнал завершения (больше файлов в очередь не добавится)
    m_queueFormingDone = true;
    m_cv.notify_all();

    for (auto& w : workers) {
        w.join();
    }

    return errorsOccured;
}


