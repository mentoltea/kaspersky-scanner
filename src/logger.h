#ifndef LOGGER_H
#define LOGGER_H

#ifndef PREFIX
    #define PREFIX
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <mutex>
#include <assert.h>

class ByOStream: private std::streambuf, public std::ostream {
    public:
    ByOStream(std::ostream &s1, std::ostream &s2);

    ByOStream(const ByOStream &other) = delete;
    ByOStream& operator=(const ByOStream &other) = delete;

    // ByOStream& operator<<(const std::string &s);
    // ByOStream& operator<<(std::ostream& (*manip)(std::ostream&));    

    private:
    int overflow(int c) override;

    std::ostream &stream1, &stream2;
};

typedef enum {
    LEVEL_DEBUG,
    LEVEL_LOG,
    LEVEL_FILE,
    LEVEL_DIR,
    LEVEL_INFO,
    LEVEL_WARNING,
    LEVEL_ERROR,
    LEVEL_RESULT,

    LEVEL_COUNT
} LogLevel;

class LogStream {
    public:
    LogStream(std::ostream &os = std::cout);

    PREFIX static std::string LevelToString(LogLevel level);

    LogStream(const LogStream &other) = delete;
    LogStream& operator=(const LogStream &other) = delete;
    
    LogStream& operator<<(const std::string &s);
    LogStream& operator<<(const char* s);
    LogStream& operator<<(const int s);
    LogStream& operator<<(const int64_t s);
    LogStream& operator<<(const long s);
    LogStream& operator<<(const size_t s);
    LogStream& operator<<(const float s);
    LogStream& operator<<(const double s);

    LogStream& operator<<(std::ostream& (*manip)(std::ostream&));

    LogStream& level(LogLevel);
    LogStream& operator()(LogLevel);
    LogStream& operator()(LogLevel, const std::string &s);

    LogStream& change_stream(std::ostream &os);

    void set_min_level(LogLevel level);

    private:
    LogLevel min_level;
    std::mutex stream_mutex;
    std::reference_wrapper<std::ostream> stream;
};

extern LogStream Logger; // Singletone


#endif // LOGGER_H
