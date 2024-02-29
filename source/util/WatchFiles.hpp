/**
 * \file
 * \author Rudy Castan
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#pragma once


#include <filesystem>
#include <functional>
#include <memory>

namespace util
{
    enum class FileStatus
    {
        Created,
        Modified,
        Erased
    };

    class WatchFiles
    {
    public:
        using Callback = std::function<void(FileStatus status)>;

    private:
        struct Implementation;
        class WatchFilesThreaded;
        class WatchFilesNoThreads;
        std::shared_ptr<Implementation> impl;

        enum UseThreads
        {
            BackgroundThread,
            NoThread
        };

    public:
        explicit WatchFiles(UseThreads should_thread = BackgroundThread, std::chrono::milliseconds the_delay = std::chrono::milliseconds(500));

    public:
        void Watch(const std::filesystem::path& file_path, const Callback& notify_changed);
        void Update();

    public:
        WatchFiles(const WatchFiles&);
        WatchFiles& operator=(const WatchFiles&);

        WatchFiles(WatchFiles&&) noexcept;
        WatchFiles& operator=(WatchFiles&&) noexcept;

        ~WatchFiles();
    };
}
