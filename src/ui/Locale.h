#pragma once
#include "../core/SoundWorkflow.h"
namespace soundstudio {
enum TextId { TxtHomeTitle, TxtHomeBody, TxtSafety, TxtLibrary, TxtBackup, TxtTest, TxtRestoreBackup, TxtExit, TxtSavedTitle, TxtLibraryTitle, TxtRestoreHint, TxtImageHint, TxtBack, TxtNoBackups, TxtNoImages, TxtReview, TxtInvalidHint, TxtRestoreTitle, TxtInstallTitle, TxtInstallSteps, TxtInstallHint, TxtRestore, TxtInstall, TxtCancel, TxtWorkingTitle, TxtCancelBeforeErase, TxtTimerFailed, TxtDone, TxtBackupReadHint, TxtCompleteTitle, TxtCancelledTitle, TxtFailedTitle, TxtPlaybackTitle, TxtPlaybackBody, TxtPlaybackHint, TxtPlayPower, TxtPlayEject, TxtSubtitle, TxtActions, TxtSelectImage, TxtInvalidLabel, TxtBackupPrefix, TxtSaveLanguageFailed, TxtSafetyTitle, TxtStudioSafety, TxtCollection, TxtEmptyCollection, TxtCollectionHint, TxtEmptyCollectionHint, TxtStepSave, TxtStepInstall, TxtStepTest, TxtStepSaveInfo, TxtStepInstallInfo, TxtStepTestInfo, TxtImageValid, TxtImageInvalid, TxtWorkshop, TxtPickPower, TxtPickEject, TxtImportHint, TxtNoImports, TxtWorkshopReview, TxtBuildSteps, TxtBuildImage, TxtOpenLibrary, TxtOutputPrefix, TxtBuildingTitle, TxtRefresh, TxtRefreshResult, TxtRefreshScanning, TxtRefreshFailed, TxtFolderMissing, TxtFolderError, TxtScanPath, TxtScanTimeout, TxtFileError, TxtPreview, TxtStopPreview, TxtPreviewHint, TxtPreviewPlaying, TxtPreviewFailed, TxtPreviewDone, TxtAbout, TxtAboutBody, TxtAboutFooter, TxtCredits, TxtCreditsTitle, TxtCreditsBody, TxtCreditsFooter, TxtCount };
class Locale {
public:
    explicit Locale(bool russian=true) : russian_(russian) {}
    bool Russian() const { return russian_; }
    void SetRussian(bool value) { russian_=value; }
    const wchar_t* Text(TextId id) const;
    const wchar_t* Message(OperationMessage id) const;
    const wchar_t* StageText(Stage stage) const;
private: bool russian_;
};
}
