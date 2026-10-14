#pragma once
#include <xtl.h>
#include "../core/SoundWorkflow.h"
#include "../core/ImageBuilder.h"
#include "../core/ImageBuildWorkflow.h"

namespace soundstudio {
class LanguageSettings {
public:
    static bool LoadRussian();
    static bool SaveRussian(bool russian);
};
class XboxSoundDevice : public ISoundDevice {
public:
    bool Connect(); bool PowerUp(); bool Read(unsigned page, Byte* data);
    bool Erase(); bool Write(unsigned page, const Byte* data); bool Play(unsigned index);
};
struct ImageEntry { std::string path, name; bool valid; };
class XboxWorkshopFiles {
public:
    static std::vector<std::string> ListImports();
    static bool ReadWave(const std::string& path, std::vector<Byte>& data, DWORD* error=NULL);
    static bool SaveImage(const std::vector<Byte>& image, std::string& path);
};
class XboxImageStore : public IImageStore {
public:
    bool Load(const std::string& path, std::vector<Byte>& data);
    bool SaveVerifiedBackup(const std::vector<Byte>& data, std::string& path);
    std::vector<ImageEntry> List(bool backups, DWORD* scanError=NULL);
};
// Worker owns the result until its thread has finished. Only the UI thread
// calls Start/Take; enumeration never calls XUI or modifies its timers.
class LibraryScanner {
public:
    LibraryScanner(); ~LibraryScanner();
    bool Start(bool backups, unsigned generation);
    bool Take(std::vector<ImageEntry>& result, unsigned& generation, DWORD& error);
    bool Active() const { return thread_ != NULL; }
private:
    HANDLE thread_;
    bool backups_;
    unsigned generation_;
    volatile LONG completed_;
    DWORD error_;
    std::vector<ImageEntry> result_;
    static DWORD WINAPI ThreadMain(void* context);
    LibraryScanner(const LibraryScanner&); LibraryScanner& operator=(const LibraryScanner&);
};

// Active Object: slow hardware work lives on a worker; UI reads a locked snapshot.
class OperationRunner {
public:
    OperationRunner(); ~OperationRunner();
    bool Start(bool install, const std::string& path);
    bool StartPlayback(unsigned index);
    bool StartBuild(const std::string& power, const std::string& eject);
    void RequestCancel();
    OperationStatus Snapshot();
    bool Running() const;
private:
    XboxSoundDevice device_; XboxImageStore store_;
    HANDLE thread_; CRITICAL_SECTION lock_;
    volatile LONG running_, cancel_;
    enum OperationKind { BackupOperation, InstallOperation, PlaybackOperation, BuildOperation };
    OperationKind kind_;
    unsigned index_;
    std::string path_, ejectPath_;
    OperationStatus snapshot_;
    static DWORD WINAPI ThreadMain(void* context);
    void Publish(const OperationStatus& status);
    bool Launch();
    OperationRunner(const OperationRunner&); OperationRunner& operator=(const OperationRunner&);
};
bool MountSystemFont();
}
