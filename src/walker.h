#ifndef WALKER_H
#define WALKER_H

#include "logger.h"
#include "hasher.h"
#include "csvloader.h"

#include <filesystem>
namespace fs = std::filesystem;
namespace chrono = std::chrono;

#include <thread>
#include <list>
#include <tuple>
#include <format>
#include <chrono>

#ifndef PREFIX
#define PREFIX
#endif

struct HashingResult {
    std::string filepath;
    std::string hashstring;
    size_t filesize;
    bool hashing_completed;
    bool unable_to_read;
    bool is_in_threatbase;
};

struct Threat {
    std::string filepath;
    std::string hashstring;
    ThreatDescription description;
};

struct SearchStatistics {
    size_t total_count, skipped_files, empty_files, skipped_directories;
    size_t bytes_processed;
    chrono::steady_clock::time_point search_start;
    chrono::steady_clock::time_point search_end;
    std::vector<Threat> threats;
    std::vector<std::string> unchecked;
};

// Стандартный размер чанка - мегабайт
#define MAX_CHUNK_SIZE 1024*1024

std::string hash_file(fs::path file, size_t max_chunk_size = MAX_CHUNK_SIZE);

void thread_hash_file(fs::path path, HashingResult& result, int& free_flag, size_t max_chunk_size, const ThreatBase &tb);

void launch_thread_on_hashing(
    fs::path path, 
    std::vector<std::jthread> &threads, 
    std::vector< std::atomic<bool> > &thread_free,
    std::vector< std::list< HashingResult > > &results, 
    size_t max_threads,
    size_t max_chunk_size,
    const ThreatBase &tb
);

class PermissionDeniedException: public std::exception {
};

std::string errno_to_string(DWORD dwError);

void recursive_search(
    std::vector< std::jthread > &threads, 
    std::vector< int > &thread_free, 
    std::vector< std::list< HashingResult > > &results,
    size_t &preskipped,

    fs::path dir, 
    size_t max_threads, 
    const ThreatBase &tb, 
    size_t max_chunk_size
);

SearchStatistics search_in_directory(fs::path dir, size_t max_threads, const ThreatBase &tb, size_t max_chunk_size = MAX_CHUNK_SIZE);

bool check_read_permission(const fs::path& path);

std::ifstream open_file(const fs::path& path);

#endif // WALKER_H