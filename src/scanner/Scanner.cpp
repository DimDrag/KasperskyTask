#include "Scanner.h"

#include <filesystem>
#include <iostream>

#include "MD5FileHasher.h"
#include "Hash.h"

Scanner::Scanner() { }

Scanner::Scanner(const std::string &folderPath,
                 const std::string &hashBase,
                 const std::string &logfile) :
    m_folderPathToExamine(folderPath),
    m_hashBase(hashBase),
    m_logger(logfile) { }

void Scanner::setFolderToExaminePath(std::string &folderPath) {
    m_folderPathToExamine = folderPath;
}

void Scanner::setHashBasePath(std::string &hashBase) {
    m_hashBase.setBasePath(hashBase);
}

void Scanner::setLogFile(std::string &logfile) {
    m_logger.setLogPath(logfile);
}

ScannerError Scanner::checkFolderToExamine() const noexcept {
    if (!std::filesystem::exists(m_folderPathToExamine))
        return ScannerError::NoSuchPath;
    if (!std::filesystem::is_directory(m_folderPathToExamine))
        return ScannerError::NotADirectory;
    try {
        std::filesystem::directory_iterator it(m_folderPathToExamine);
    } catch (const std::filesystem::filesystem_error& e) {
        return ScannerError::NoAccess;
    }
    return ScannerError::Ok;
}

bool Scanner::check() {
    bool errorOccured = false;
    if (auto err = m_hashBase.checkBaseFile(); err != BaseError::Ok) {
        std::cerr << BASE_ERROR_MESSAGES.at(err) << "\n";
        errorOccured = true;
    }
    if (auto err = m_logger.checkLogFile(); err != LogError::Ok) {
        std::cerr << LOG_ERROR_MESSAGES.at(err) << "\n";
        errorOccured = true;
    }
    if (auto err = checkFolderToExamine(); err != ScannerError::Ok) {
        std::cerr << SCANNER_ERROR_MESSAGES.at(err) << "\n";
        errorOccured = true;
    }
    return !errorOccured;
}

Scanner::Metrics Scanner::getProcessMetrics() {
    return {m_processedFiles, m_analysisErrorFiles, m_malwareFiles, m_duration};
}

bool Scanner::processFile(const std::string &path) {
    m_processedFiles++;
    if (auto hash = MD5FileHasher::fromFile(path); hash) {
        if (auto verdict = m_hashBase.findHashVerdict(Hash::fromString(hash.value())); verdict) {
            m_logger.log(path, hash.value(), verdict.value());
            m_malwareFiles++;
        }
        return true;
    }
    m_analysisErrorFiles++;
    return false;
}

bool Scanner::process() {
    m_processedFiles = 0;
    m_analysisErrorFiles = 0;
    m_malwareFiles = 0;
    m_duration = 0;
    const auto timeStart = std::chrono::high_resolution_clock::now();

    if (!check())
        return false;

    m_hashBase.indexBaseFileContent();

    std::filesystem::path root_path(m_folderPathToExamine);
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(root_path)) {
            if (entry.is_directory())
                continue;
            processFile(entry.path().string());
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Ошибка файловой системы: " << e.what() << std::endl;
        return false;
    }

    const auto timeEnd = std::chrono::high_resolution_clock::now();
    m_duration = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count();
    return true;
}
