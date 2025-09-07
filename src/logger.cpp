#include "logger.h"

LogStream Logger;

LogStream::LogStream(std::ostream &os): stream(os) {}

LogStream& LogStream::operator<< (const std::string &s) {
    const std::lock_guard<std::mutex> lock(stream_mutex);
    stream.get() << s << std::flush_emit;
    return *this;
}

LogStream& LogStream::operator<<(const char* s) {
    const std::lock_guard<std::mutex> lock(stream_mutex);
    stream.get() << s << std::flush_emit;
    return *this;
}

LogStream& LogStream::operator<<(const int s) {
    const std::lock_guard<std::mutex> lock(stream_mutex);
    stream.get() << s << std::flush_emit;
    return *this;
}

LogStream& LogStream::operator<<(const int64_t s) {
    const std::lock_guard<std::mutex> lock(stream_mutex);
    stream.get() << s << std::flush_emit;
    return *this;
}

LogStream& LogStream::operator<<(const long s) {
    const std::lock_guard<std::mutex> lock(stream_mutex);
    stream.get() << s << std::flush_emit;
    return *this;
}

LogStream& LogStream::operator<<(const size_t s) {
    const std::lock_guard<std::mutex> lock(stream_mutex);
    stream.get() << s << std::flush_emit;
    return *this;
}

LogStream& LogStream::operator<<(const float s) {
    const std::lock_guard<std::mutex> lock(stream_mutex);
    stream.get() << s << std::flush_emit;
    return *this;
}

LogStream& LogStream::operator<<(const double s) {
    const std::lock_guard<std::mutex> lock(stream_mutex);
    stream.get() << s << std::flush_emit;
    return *this;
}


LogStream& LogStream::operator<<(std::ostream& (*manip)(std::ostream&)) {
    const std::lock_guard<std::mutex> lock(stream_mutex);
    manip(stream);
    return *this;
}

std::string LogStream::LevelToString(LogLevel level) {
    switch (level) {
    case LEVEL_DEBUG: return "DEBUG";
    case LEVEL_LOG: return "LOG";
    case LEVEL_FILE: return "FILE";
    case LEVEL_DIR: return "DIR";
    case LEVEL_INFO: return "INFO";
    case LEVEL_WARNING: return "WARNING";
    case LEVEL_RESULT: return "RESULT";
    case LEVEL_ERROR: return "ERROR";
    
    default: {
        assert("Unreachable" && 0);
        break;
    }
    }
}

LogStream& LogStream::level(LogLevel level) {
    if (level >= min_level) {
        std::string levelstr = LevelToString(level);

        const std::lock_guard<std::mutex> lock(stream_mutex);
        stream.get() << "[" << levelstr << "] " << std::flush_emit;
    }
    return *this;
}

LogStream& LogStream::operator()(LogLevel level) {
    if (level >= min_level) {
        std::string levelstr = LevelToString(level);

        const std::lock_guard<std::mutex> lock(stream_mutex);
        stream.get() << "[" << levelstr << "] " << std::flush_emit;
    }
    return *this;
}

LogStream& LogStream::operator()(LogLevel level, const std::string &s) {
    if (level >= min_level) {
        std::string levelstr = LevelToString(level);

        const std::lock_guard<std::mutex> lock(stream_mutex);
        stream.get() << "[" << levelstr << "] " << s << std::flush_emit;
    }
    return *this;
}


LogStream& LogStream::change_stream(std::ostream &os) {
    const std::lock_guard<std::mutex> lock(stream_mutex);
    stream = std::ref(os);
    return *this;
}

void LogStream::set_min_level(LogLevel level) {
    const std::lock_guard<std::mutex> lock(stream_mutex);
    min_level = level;    
}

ByOStream::ByOStream(std::ostream &s1, std::ostream &s2): std::ostream(this), stream1(s1), stream2(s2) {}

// ByOStream& ByOStream::operator<< (const std::string &s) {
//     stream1 << s;
//     stream2 << s;
//     return *this;
// }

// ByOStream& ByOStream::operator<<(std::ostream& (*manip)(std::ostream&)) {
//     manip(stream1);
//     manip(stream2);
//     return *this;
// }

int ByOStream::overflow(int c) {
    stream1 << (char)c;
    stream2 << (char)c;
    return 0;
}