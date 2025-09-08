#include <iostream>
#include <string>
#include <cxxopts.hpp>

#include "scanner/Scanner.h"

struct Option {
    const char* short_opt = nullptr;  //!< короткая форма (например, "-p")
    const char* long_opt  = nullptr;  //!< длинная форма (например, "--path")
    const char* descr     = nullptr;  //!< описание назначения опции
    const char* help      = nullptr;  //!< пример значения (для справки)
};

constexpr Option PATH_OPT = {"p", "path", "Path to folder to check for malicious files", "/folder/"};
constexpr Option BASE_OPT = {"b", "base", "Path to file with malicious hash database", "/base.csv"};
constexpr Option LOG_OPT  = {"l", "log",  "Path to log file", "/logfile.log"};
constexpr Option HELP_OPT = {"h", "help", "Print usage"};

constexpr auto CUSTOM_HELP_TXT = "[OPTIONS...]";

std::string make_full_opt(const Option& o) {
    if (o.short_opt)
        return std::string(o.short_opt) + "," + o.long_opt;
    return o.long_opt;
}

bool check_required_opts(const cxxopts::ParseResult& result, std::initializer_list<Option> required) {
    bool ok = true;
    for (auto& opt : required) {
        if (!result.contains(opt.long_opt)) {
            std::cerr << "Missing required option: --" << opt.long_opt << "\n"
                      << "  " << opt.descr << "\n";
            ok = false;
        }
    }
    return ok;
}

void print_metrics(const Scanner::Metrics& metrics) {
    std::cout << "Time elapsed:      " << metrics.duration.count()   << " ms\n"
              << "Processed files:   " << metrics.processedFiles     << "\n"
              << "Malware files:     " << metrics.malwareFiles       << "\n"
              << "Failed to analyse: " << metrics.analysisErrorFiles << "\n";
}

int main(int argc, char *argv[]) {
    // подготовка опций
    cxxopts::Options options("Scanner", "Utility for searching malicious files");
    options.add_options()
        (make_full_opt(PATH_OPT), PATH_OPT.descr, cxxopts::value<std::string>(), PATH_OPT.help)
        (make_full_opt(BASE_OPT), BASE_OPT.descr, cxxopts::value<std::string>(), BASE_OPT.help)
        (make_full_opt(LOG_OPT),  LOG_OPT.descr,  cxxopts::value<std::string>(), LOG_OPT.help)
        (make_full_opt(HELP_OPT), HELP_OPT.descr)
        ;
    options.custom_help(CUSTOM_HELP_TXT);

    // парсинг опций
    cxxopts::ParseResult result;
    try {
        result = options.parse(argc, argv);
    } catch (const cxxopts::exceptions::exception &e) {
        std::cerr << e.what();
        return 1;
    }

    // вызов спарвки
    if (result.contains(HELP_OPT.long_opt)) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    // проверка на наличие всех трёх параметров
    if (!check_required_opts(result, {PATH_OPT, BASE_OPT, LOG_OPT})) {
        return 1;
    }

    // поиск вредоносных файлов
    Scanner scanner(result[PATH_OPT.long_opt].as<std::string>(),
                    result[BASE_OPT.long_opt].as<std::string>(),
                    result[LOG_OPT.long_opt].as<std::string>());
    switch (scanner.process()) {
        case ProcessResult::Ok: {
            print_metrics(scanner.getProcessMetrics());
            break;
        }
        case ProcessResult::ScanCompletedWithErrors: {
            print_metrics(scanner.getProcessMetrics());
            std::cerr << "Scan finished, but following errors occurred:\n";
            for (const auto& errString : scanner.getLastScanErrors()) {
                std::cerr << "  " << errString << "\n";
            }
            break;
        }
        case ProcessResult::CheckFailed: {
            std::cerr << "The following errors occurred:\n";
            for (const auto& errString : scanner.getLastCheckErrors()) {
                std::cerr << "  " << errString << "\n";
            }
            break;
        }
    }
    return 0;
}
