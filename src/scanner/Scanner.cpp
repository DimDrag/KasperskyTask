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

bool Scanner::processFile(const std::filesystem::path &filePath) {
    m_metrics.processedFiles++;
    if (auto hash = MD5FileHasher::fromFile(filePath.string()); hash) {
        auto verdict = m_hashBase.findIndexedHashVerdict(
            Kaspersky::Hash::fromString(hash.value()));
        m_lastScanResults.push_back({filePath.string(), verdict});
        if (verdict) {
            m_logger.log(filePath.string(), hash.value(), verdict.value());
            m_metrics.malwareFiles++;
        }
        return true;
    }
    m_metrics.analysisErrorFiles++;
    return false;
}

bool Scanner::processDirectory(const std::filesystem::path& directoryPath) {
    // проверяем, доступна ли директория для чтения
    std::filesystem::directory_iterator it;
    try {
        it = std::filesystem::directory_iterator(directoryPath);
    } catch (const std::filesystem::filesystem_error& e) {
        m_lastScanErrors.push_back(directoryPath.string() + " is not readable");
        return false;
    }
    // рекурсивно обходим все элементы в директории
    bool ok = true;
    for (const auto& entry : it) {
        if (std::filesystem::is_directory(entry)) {
            ok &= processDirectory(entry.path());
        } else {
            processFile(entry.path().lexically_normal().string());
        }
    }
    return ok;
}

ProcessResult Scanner::process() {
    m_metrics = {};
    m_lastScanResults.clear();
    m_lastScanErrors.clear();
    const auto timeStart = std::chrono::high_resolution_clock::now();

    if (!check())
        return ProcessResult::CheckFailed;

    m_hashBase.indexBaseFileContent();

    bool processedSuccesfully = processDirectory(std::filesystem::path(m_folderPathToExamine));
    const auto timeEnd = std::chrono::high_resolution_clock::now();
    m_metrics.duration = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart);

    if (!processedSuccesfully)
        return ProcessResult::ScanCompletedWithErrors;

    return ProcessResult::Ok;
}
