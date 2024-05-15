/**
 * \file
 * \author Rudy Castan
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#include "WatchFiles.hpp"

#include "environment/Environment.hpp"

#if defined(CAN_USE_THREADS)
#    include <atomic>
#    include <chrono>
#    include <condition_variable>
#    include <mutex>
#    include <thread>
#endif

#include "Timer.hpp"
#include <iostream>
#include <vector>

namespace
{
    struct FileToWatch
    {
        std::filesystem::path           FilePath;
        util::WatchFiles::Callback      Notify;
        util::FileStatus                Status;
        std::filesystem::file_time_type LastWriteTime;
    };

    class FilePathCollection
    {
        std::vector<FileToWatch> the_files;

    public:
        void AddFile(const std::filesystem::path& file_path, const util::WatchFiles::Callback& notify_changed)
        {
            FileToWatch info{ file_path, notify_changed, util::FileStatus::Created, std::filesystem::file_time_type{} };
            if (!std::filesystem::exists(file_path))
            {
                info.Status = util::FileStatus::Erased;
            }
            else
            {
                info.LastWriteTime = std::filesystem::last_write_time(file_path);
            }
            the_files.push_back(info);
        }

        void CheckForFileChanges()
        {
            const auto NotifyFileChange = [](const FileToWatch& info)
            {
                try
                {
                    info.Notify(info.Status);
                }
                catch (const std::exception& e)
                {
                    std::cerr << e.what();
                }
            };
            for (auto& info : the_files)
            {
                switch (info.Status)
                {
                    case util::FileStatus::Created:
                    case util::FileStatus::Modified:
                        {
                            if (std::filesystem::exists(info.FilePath))
                            {
                                if (const auto current_write_time = std::filesystem::last_write_time(info.FilePath); current_write_time != info.LastWriteTime)
                                {
                                    info.Status        = util::FileStatus::Modified;
                                    info.LastWriteTime = current_write_time;
                                    NotifyFileChange(info);
                                }
                            }
                            else
                            {
                                info.Status = util::FileStatus::Erased;
                                NotifyFileChange(info);
                            }
                        }
                        break;
                    case util::FileStatus::Erased:
                        if (std::filesystem::exists(info.FilePath))
                        {
                            info.Status        = util::FileStatus::Created;
                            info.LastWriteTime = std::filesystem::last_write_time(info.FilePath);
                            NotifyFileChange(info);
                        }
                        break;
                }
            }
        }
    };


}

namespace util
{
    struct WatchFiles::Implementation
    {
        Implementation() = default;

        virtual void Watch(const std::filesystem::path& file_path, const Callback& notify_changed) = 0;

        virtual void Update()
        {
        }

        constexpr virtual ~Implementation()                  = default;
        Implementation(const Implementation&)                = default;
        Implementation& operator=(const Implementation&)     = default;
        Implementation(Implementation&&) noexcept            = default;
        Implementation& operator=(Implementation&&) noexcept = default;
    };

#if defined(CAN_USE_THREADS)

    class WatchFiles::WatchFilesThreaded : public Implementation
    {
        std::chrono::milliseconds delay;
        std::atomic_bool          isWatching;
        FilePathCollection        files;
        std::mutex                filesMutex;
        std::thread               watcherThread;
        std::condition_variable   wakeUpEvent;
        std::mutex                wakeUpMutex;


    public:
        explicit WatchFilesThreaded(std::chrono::milliseconds the_delay)
            : delay(the_delay), isWatching{}, files{}, filesMutex{}, watcherThread{}
        {
            isWatching = false;
        }

    public:
        void Watch(const std::filesystem::path& file_path, const Callback& notify_changed) override
        {
            {
                std::scoped_lock lock(filesMutex);
                files.AddFile(file_path, notify_changed);
            }
            if (!isWatching)
            {
                isWatching    = true;
                watcherThread = std::thread(
                    [this]()
                    {
                        while (isWatching)
                        {
                            {
                                std::scoped_lock lock(filesMutex);
                                files.CheckForFileChanges();
                            }
                            {
                                std::unique_lock lock(wakeUpMutex);
                                wakeUpEvent.wait_for(lock, delay);
                            }
                        }
                    });
            }
        }


    public:
        WatchFilesThreaded(const WatchFilesThreaded&)                      = delete;
        WatchFilesThreaded& operator=(const WatchFilesThreaded&)           = delete;
        WatchFilesThreaded(WatchFilesThreaded&&) noexcept                  = delete;
        WatchFilesThreaded& operator=(const WatchFilesThreaded&&) noexcept = delete;

        ~WatchFilesThreaded() override
        {
            isWatching = false;
            wakeUpEvent.notify_all();
            if (watcherThread.joinable())
                watcherThread.join();
        }
    };

#endif

    class WatchFiles::WatchFilesNoThreads : public Implementation
    {
        double             delay;
        FilePathCollection files;
        Timer              timer;

    public:
        WatchFilesNoThreads(std::chrono::milliseconds the_delay)
        {
            const auto in_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(the_delay);
            delay                 = in_seconds.count();
        }

        void Watch(const std::filesystem::path& file_path, const Callback& notify_changed) override
        {
            files.AddFile(file_path, notify_changed);
        }

        void Update() override
        {
            if (timer.GetElapsedSeconds() > delay)
            {
                files.CheckForFileChanges();
                timer.ResetTimeStamp();
            }
        }
    };

    WatchFiles::WatchFiles([[maybe_unused]] UseThreads should_thread, std::chrono::milliseconds the_delay)
        : impl{ nullptr }
    {
#if defined(CAN_USE_THREADS)
        switch (should_thread)
        {
            case BackgroundThread:
                impl = std::make_shared<WatchFilesThreaded>(the_delay);
                break;
            case NoThread:
                impl = std::make_shared<WatchFilesNoThreads>(the_delay);
                break;
        }

#else
        impl = std::make_shared<WatchFilesNoThreads>(the_delay);
#endif
    }

    void WatchFiles::Watch(const std::filesystem::path& file_path, const Callback& notify_changed)
    {
        impl->Watch(file_path, notify_changed);
    }

    void WatchFiles::Update()
    {
        impl->Update();
    }

    WatchFiles::WatchFiles(const WatchFiles&)            = default;
    WatchFiles& WatchFiles::operator=(const WatchFiles&) = default;

    WatchFiles::WatchFiles(WatchFiles&&) noexcept            = default;
    WatchFiles& WatchFiles::operator=(WatchFiles&&) noexcept = default;

    WatchFiles::~WatchFiles() = default;
}
