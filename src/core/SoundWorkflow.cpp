#include "SoundWorkflow.h"
#include <cstring>
namespace soundstudio {
SoundWorkflow::SoundWorkflow(ISoundDevice& device, IImageStore& store)
    : device_(device), store_(store), page_(0), install_(false) {}
bool SoundWorkflow::ValidImage(const std::vector<Byte>& image) {
    return image.size() == ImageSize && image[0] == 0xCF;
}
bool SoundWorkflow::Busy() const {
    return status_.stage >= Connecting && status_.stage <= Verifying;
}
void SoundWorkflow::Enter(Stage stage, OperationMessage message) {
    status_.stage = stage; status_.message = message; status_.percent = 0; page_ = 0;
}
void SoundWorkflow::Fail(OperationMessage message) { Enter(Failed, message); }
bool SoundWorkflow::Begin(bool install) {
    if (Busy()) return false;
    status_ = OperationStatus(); install_ = install;
    backup_.assign(ImageSize, 0);
    Enter(Connecting, MsgConnecting);
    return true;
}
bool SoundWorkflow::BeginBackup() {
    if (Busy()) return false;
    image_.clear(); return Begin(false);
}
bool SoundWorkflow::BeginInstall(const std::string& path) {
    if (Busy()) return false;
    status_ = OperationStatus(); image_.clear();
    if (!store_.Load(path, image_) || !ValidImage(image_)) {
        Fail(MsgInvalidImage); return false;
    }
    return Begin(true);
}
bool SoundWorkflow::Cancel() {
    if (!Busy() || status_.chipMayBeModified) return false;
    Enter(Cancelled, MsgCancelled); return true;
}
void SoundWorkflow::Tick() {
    if (!Busy()) return;
    switch (status_.stage) {
    case Connecting:
        if (!device_.Connect() || !device_.PowerUp()) { Fail(MsgUnavailable); return; }
        Enter(ReadingBackup, MsgReading); break;
    case ReadingBackup:
    case CheckingBackup: {
        const bool checking = status_.stage == CheckingBackup;
        Byte data[ReadSize]; unsigned end = page_ + 4;
        if (end > ImageSize / ReadSize) end = ImageSize / ReadSize;
        for (; page_ < end; ++page_) {
            if (!device_.Read(page_, data)) { Fail(MsgReadFailed); return; }
            if (checking) {
                if (std::memcmp(data, &backup_[page_ * ReadSize], ReadSize)) {
                    Fail(MsgUnstable); return;
                }
            } else std::memcpy(&backup_[page_ * ReadSize], data, ReadSize);
        }
        status_.percent = page_ * 100 / (ImageSize / ReadSize);
        if (page_ == ImageSize / ReadSize)
            Enter(checking ? SavingBackup : CheckingBackup,
                  checking ? MsgSaving : MsgChecking);
        break;
    }
    case SavingBackup:
        if (!store_.SaveVerifiedBackup(backup_, status_.backupPath)) {
            Fail(MsgSaveFailed); return;
        }
        Enter(install_ ? Erasing : Complete, install_ ? MsgErasing : MsgBackupDone);
        if (!install_) status_.percent = 100;
        break;
    case Erasing:
        // From this point even an erase timeout may have altered the chip.
        status_.chipMayBeModified = true;
        if (!device_.PowerUp() || !device_.Erase()) { Fail(MsgEraseFailed); return; }
        Enter(Writing, MsgWriting); break;
    case Writing: {
        unsigned end = page_ + 32;
        if (end > ImageSize / WriteSize) end = ImageSize / WriteSize;
        for (; page_ < end; ++page_)
            if (!device_.Write(page_, &image_[page_ * WriteSize])) {
                Fail(MsgWriteFailed); return;
            }
        status_.percent = page_ * 100 / (ImageSize / WriteSize);
        if (page_ == ImageSize / WriteSize) Enter(Verifying, MsgVerifying);
        break;
    }
    case Verifying: {
        Byte data[ReadSize]; unsigned end = page_ + 4;
        if (end > ImageSize / ReadSize) end = ImageSize / ReadSize;
        for (; page_ < end; ++page_)
            if (!device_.Read(page_, data) || std::memcmp(data, &image_[page_ * ReadSize], ReadSize)) {
                Fail(MsgVerifyFailed); return;
            }
        status_.percent = page_ * 100 / (ImageSize / ReadSize);
        if (page_ == ImageSize / ReadSize) {
            Enter(Complete, MsgInstalled); status_.percent = 100;
        }
        break;
    }
    default: break;
    }
}
const char* StageName(Stage stage) {
    static const char* names[] = { "Ready", "Connect", "Read backup", "Check backup", "Save backup",
        "Erase", "Install", "Verify", "Complete", "Failed", "Cancelled" };
    return names[stage];
}
}
