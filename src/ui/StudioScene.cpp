#include "StudioScene.h"
#include <cstdio>
#include <cmath>
namespace soundstudio {
bool exitRequested = false;
StudioScene* StudioScene::active_=NULL;
StudioScene::~StudioScene(){if(active_==this)active_=NULL;}
void StudioScene::TickActive(){if(active_)active_->OnFrame();}
static std::wstring Wide(const std::string& text) {
    int count = MultiByteToWideChar(CP_ACP, 0, text.c_str(), -1, NULL, 0);
    if (count <= 0) return L"";
    std::vector<wchar_t> value(count);
    MultiByteToWideChar(CP_ACP, 0, text.c_str(), -1, &value[0], count);
    return std::wstring(&value[0]);
}
void StudioScene::Action(CXuiControl& control, const wchar_t* text) {
    control.SetText(text); control.SetEnable(true); control.SetShow(true);
}
void StudioScene::Body(const std::string& text) { body_.SetText(Wide(text).c_str()); }
void StudioScene::ResetView() {
    preview_.Stop();previewNotice_=false;
    ++scanGeneration_;scanWaiting_=false;scanQueued_=false;
    notice_.SetText(L""); notice_.SetShow(false);
    // Restore default action positions before applying screen-specific layouts.
    D3DXVECTOR3 xPosition(20.0f,410.0f,0.0f); x_.SetPosition(&xPosition);
    KillTimer(2); homeArt_.SetShow(false); wave_.SetShow(false);
    homeTitle_.SetShow(false); tagline_.SetShow(false); collection_.SetShow(false); collectionHint_.SetShow(false);
    for (unsigned i=0; i<3; ++i) { cardNames_[i].SetShow(false); cardInfo_[i].SetShow(false); }
    title_.SetShow(true);aboutAuthor_.SetShow(false);aboutBody_.SetShow(false);aboutFooter_.SetShow(false);
    CXuiControl* controls[] = { &a_, &b_, &x_, &y_, &lb_, &exit_, &language_, &workshop_, &about_ };
    for (unsigned i = 0; i < sizeof(controls)/sizeof(controls[0]); ++i) {
        controls[i]->SetShow(false); controls[i]->SetEnable(false);
    }
    navigationButtons_=0;navigationRepeatAt_=0;
    list_.SetShow(false); list_.SetEnable(false);
    body_.SetShow(true); body_.SetText(L""); detail_.SetText(L""); progress_.SetText(L"");
}
HRESULT StudioScene::OnInit(XUIMessageInit*, BOOL& handled) {
    GetChildById(L"title", &title_); GetChildById(L"body", &body_);
    GetChildById(L"detail", &detail_); GetChildById(L"progress", &progress_);
    GetChildById(L"a", &a_); GetChildById(L"b", &b_);
    GetChildById(L"x", &x_); GetChildById(L"y", &y_);
    GetChildById(L"lb", &lb_); GetChildById(L"exit", &exit_); GetChildById(L"library", &list_);
    list_.Initialize(*this);
    GetChildById(L"language", &language_); GetChildById(L"subtitle", &subtitle_); GetChildById(L"actions", &actions_);
    GetChildById(L"workshop", &workshop_);
    GetChildById(L"about", &about_);GetChildById(L"aboutAuthor",&aboutAuthor_);
    GetChildById(L"aboutBody",&aboutBody_);GetChildById(L"aboutFooter",&aboutFooter_);
    GetChildById(L"notice", &notice_);
    locale_.SetRussian(LanguageSettings::LoadRussian());
    GetChildById(L"homeArt", &homeArt_); GetChildById(L"wave", &wave_);
    GetChildById(L"homeTitle", &homeTitle_); GetChildById(L"tagline", &tagline_);
    GetChildById(L"libraryHeading", &collection_); GetChildById(L"libraryHint", &collectionHint_);
    GetChildById(L"safetyTitle", &safetyTitle_); GetChildById(L"safetyBody", &safetyBody_);
    for (unsigned i=0; i<3; ++i) {
        wchar_t id[32]; swprintf_s(id,32,L"cardName%u",i); GetChildById(id,&cardNames_[i]);
        swprintf_s(id,32,L"cardInfo%u",i); GetChildById(id,&cardInfo_[i]);
    }
    scanGeneration_=0;scanWaiting_=false;scanQueued_=false;
    lastFramePoll_=GetTickCount();scanStartedAt_=0;
    lastRefreshAt_=GetTickCount()-250;
    screen_ = Home; backups_ = false; buildResult_=false; choosingEject_=false; languageSaved_=true; HomeView();active_=this; handled = TRUE; return S_OK;
}
void StudioScene::HomeView() {
    ResetView(); screen_ = Home; buildResult_=false; title_.SetText(locale_.Text(TxtHomeTitle));
    subtitle_.SetText(locale_.Text(TxtSubtitle)); actions_.SetText(locale_.Text(TxtActions));
    list_.CXuiControl::SetText(locale_.Text(TxtSelectImage));
    Action(language_, locale_.Russian() ? L"English" : L"\u0420\u0443\u0441\u0441\u043a\u0438\u0439");
    safetyTitle_.SetText(locale_.Text(TxtSafetyTitle)); safetyBody_.SetText(locale_.Text(TxtStudioSafety));
    body_.SetShow(false); title_.SetShow(false);
    homeArt_.SetShow(true); wave_.SetShow(true); homeTitle_.SetShow(true); tagline_.SetShow(true);
    homeTitle_.SetText(locale_.Text(TxtHomeTitle)); tagline_.SetText(locale_.Text(TxtHomeBody));
    collection_.SetShow(true); collectionHint_.SetShow(true);
    if(!libraryScanner_.Active())homeEntries_=images_.List(false);
    entries_=homeEntries_;
    collection_.SetText(locale_.Text(entries_.empty()?TxtEmptyCollection:TxtCollection));
    collectionHint_.SetText(locale_.Text(entries_.empty()?TxtEmptyCollectionHint:TxtCollectionHint));
    TextId emptyNames[]={TxtStepSave,TxtStepInstall,TxtStepTest};
    TextId emptyInfo[]={TxtStepSaveInfo,TxtStepInstallInfo,TxtStepTestInfo};
    for (unsigned i=0; i<3; ++i) {
        cardNames_[i].SetShow(true); cardInfo_[i].SetShow(true);
        if (entries_.empty()) {
            cardNames_[i].SetText(locale_.Text(emptyNames[i])); cardInfo_[i].SetText(locale_.Text(emptyInfo[i]));
        } else if (i<entries_.size()) {
            cardNames_[i].SetText(Wide(entries_[i].name).c_str());
            cardInfo_[i].SetText(locale_.Text(entries_[i].valid?TxtImageValid:TxtImageInvalid));
        } else { cardNames_[i].SetText(L"\u2014"); cardInfo_[i].SetText(L""); }
    }
    SetTimer(2,33);
    Action(a_, locale_.Text(TxtLibrary)); Action(x_, locale_.Text(TxtBackup)); Action(y_, locale_.Text(TxtTest));
    Action(lb_, locale_.Text(TxtRestoreBackup)); Action(exit_, locale_.Text(TxtExit)); a_.SetFocus();
    Action(workshop_,locale_.Text(TxtWorkshop));Action(about_,locale_.Text(TxtAbout));
    if (!languageSaved_) { notice_.SetText(locale_.Text(TxtSaveLanguageFailed)); notice_.SetShow(true); }
}
void StudioScene::AboutView(bool credits) {
    ResetView();screen_=About;aboutCredits_=credits;
    title_.SetText(locale_.Text(credits?TxtCreditsTitle:TxtAbout));body_.SetShow(false);
    aboutAuthor_.SetText(L"Aleksandr Nikitin\naka Niuanli");aboutAuthor_.SetShow(true);
    aboutBody_.SetText(locale_.Text(credits?TxtCreditsBody:TxtAboutBody));aboutBody_.SetShow(true);
    aboutFooter_.SetText(locale_.Text(credits?TxtCreditsFooter:TxtAboutFooter));aboutFooter_.SetShow(true);
    Action(x_,locale_.Text(credits?TxtAbout:TxtCredits));Action(b_,locale_.Text(TxtBack));b_.SetFocus();
}
void StudioScene::LibraryView(bool backups) {
    ResetView(); screen_ = Library; backups_ = backups; entries_.clear();
    title_.SetText(backups ? locale_.Text(TxtSavedTitle) : locale_.Text(TxtLibraryTitle));
    detail_.SetText(backups ? locale_.Text(TxtRestoreHint)
                            : locale_.Text(TxtImageHint));
    list_.DeleteItems(0, list_.GetItemCount()); Action(b_, locale_.Text(TxtBack));
    Action(x_, locale_.Text(TxtRefresh));
    D3DXVECTOR3 refreshPosition(20.0f,258.0f,0.0f); x_.SetPosition(&refreshPosition);
    // Read physical button state without consuming XUI's keystroke queue.
    // This also works while the list owns focus and eats shortcut messages.
    for(DWORD user=0;user<4;++user) {
        XINPUT_STATE input; ZeroMemory(&input,sizeof(input));
        refreshDown_[user]=XInputGetState(user,&input)==ERROR_SUCCESS && (input.Gamepad.wButtons&XINPUT_GAMEPAD_X)!=0;
    }
    RenderLibrary();
    lastRefreshAt_=GetTickCount()-250;RefreshLibrary();
}
void StudioScene::RenderLibrary() {
    list_.DeleteItems(0,list_.GetItemCount());
    Action(x_,locale_.Text(TxtRefresh));
    if (entries_.empty()) {
        body_.SetText(backups_ ? locale_.Text(TxtNoBackups)
                            : locale_.Text(TxtNoImages));
        list_.SetShow(false);list_.SetEnable(false);a_.SetShow(false);a_.SetEnable(false);
        body_.SetShow(true);y_.SetShow(false);y_.SetEnable(false);x_.SetFocus(); return;
    }
    for (unsigned i = 0; i < entries_.size(); ++i) {
        list_.InsertItems(i, 1);
        std::wstring label = (entries_[i].valid ? L"" : locale_.Text(TxtInvalidLabel)) + Wide(entries_[i].name);
        list_.SetText(i, label.c_str());
    }
    body_.SetShow(false); list_.SetEnable(true); list_.SetShow(true);
    list_.SetCurSel(0); list_.SetFocus(); Action(a_, locale_.Text(TxtReview));Action(y_,locale_.Text(TxtPreview));
}
void StudioScene::RefreshLibrary() {
    if(scanWaiting_)return;
    DWORD now=GetTickCount();
    if((DWORD)(now-lastRefreshAt_)<250) return;
    lastRefreshAt_=now;
    int index=list_.GetCurSel();refreshSelection_.clear();
    if(index>=0 && index<(int)entries_.size())refreshSelection_=entries_[index].path;
    ++scanGeneration_;scanWaiting_=true;scanQueued_=true;
    scanStartedAt_=now;
    // Only queue work here. Never reset the scene or kill/recreate timer 3
    // from inside its callback: that can invalidate XUI's active timer iterator.
}
void StudioScene::PollLibraryScan() {
    std::vector<ImageEntry> result;unsigned generation=0;DWORD error=0;
    if(libraryScanner_.Take(result,generation,error) && generation==scanGeneration_ && scanWaiting_) {
        entries_.swap(result);if(!backups_)homeEntries_=entries_;
        scanWaiting_=false;RenderLibrary();
        for(unsigned i=0;i<entries_.size();++i)if(entries_[i].path==refreshSelection_){list_.SetCurSel(i);list_.SetFocus();break;}
        wchar_t message[180];swprintf_s(message,180,locale_.Text(TxtRefreshResult),(unsigned)entries_.size());
        notice_.SetText(message);notice_.SetShow(true);
        if(error) {
            const wchar_t* path=backups_?L"GAME:\\Dumps":L"GAME:\\Sounds";
            bool missing=error==ERROR_FILE_NOT_FOUND||error==ERROR_PATH_NOT_FOUND;
            body_.SetText(locale_.Text(missing?TxtFolderMissing:TxtFolderError));body_.SetShow(true);
            list_.SetShow(false);list_.SetEnable(false);a_.SetEnable(false);a_.SetShow(false);
            swprintf_s(message,180,L"%s  %s  (0x%08X)",locale_.Text(TxtScanPath),path,error);
            notice_.SetText(message);
        }
    }
    if(scanQueued_ && !libraryScanner_.Active()) {
        scanQueued_=false;
        // Keep visible controls in their normal XUI states. Request/selection
        // guards already reject duplicate refreshes and selection while busy.
        // Leave focus usable while the request guard ignores repeat presses.
        wchar_t message[256];swprintf_s(message,256,locale_.Text(TxtRefreshScanning),backups_?L"GAME:\\Dumps":L"GAME:\\Sounds");
        notice_.SetText(message);notice_.SetShow(true);
        if(!libraryScanner_.Start(backups_,scanGeneration_)) {
            scanWaiting_=false;RenderLibrary();
            notice_.SetText(locale_.Text(TxtRefreshFailed));notice_.SetShow(true);
        }
    }
    if(scanWaiting_ && (DWORD)(GetTickCount()-scanStartedAt_)>=5000) {
        ++scanGeneration_;scanWaiting_=false;scanQueued_=false;RenderLibrary();
        notice_.SetText(locale_.Text(TxtScanTimeout));notice_.SetShow(true);
    }
}
void StudioScene::Select() {
    int index = list_.GetCurSel(); if (index < 0 || index >= (int)entries_.size()) return;
    if (!entries_[index].valid) {
        detail_.SetText(locale_.Text(TxtInvalidHint)); return;
    }
    selected_ = entries_[index].path; std::string name = entries_[index].name;
    ResetView(); screen_ = Confirm; title_.SetText(backups_ ? locale_.Text(TxtRestoreTitle) : locale_.Text(TxtInstallTitle));
    body_.SetText((Wide(name) + locale_.Text(TxtInstallSteps)).c_str());
    detail_.SetText(locale_.Text(TxtInstallHint));
    Action(a_, backups_ ? locale_.Text(TxtRestore) : locale_.Text(TxtInstall)); Action(b_, locale_.Text(TxtCancel)); b_.SetFocus();
}
bool StudioScene::WorkView() {
    ResetView(); screen_ = Working; title_.SetText(locale_.Text(buildResult_?TxtBuildingTitle:TxtWorkingTitle));
    Action(b_, locale_.Text(buildResult_?TxtCancel:TxtCancelBeforeErase)); b_.SetFocus();
    // This timer only observes the worker; hardware never blocks the rendering thread.
    if (FAILED(SetTimer(1, 50))) {
        screen_ = Result; body_.SetText(locale_.Text(TxtTimerFailed));
        b_.SetShow(false); b_.SetEnable(false); Action(a_, locale_.Text(TxtDone)); a_.SetFocus(); return false;
    }
    return true;
}
void StudioScene::UpdateOperation() {
    OperationStatus status = operations_.Snapshot();
    std::wstring message=locale_.Message(status.message);
    if(!status.inputPath.empty()) {
        message+=L"\n\n";message+=Wide(status.inputPath);
        if(status.fileError){wchar_t code[32];swprintf_s(code,32,L"%u",status.fileError);message+=L"\n";message+=locale_.Text(TxtFileError);message+=code;}
    }
    body_.SetText(message.c_str());
    if (!status.outputPath.empty()) detail_.SetText((std::wstring(locale_.Text(TxtOutputPrefix)) + Wide(status.outputPath)).c_str());
    else if (!status.backupPath.empty()) detail_.SetText((std::wstring(locale_.Text(TxtBackupPrefix)) + Wide(status.backupPath)).c_str());
    else if (buildResult_) detail_.SetText(locale_.Text(TxtImportHint));
    else detail_.SetText(locale_.Text(TxtBackupReadHint));
    wchar_t value[180];
    unsigned filled = status.percent / 5; std::wstring bar;
    for (unsigned i = 0; i < 20; ++i) bar += i < filled ? L'=' : L'-';
    swprintf_s(value, 180, L"%s   [%s]   %u%%", buildResult_?locale_.Text(TxtBuildImage):locale_.StageText(status.stage), bar.c_str(), status.percent);
    progress_.SetText(value);
    bool busy = operations_.Running();
    b_.SetEnable(busy && !status.chipMayBeModified); b_.SetShow(busy && !status.chipMayBeModified);
    if (busy) return;
    KillTimer(1); screen_ = Result;
    title_.SetText(status.stage == Complete ? locale_.Text(TxtCompleteTitle) : status.stage == Cancelled ? locale_.Text(TxtCancelledTitle) : locale_.Text(TxtFailedTitle));
    Action(a_, locale_.Text(TxtDone)); a_.SetFocus();
    if (status.chipMayBeModified && status.stage == Failed) Action(y_, locale_.Text(TxtRestoreBackup));
    else if (status.stage == Complete) Action(y_, locale_.Text(buildResult_?TxtOpenLibrary:TxtTest));
}
static std::string FileName(const std::string& path) {
    size_t slash=path.find_last_of("\\/");std::string name=slash==std::string::npos?path:path.substr(slash+1);
    if(name.size()>56) name=name.substr(0,40)+"..."+name.substr(name.size()-10);
    return name;
}
void StudioScene::ImportView(bool eject) {
    ResetView();screen_=ImportPicker;choosingEject_=eject;
    title_.SetText(locale_.Text(eject?TxtPickEject:TxtPickPower));detail_.SetText(locale_.Text(TxtImportHint));
    imports_=XboxWorkshopFiles::ListImports();list_.DeleteItems(0,list_.GetItemCount());
    list_.CXuiControl::SetText(L"WAV / Imports");
    Action(b_,locale_.Text(TxtBack));
    if(imports_.empty()){body_.SetText(locale_.Text(TxtNoImports));b_.SetFocus();return;}
    for(unsigned i=0;i<imports_.size();++i){list_.InsertItems(i,1);list_.SetText(i,Wide(FileName(imports_[i])).c_str());}
    body_.SetShow(false);list_.SetShow(true);list_.SetEnable(true);list_.SetCurSel(0);list_.SetFocus();Action(a_,locale_.Text(TxtReview));Action(y_,locale_.Text(TxtPreview));
}
void StudioScene::PickImport() {
    int index=list_.GetCurSel();if(index<0||index>=(int)imports_.size())return;
    if(!choosingEject_){powerWave_=imports_[index];ImportView(true);return;}
    ejectWave_=imports_[index];ResetView();screen_=WorkshopReview;
    title_.SetText(locale_.Text(TxtWorkshopReview));
    std::wstring summary=L"1  "+Wide(FileName(powerWave_))+L"\n2  "+Wide(FileName(ejectWave_))+locale_.Text(TxtBuildSteps);
    body_.SetText(summary.c_str());detail_.SetText(locale_.Text(TxtImportHint));
    Action(a_,locale_.Text(TxtBuildImage));Action(b_,locale_.Text(TxtBack));a_.SetFocus();
}
void StudioScene::PreviewView() {
    int index=list_.GetCurSel();if(scanWaiting_||index<0||index>=(int)entries_.size())return;
    selected_=entries_[index].path;
    ResetView();screen_=FilePreview;title_.SetText(locale_.Text(TxtPreview));
    Body(FileName(selected_));detail_.SetText(locale_.Text(TxtPreviewHint));
    D3DXVECTOR3 xPosition(20.0f,258.0f,0.0f);x_.SetPosition(&xPosition);
    Action(a_,locale_.Text(TxtPlayPower));Action(x_,locale_.Text(TxtPlayEject));
    Action(y_,locale_.Text(TxtStopPreview));Action(b_,locale_.Text(TxtBack));a_.SetFocus();
}
void StudioScene::PreviewSound(bool wave,unsigned clip) {
    bool ok=false;
    if(wave){int index=list_.GetCurSel();if(index<0||index>=(int)imports_.size())return;ok=preview_.PlayWave(imports_[index]);}
    else ok=preview_.PlayImage(selected_,clip);
    previewNotice_=ok;
    notice_.SetText(locale_.Text(ok?TxtPreviewPlaying:TxtPreviewFailed));notice_.SetShow(true);
}
void StudioScene::PlaybackView() {
    ResetView(); screen_ = Playback; title_.SetText(locale_.Text(TxtPlaybackTitle));
    D3DXVECTOR3 ejectPosition(20.0f,258.0f,0.0f); x_.SetPosition(&ejectPosition);
    body_.SetText(locale_.Text(TxtPlaybackBody));
    detail_.SetText(locale_.Text(TxtPlaybackHint));
    Action(a_, locale_.Text(TxtPlayPower)); Action(x_, locale_.Text(TxtPlayEject)); Action(b_, locale_.Text(TxtBack)); a_.SetFocus();
}
HRESULT StudioScene::OnPress(HXUIOBJ pressed, BOOL& handled) {
    handled = TRUE;
    if (screen_ == Working) { if (pressed == b_) operations_.RequestCancel(); return S_OK; }
    if (pressed == language_ && screen_ == Home) {
        locale_.SetRussian(!locale_.Russian());
        languageSaved_ = LanguageSettings::SaveRussian(locale_.Russian());
        HomeView(); language_.SetFocus();
        return S_OK;
    }
    if (pressed == exit_) { exitRequested = true; return S_OK; }
    switch (screen_) {
    case Home:
        if (pressed == about_) AboutView();
        else if (pressed == a_) LibraryView(false);
        else if (pressed == lb_) LibraryView(true);
        else if (pressed == x_ && WorkView()) { operations_.Start(false, ""); UpdateOperation(); }
        else if (pressed == y_) PlaybackView();
        else if (pressed == workshop_) {powerWave_.clear();ejectWave_.clear();ImportView(false);}
        break;
    case About:
        if(pressed==b_)HomeView();
        else if(pressed==x_)AboutView(!aboutCredits_);
        break;
    case ImportPicker:
        if(pressed==y_){if(preview_.Playing()){preview_.Stop();previewNotice_=false;notice_.SetShow(false);}else PreviewSound(true);}
        else if(pressed==a_||pressed==list_)PickImport();
        else if(pressed==b_){if(choosingEject_)ImportView(false);else HomeView();}
        break;
    case WorkshopReview:
        if(pressed==b_)ImportView(true);
        else if(pressed==a_){buildResult_=true;if(WorkView()){operations_.StartBuild(powerWave_,ejectWave_);UpdateOperation();}}
        break;
    case FilePreview:
        if(pressed==b_)LibraryView(backups_);
        else if(pressed==y_){preview_.Stop();previewNotice_=false;notice_.SetShow(false);}
        else if(pressed==a_||pressed==x_)PreviewSound(false,pressed==a_?0:1);
        break;
    case Library:
        if ((pressed == list_ || pressed == a_) && !scanWaiting_) Select();
        else if (pressed == y_) PreviewView();
        else if (pressed == x_) RefreshLibrary();
        else if (pressed == b_) HomeView();
        break;
    case Confirm:
        if (pressed == b_) LibraryView(backups_);
        else if (pressed == a_ && WorkView()) { operations_.Start(true, selected_); UpdateOperation(); }
        break;
    case Result:
        if (pressed == a_) HomeView();
        else if (pressed == y_) {
            if(buildResult_){buildResult_=false;LibraryView(false);break;}
            OperationStatus status = operations_.Snapshot();
            if (status.chipMayBeModified && status.stage == Failed) LibraryView(true); else PlaybackView();
        }
        break;
    case Playback:
        if (pressed == b_) HomeView();
        else if (pressed == a_ || pressed == x_) {
            if (WorkView()) { operations_.StartPlayback(pressed == a_ ? 5 : 6); UpdateOperation(); }
        }
        break;
    default: break;
    }
    return S_OK;
}
void StudioScene::OnFrame() {
    DWORD now=GetTickCount();if((DWORD)(now-lastFramePoll_)<50)return;
    lastFramePoll_=now;
    if(previewNotice_&&!preview_.Playing()){preview_.Stop();previewNotice_=false;notice_.SetText(locale_.Text(TxtPreviewDone));notice_.SetShow(true);}

    if(screen_==Library || screen_==ImportPicker) {
        if(screen_==Library)PollLibraryScan();
        bool refresh=false;
        WORD navigation=0;
        for(DWORD user=0;user<4;++user) {
            XINPUT_STATE input;ZeroMemory(&input,sizeof(input));
            bool down=XInputGetState(user,&input)==ERROR_SUCCESS && (input.Gamepad.wButtons&XINPUT_GAMEPAD_X)!=0;
            navigation|=input.Gamepad.wButtons&(XINPUT_GAMEPAD_DPAD_UP|XINPUT_GAMEPAD_DPAD_DOWN);
            if(input.Gamepad.sThumbLY>16000)navigation|=XINPUT_GAMEPAD_DPAD_UP;
            if(input.Gamepad.sThumbLY<-16000)navigation|=XINPUT_GAMEPAD_DPAD_DOWN;
            if(down && !refreshDown_[user])refresh=true;
            refreshDown_[user]=down;
        }
        if(refresh && screen_==Library)RefreshLibrary();
        bool changed=navigation!=navigationButtons_;
        if(navigation && (changed || (LONG)(now-navigationRepeatAt_)>=0)) {
            if(!scanWaiting_ && list_.GetItemCount()>0 && navigation!=(XINPUT_GAMEPAD_DPAD_UP|XINPUT_GAMEPAD_DPAD_DOWN)) {
                list_.MoveSelection(navigation==XINPUT_GAMEPAD_DPAD_UP?-1:1);list_.SetFocus();
            }
            navigationRepeatAt_=now+(changed?350:120);
        }
        navigationButtons_=navigation;
    }
}
HRESULT StudioScene::OnTimer(XUIMessageTimer* timer, BOOL& handled) {
    if (timer->nId==2 && screen_==Home) wave_.SetOpacity(0.85f+0.15f*(float)std::sin(GetTickCount()*0.003));
    else if (screen_ == Working) UpdateOperation(); handled = TRUE; return S_OK;
}
}
