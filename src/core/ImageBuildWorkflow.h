#pragma once
#include "ImageBuilder.h"
namespace soundstudio {
// Protected Variations: the use case has no Xbox, UI or filesystem dependency.
class IWorkshopStore {
public:
    virtual ~IWorkshopStore() {}
    virtual bool ReadWave(const std::string& path, std::vector<Byte>& data, unsigned& error)=0;
    virtual bool SaveImage(const std::vector<Byte>& image, std::string& path)=0;
};
class ImageBuildWorkflow {
public:
    explicit ImageBuildWorkflow(IWorkshopStore& store);
    void Begin(const std::string& power, const std::string& eject);
    void Tick();
    void Cancel();
    bool Busy() const;
    const OperationStatus& Status() const { return status_; }
private:
    enum Step { ReadPower, ReadEject, DecodePower, DecodeEject, Assemble, Save, Finished };
    IWorkshopStore& store_;
    Step step_;
    OperationStatus status_;
    std::string powerPath_, ejectPath_;
    std::vector<Byte> power_, eject_, image_;
    WaveClip powerClip_, ejectClip_;
    void Fail(BuildError error);
    ImageBuildWorkflow(const ImageBuildWorkflow&);
    ImageBuildWorkflow& operator=(const ImageBuildWorkflow&);
};
}
