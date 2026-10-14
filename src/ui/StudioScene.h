#pragma once
#include <xtl.h>
#include <xui.h>
#include <xuiapp.h>
#include "../platform/XboxServices.h"
#include "Locale.h"
#include "../platform/PreviewPlayer.h"
#include "CatalogueList.h"
namespace soundstudio {
extern bool exitRequested;
class StudioScene : public CXuiSceneImpl {
public:
    XUI_IMPLEMENT_CLASS(StudioScene, L"CodaScene", XUI_CLASS_SCENE)
    virtual ~StudioScene();
    static void TickActive();
protected:
    enum Screen { Home, Library, Confirm, Working, Result, Playback, ImportPicker, WorkshopReview, FilePreview, About };
    Screen screen_;
    bool backups_, aboutCredits_;
    bool choosingEject_, buildResult_, languageSaved_;
    bool refreshDown_[4];
    DWORD lastRefreshAt_;
    DWORD lastFramePoll_,scanStartedAt_;
    WORD navigationButtons_;
    DWORD navigationRepeatAt_;
    static StudioScene* active_;
    void OnFrame();
    LibraryScanner libraryScanner_;
    bool scanWaiting_, scanQueued_;
    unsigned scanGeneration_;
    std::string refreshSelection_;
    std::vector<ImageEntry> homeEntries_;
    std::vector<std::string> imports_;
    std::string powerWave_, ejectWave_;
    std::string selected_;
    XboxImageStore images_;
    OperationRunner operations_;
    PreviewPlayer preview_;
    bool previewNotice_;
    std::vector<ImageEntry> entries_;
    Locale locale_;
    CXuiTextElement subtitle_, actions_, notice_;
    CXuiTextElement homeTitle_, tagline_, collection_, collectionHint_, safetyTitle_, safetyBody_;
    CXuiTextElement cardNames_[3], cardInfo_[3];
    CXuiImageElement homeArt_, wave_;
    CXuiControl language_, workshop_, about_;
    CXuiTextElement aboutAuthor_, aboutBody_, aboutFooter_;
    CXuiTextElement title_, body_, detail_, progress_;
    CXuiControl a_, b_, x_, y_, lb_, exit_;
    CatalogueList list_;
    HRESULT OnInit(XUIMessageInit*, BOOL& handled);
    HRESULT OnPress(HXUIOBJ pressed, BOOL& handled);
    HRESULT OnTimer(XUIMessageTimer*, BOOL& handled);
    void ResetView();
    void HomeView();
    void AboutView(bool credits=false);
    void LibraryView(bool backups);
    void RefreshLibrary();
    void RenderLibrary();
    void PollLibraryScan();
    void Select();
    bool WorkView();
    void UpdateOperation();
    void PlaybackView();
    void ImportView(bool eject);
    void PickImport();
    void PreviewView();
    void PreviewSound(bool wave,unsigned clip=0);
    void Action(CXuiControl& control, const wchar_t* text);
    void Body(const std::string& text);
    XUI_BEGIN_MSG_MAP()
        XUI_ON_XM_INIT(OnInit)
        XUI_ON_XM_NOTIFY_PRESS(OnPress)
        XUI_ON_XM_TIMER(OnTimer)
    XUI_END_MSG_MAP()
};
class StudioApp : public CXuiModule {
public:
    virtual void RunFrame() { CXuiModule::RunFrame(); StudioScene::TickActive(); }
protected:
    HRESULT RegisterXuiClasses() { return StudioScene::Register(); }
    HRESULT UnregisterXuiClasses() { StudioScene::Unregister(); return S_OK; }
};
}
