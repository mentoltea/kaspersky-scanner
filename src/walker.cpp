#include "walker.h"

bool check_read_permission(const fs::path& path) {
    if (!fs::exists(path)) return false;
    if (!fs::is_regular_file(path)) return false;

    auto perms = fs::status(path).permissions();
    bool can_read = (perms & fs::perms::others_read) != fs::perms::none;
    return can_read;
}

std::ifstream open_file(const fs::path& path) {
    return std::ifstream(path.wstring().c_str());
}

std::string hash_file(fs::path file, size_t max_chunk_size) {
    char* buffer = nullptr;
    MD5Hasher hasher;
    
    std::ifstream fd = open_file(file);
    
    buffer = new char[max_chunk_size];
    if (!buffer) Logger(LEVEL_ERROR, std::format("Cannot allocate {0} bytes\n", max_chunk_size)); 

    int empty_reads = 0;

    while (!fd.eof()) {\
        fd.read(buffer, max_chunk_size);
        int buffsize = fd.gcount();
        if (buffsize == 0) { 
            empty_reads++;
            // if (empty_reads>=3) {
            //     if (buffer) delete[] buffer;
            //     // throw PermissionDeniedException();
            //     return "";
            // }
        }
        Logger(LEVEL_DEBUG, std::format("Loaded {0} bytes to buffer\n", buffsize));
        hasher.Hash(buffer, buffsize);
    }
    if (buffer) delete[] buffer;
    
    return hasher.GetHashString();
}

void thread_hash_file(fs::path path, HashingResult& result, int& free_flag, size_t max_chunk_size, const ThreatBase &tb) {
    result.filepath = path.string();
    result.unable_to_read = false;
    result.filesize = 0;
    
    try {
    if (check_read_permission(path)) {
        result.filesize = fs::file_size(path);
        if (result.filesize == 0) {
            Logger(LEVEL_LOG, std::format("Empty file {0}\n", result.filepath));
            result.hashing_completed = false;
        } else {
            Logger(LEVEL_LOG, std::format("Starting hashing file {0}\n", result.filepath));
            result.hashstring = hash_file(path, max_chunk_size);
            if (result.hashstring.size() == 0) {
                Logger(LEVEL_INFO, std::format("Cannot read file {0}\n", result.filepath));
                result.unable_to_read = true;
                result.hashing_completed = false;
            } else {
                result.hashing_completed = true;
                Logger(LEVEL_LOG, std::format("Hash of {0} : {1}\n", result.filepath, result.hashstring));
            }
        }
    } else {
        Logger(LEVEL_INFO, std::format("Permission denied to {0}\n", result.filepath));
        result.hashing_completed = false;
        result.unable_to_read = true;
    }

    if (result.hashing_completed) {
        if (tb.contains(result.hashstring)) {
            result.is_in_threatbase = true;
            Logger(LEVEL_INFO, std::format("{0} is in threat base\n", result.filepath));
        } else result.is_in_threatbase = false;
    }
    } catch (std::exception &e) {
        Logger(LEVEL_ERROR, std::format("Fatal error reading {0}\n", result.filepath));
        result.unable_to_read = true;
    }

    free_flag = true;
    return;
}

void launch_thread_on_hashing(
    fs::path path, 
    std::vector<std::jthread> &threads, 
    std::vector< int > &thread_free,
    std::vector< std::list< HashingResult > > &results, 
    size_t max_threads,
    size_t max_chunk_size,
    const ThreatBase &tb
) {
    if (threads.size() < max_threads) {
        int idx = threads.size();
        
        Logger(LEVEL_DEBUG, std::format("Creating thread[{0}]\n", idx));

        thread_free[idx] = false;
        results[idx].push_back( {.hashing_completed=false} );

        threads.push_back(
            std::jthread(
                thread_hash_file, 
                path, std::ref((results[idx].back())), std::ref(thread_free[idx]), max_chunk_size, std::ref(tb)
            )
        );
        
        return;
    }
    
    int free_idx = -1;
    while (free_idx < 0) {
        for (size_t i=0; i<thread_free.size(); i++) {
            if (thread_free[i]) {
                if (threads[i].joinable()) threads[i].join();
                free_idx = (int)i;
                break;
            }
        }
        // Logger(LEVEL_DEBUG, "Waiting for free thread\n");
        // std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    Logger(LEVEL_DEBUG, std::format("Using thread[{0}]\n", free_idx));
    thread_free[free_idx] = false;
    results[free_idx].push_back( {.hashing_completed=false} );
    threads[free_idx] = std::jthread(
        thread_hash_file, 
        path, std::ref((results[free_idx].back())), std::ref(thread_free[free_idx]), max_chunk_size, std::ref(tb)
    );
}

std::string errno_to_string(DWORD dwError) {
    LPSTR buffer = NULL;
    size_t _ = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, dwError, 
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
        (LPSTR)&buffer, 
        0, 
        NULL
    );
    std::string str = buffer;
    LocalFree(buffer);
    return str;
}

#define WINAPI_FILE_SEARCH

void recursive_search(
    std::vector< std::jthread > &threads, 
    std::vector< int > &thread_free, 
    std::vector< std::list< HashingResult > > &results,
    size_t &preskipped_directories,

    fs::path dir, 
    size_t max_threads, 
    const ThreatBase &tb, 
    size_t max_chunk_size
) {

#ifdef WINAPI_FILE_SEARCH
    std::wstring wdir = dir.wstring();
    
    if (wdir[wdir.size()-1] != '\\') {
        wdir = wdir + (wchar_t)'\\';
    }
    
    std::wstring constructed_dir = wdir;
    if (constructed_dir[constructed_dir.size()-1] != '*') {
        constructed_dir = constructed_dir + (wchar_t)'*';
    }

    const wchar_t* szDir = constructed_dir.c_str();
    HANDLE hFind = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW ffd;
    DWORD dwError=0;

    hFind = FindFirstFileW(szDir, &ffd);
    
    if (INVALID_HANDLE_VALUE == hFind) {
        Logger(LEVEL_INFO, std::format("Cannot acces {0}\n", dir.string()));
        return;
    }
    
    do {
        fs::path entry = ffd.cFileName; 
        std::string filename = entry.string();
        fs::path constructed_path = wdir + entry.wstring();
        std::string real_path = constructed_path.string();
        
        if (filename == ".") continue;
        if (filename == "..") continue;
        
        if (! (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            // file
            Logger(LEVEL_FILE, std::format("\t{0}\n", real_path));
            launch_thread_on_hashing(constructed_path, threads, thread_free, results, max_threads, max_chunk_size, tb);
            
        }
    } while (FindNextFileW(hFind, &ffd) != 0);
    
    dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES) {
        std::string error = errno_to_string(dwError);
        Logger(LEVEL_ERROR, std::format("{0} {1}\n", dwError, error));
    }

    FindClose(hFind);
    
    hFind = FindFirstFileW(szDir, &ffd);
    
    if (INVALID_HANDLE_VALUE == hFind) {
        dwError = GetLastError();
        std::string error = errno_to_string(dwError);
        Logger(LEVEL_INFO, std::format("Cannot get handle on {0}\n", dir.string()));
        return;
    }

    do {
        fs::path entry = ffd.cFileName; 
        std::string filename = entry.string();
        fs::path constructed_path = wdir + entry.wstring();
        std::string real_path = constructed_path.string();
        
        if (filename == ".") continue;
        if (filename == "..") continue;
        
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // directory
            Logger(LEVEL_DIR, std::format("\t{0}\n", real_path));
            try {
                recursive_search(threads, thread_free, results, preskipped_directories, constructed_path, max_threads, tb, max_chunk_size);            
            } catch (fs::filesystem_error &e) {
                preskipped_directories++;
                Logger(LEVEL_INFO, std::format("Cannot access to {0}\n", dir.string()));
            }
        }
    } while (FindNextFileW(hFind, &ffd) != 0);
    
    dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES) {
        std::string error = errno_to_string(dwError);
        Logger(LEVEL_ERROR, std::format("{0} {1}\n", dwError, error));
    }
    
    FindClose(hFind);

    return;
    
    #else // filesystem file search

    for (const auto &entry: fs::directory_iterator(dir)) {
        if (entry.is_regular_file()) {
            launch_thread_on_hashing(entry.path(), threads, thread_free, results, max_threads, max_chunk_size, tb);
        }
    }

    for (const auto &entry: fs::directory_iterator(dir, fs::directory_options::skip_permission_denied)) {
        if (entry.is_directory()) {
            Logger(LEVEL_DIRECTORIES, std::format("\t{0}\n", entry.path().string()));
            try {
                recursive_search(threads, thread_free, results, preskipped_directories, entry, max_threads, tb, max_chunk_size);
            } catch (std::exception &e) {
                preskipped_directories++;
                Logger(LEVEL_ERROR, std::format("{0}\n", entry.path().string()));
            }
        }
    }
#endif
}

SearchStatistics search_in_directory(fs::path dir, size_t max_threads, const ThreatBase &tb, size_t max_chunk_size) {
    std::vector< std::jthread > threads;
    std::vector< int > thread_free(max_threads);
    std::vector< std::list< HashingResult > > results(max_threads);
    
    SearchStatistics stat = {.total_count=0, .skipped_files=0, .skipped_directories=0, .bytes_processed=0};
    
    
    Logger(LEVEL_INFO) << "Search started" << std::endl;
    stat.search_start = chrono::steady_clock::now();
    
    try {
        recursive_search(threads, thread_free, results, stat.skipped_directories, dir, max_threads, tb, max_chunk_size);
    } catch (std::exception &e) {
        Logger(LEVEL_ERROR, std::format("{0}\n", std::string(e.what())) );
    }
    
    for (std::jthread &t: threads) {
        t.join();
    }
    
    stat.search_end = chrono::steady_clock::now();
    Logger(LEVEL_INFO) << "Search completed" << std::endl;
    
    for (auto &list: results) {
        for (auto &result: list) {
            stat.total_count++;
            if (result.hashing_completed) {
                stat.bytes_processed += result.filesize;
                if (result.is_in_threatbase) {
                    Threat threat = {
                        .filepath = result.filepath,
                        .hashstring = result.hashstring,
                    };
                    threat.description = tb.at(result.hashstring);
                    stat.threats.push_back(threat);
                }
            } else {
                if (result.unable_to_read) {
                    stat.skipped_files++;
                    stat.unchecked.push_back(result.filepath);
                }
                else stat.empty_files++;
            }
        }
    }
    
    return stat;
}