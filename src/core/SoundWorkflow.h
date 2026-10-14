#pragma once
#include <string>
#include <vector>

namespace soundstudio {
typedef unsigned char Byte;
enum { ImageSize = 45056, ReadSize = 512, WriteSize = 16 };

// Protected Variations: the application knows neither GPIO nor Xbox file APIs.
class ISoundDevice {
public:
    virtual ~ISoundDevice() {}
    virtual bool Connect() = 0;
    virtual bool PowerUp() = 0;
    virtual bool Read(unsigned page, Byte* data) = 0;
    virtual bool Erase() = 0;
    virtual bool Write(unsigned page, const Byte* data) = 0;
    virtual bool Play(unsigned index) = 0;
};
class IImageStore {
public:
    virtual ~IImageStore() {}
    virtual bool Load(const std::string& path, std::vector<Byte>& data) = 0;
    // Must create a new file, flush it, reload it and compare before returning true.
    virtual bool SaveVerifiedBackup(const std::vector<Byte>& data, std::string& path) = 0;
};
enum Stage { Idle, Connecting, ReadingBackup, CheckingBackup, SavingBackup,
             Erasing, Writing, Verifying, Complete, Failed, Cancelled };
enum OperationMessage { MsgNone, MsgConnecting, MsgInvalidImage, MsgCancelled, MsgUnavailable, MsgReading, MsgReadFailed, MsgUnstable, MsgSaving, MsgChecking, MsgSaveFailed, MsgErasing, MsgBackupDone, MsgEraseFailed, MsgWriting, MsgWriteFailed, MsgVerifying, MsgVerifyFailed, MsgInstalled, MsgStarting, MsgWorkerFailed, MsgPlayDone, MsgPlayFailed, MsgBuildReading, MsgBuildDone, MsgBuildReadFailed, MsgBuildInvalid, MsgBuildUnsupported, MsgBuildEmpty, MsgBuildTooLarge, MsgBuildSaveFailed, MsgCount };
struct OperationStatus {
    Stage stage;
    unsigned percent;
    bool chipMayBeModified;
    OperationMessage message;
    std::string backupPath;
    std::string outputPath;
    std::string inputPath;
    unsigned fileError;
    OperationStatus() : stage(Idle), percent(0), chipMayBeModified(false), message(MsgNone), fileError(0) {}
};

// GRASP Controller: owns one use case; never touches controls or platform APIs.
class SoundWorkflow {
public:
    SoundWorkflow(ISoundDevice& device, IImageStore& store);
    bool BeginBackup();
    bool BeginInstall(const std::string& path);
    void Tick();
    bool Cancel();
    bool Busy() const;
    const OperationStatus& Status() const { return status_; }
    static bool ValidImage(const std::vector<Byte>& image);
private:
    ISoundDevice& device_;
    IImageStore& store_;
    OperationStatus status_;
    std::vector<Byte> image_, backup_;
    unsigned page_;
    bool install_;
    bool Begin(bool install);
    void Enter(Stage stage, OperationMessage message);
    void Fail(OperationMessage message);
    SoundWorkflow(const SoundWorkflow&);
    SoundWorkflow& operator=(const SoundWorkflow&);
};
const char* StageName(Stage stage);
}
