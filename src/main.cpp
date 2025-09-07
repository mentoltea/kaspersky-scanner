#include "connector.h"


std::string dir_path = "./";
std::string base_path = "base.csv";
std::string logfile = "out.log";
size_t max_threads = 8;
size_t max_chunk_size = MAX_CHUNK_SIZE;
LogLevel min_loglevel = LEVEL_INFO;

bool parse_args(int argc, char** argv) {
    bool 
    base_flag=false, 
    log_flag=false, 
    path_flag=false, 
    threads_flag=false, 
    loglevel_flag=false, 
    chunk_flag=false;
    for (int i=1; i<argc; i++) {
        std::string elem = argv[i];
        if (base_flag) {
            base_path = elem;
            base_flag = false;
            continue;
        } else if (log_flag) {
            logfile = elem;
            log_flag = false;
            continue;
        } else if (path_flag) {
            dir_path = elem;
            path_flag = false;
            continue;
        } else if (threads_flag) {
            size_t new_threads = std::stoull(elem);
            if (new_threads == 0) {
                std::cout << "Cannot set 0 threads" << std::endl;
                return false;
            }
            max_threads = new_threads;
            threads_flag = false;
            continue;
        } else if (loglevel_flag) {
            if (elem == "DEBUG") {
                min_loglevel = LEVEL_DEBUG;
            } else if (elem == "LOG") {
                min_loglevel = LEVEL_LOG;
            } else if (elem == "FILE") {
                min_loglevel = LEVEL_FILE;
            } else if (elem == "DIR") {
                min_loglevel = LEVEL_DIR;
            } else if (elem == "INFO") {
                min_loglevel = LEVEL_INFO;
            } else if (elem == "WARNING") {
                min_loglevel = LEVEL_WARNING;
            } else if (elem == "ERROR") {
                min_loglevel = LEVEL_ERROR;
            } else {
                std::cout << "Cannot identify log level " << elem << std::endl;
                std::cout << "Available levels:" << std::endl;
                for (int i=0; i<LEVEL_COUNT; i++) {
                    std::cout << "\t" << LogStream::LevelToString( (LogLevel) i) << std::endl;
                }
                return false;
            }
            loglevel_flag = false;
            continue;
        } else if (chunk_flag) {
            size_t new_chunk = std::stoull(elem);
            if (new_chunk == 0) {
                std::cout << "Cannot set 0 bytes chunk" << std::endl;
                return false;
            }
            max_chunk_size = new_chunk;
            chunk_flag = false;
            continue;
        } else if (elem.starts_with("--")) {
            if (elem == "--base") base_flag = true; 
            else if (elem == "--log") log_flag = true; 
            else if (elem == "--loglevel") loglevel_flag = true; 
            else if (elem == "--path") path_flag = true; 
            else if (elem == "--threads") threads_flag = true; 
            else if (elem == "--chunk") chunk_flag = true;
            else {
                std::cout << "Unknown flag " << elem << std::endl;
                return false; 
            } 
            continue;
        }

        std::cout << "Cannot identify token " << elem << std::endl;
        return false;
    }
    return true;
}

int main(int argc, char** argv) {
    std::setlocale(LC_ALL, ".UTF-8");
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    if (!parse_args(argc, argv)) {
        std::cout << "Error parsing arguments" << std::endl;
        return 1;
    }

    std::ofstream fd(logfile);
    ByOStream logstream(fd, std::cout);
    
    Logger.change_stream(logstream);
    
    Logger(LEVEL_INFO) << "Search directory: " << dir_path << std::endl;
    Logger(LEVEL_INFO) << "Base: " << base_path << std::endl;
    Logger(LEVEL_INFO) << "Logfile: " << logfile << std::endl;
    Logger(LEVEL_INFO) << "Max threads: " << max_threads << std::endl;
    Logger(LEVEL_INFO) << "Max chunk size: " << max_chunk_size << std::endl;
    Logger(LEVEL_INFO) << "Min loglevel: " << LogStream::LevelToString( min_loglevel ) << std::endl;
    Logger << std::endl;
    
    Logger.set_min_level(min_loglevel);
    
    std::ifstream stream(base_path);
    Table table = read_csv(stream);
    ThreatBase tb = table_to_base(table);
    
    Logger.level(LEVEL_INFO) << "Loaded " << tb.size() << " threats from " << base_path << std::endl;
    Logger << std::endl;
    
    if (tb.size() == 0) {
        Logger(LEVEL_ERROR) << "Base is empty - exiting" << std::endl; 
        return 1;
    }
    
    fs::path path(dir_path);
    
    SearchStatistics stat;
    if (1) {
        stat = search_in_directory( path , max_threads , tb , max_chunk_size);
    } else {
        Logger(LEVEL_ERROR) << dir_path << " is not a directory" << std::endl;
        return 1;  
    }
    
    Logger << std::endl;
    Logger(LEVEL_INFO) << "\tTotal scanned: " << stat.total_count << std::endl;
    Logger(LEVEL_INFO) << "\tBytes processed: " << stat.bytes_processed << std::endl;
    Logger(LEVEL_INFO) << "\tEmpty files: " << stat.empty_files << std::endl;
    Logger(LEVEL_INFO) << "\tSkipped directories: " << stat.skipped_directories << std::endl;
    Logger(LEVEL_INFO) << "\tSkipped files: " << stat.skipped_files << std::endl;
    for (auto &s: stat.unchecked) {
        Logger << "\t\t" << s << std::endl;
    }
    Logger(LEVEL_INFO) << "\tPotential threats: " << stat.threats.size() << std::endl;
    for (auto &threat: stat.threats) {
        Logger << "\t\t" << threat.filepath << " : " << threat.description.name << std::endl;
    }

    return 0;
}