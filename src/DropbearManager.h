#pragma once

#include <string>
#include <functional>
#include <sys/types.h>

class DropbearManager {
public:
    using LogCallback = std::function<void(const std::string&)>;

    explicit DropbearManager(LogCallback logCallback);
    ~DropbearManager();

    // Delete copy operations
    DropbearManager(const DropbearManager&) = delete;
    DropbearManager& operator=(const DropbearManager&) = delete;

    bool start();
    void stop();
    void pumpLogs();

private:
    bool ensureHostKey();
    bool fileExists(const std::string& path) const;
    bool isExecutable(const std::string& path) const;
    bool generateHostKey(const std::string& keyPath);
    [[noreturn]] void executeDropbearKeygen(const std::string& keygenPath, const std::string& keyPath);
    bool waitForKeygenCompletion(pid_t pid, const std::string& keyPath);
    
    void startDropbear();
    bool createLogPipe(int pipefd[2]);
    [[noreturn]] void executeDropbear(int pipefd[2], const std::string& db_path);
    void stopDropbearGracefully();
    
    void flushPendingLines(bool force);
    static void trimCR(std::string& s);

    LogCallback log_callback_;
    pid_t dropbear_pid_ = -1;
    int dropbear_fd_ = -1;
    std::string pending_chunk_;
};
