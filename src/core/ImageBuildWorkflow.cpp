#include "ImageBuildWorkflow.h"
namespace soundstudio {
ImageBuildWorkflow::ImageBuildWorkflow(IWorkshopStore& store):store_(store),step_(Finished) {}
bool ImageBuildWorkflow::Busy() const { return step_!=Finished; }
void ImageBuildWorkflow::Begin(const std::string& power,const std::string& eject) {
    powerPath_=power;ejectPath_=eject;
    power_.clear();eject_.clear();image_.clear();
    powerClip_=WaveClip();ejectClip_=WaveClip();
    status_=OperationStatus();status_.stage=ReadingBackup;status_.message=MsgBuildReading;
    step_=ReadPower;
}
void ImageBuildWorkflow::Cancel() {
    if(!Busy())return;
    status_.stage=Cancelled;status_.message=MsgCancelled;step_=Finished;
}
void ImageBuildWorkflow::Fail(BuildError error) {
    switch(error) {
    case InvalidWave: status_.message=MsgBuildInvalid;break;
    case UnsupportedWave: status_.message=MsgBuildUnsupported;break;
    case EmptyAudio: status_.message=MsgBuildEmpty;break;
    case ImageTooLarge: status_.message=MsgBuildTooLarge;break;
    case BuildReadFailed: status_.message=MsgBuildReadFailed;break;
    default: status_.message=MsgBuildSaveFailed;break;
    }
    status_.stage=Failed;step_=Finished;
}
void ImageBuildWorkflow::Tick() {
    if(!Busy())return;
    BuildError error=BuildOk;
    switch(step_) {
    case ReadPower:
        status_.inputPath=powerPath_;
        if(!store_.ReadWave(powerPath_,power_,status_.fileError))error=BuildReadFailed;
        break;
    case ReadEject:
        status_.inputPath=ejectPath_;
        if(!store_.ReadWave(ejectPath_,eject_,status_.fileError))error=BuildReadFailed;
        break;
    case DecodePower:
        status_.inputPath=powerPath_;error=ImageBuilder::Decode(power_,powerClip_);break;
    case DecodeEject:
        status_.inputPath=ejectPath_;error=ImageBuilder::Decode(eject_,ejectClip_);break;
    case Assemble:
        status_.inputPath.clear();error=ImageBuilder::Assemble(powerClip_,ejectClip_,image_);break;
    case Save:
        if(!store_.SaveImage(image_,status_.outputPath))error=BuildSaveFailed;
        else {status_.stage=Complete;status_.message=MsgBuildDone;}
        break;
    default: return;
    }
    if(error!=BuildOk){Fail(error);return;}
    step_=static_cast<Step>(step_+1);
    status_.percent=static_cast<unsigned>(step_)*100/Finished;
}
}
