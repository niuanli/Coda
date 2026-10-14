#pragma once
#include "CatalogueModel.h"
#include <cstdio>
namespace soundstudio {
// Fixed native elements avoid CommonList's dynamic layout/recycling during Render.
class CatalogueList : public CXuiControl {
public:
    CatalogueList():visible_(false) {}
    void Initialize(CXuiSceneImpl& scene) {
        for(unsigned i=0;i<CatalogueModel::PageSize;++i) {
            wchar_t id[32];swprintf_s(id,32,L"catalogueRow%u",i);scene.GetChildById(id,&rows_[i]);
        }
        scene.GetChildById(L"catalogueSelection",&selection_);
        scene.GetChildById(L"cataloguePage",&page_);Paint();
    }
    void SetShow(bool visible) { visible_=visible;CXuiControl::SetShow(false);Paint(); }
    // Navigation is handled by the scene; the native placeholder stays hidden.
    void SetFocus() {}
    int GetItemCount() const { return model_.Count(); }
    int GetCurSel() const { return model_.Selected(); }
    void InsertItems(int index,int count) { model_.Insert(index,count);Paint(); }
    void DeleteItems(int index,int count) { model_.Delete(index,count);Paint(); }
    void SetText(int index,const wchar_t* text) { model_.SetLabel(index,text);Paint(); }
    void SetCurSel(int index) { model_.Select(index);Paint(); }
    bool MoveSelection(int direction) { if(!model_.Move(direction))return false;Paint();return true; }
private:
    CatalogueModel model_;
    bool visible_;
    CXuiTextElement rows_[CatalogueModel::PageSize],page_;
    CXuiElement selection_;
    void Paint() {
        int first=model_.FirstVisible();
        for(int row=0;row<CatalogueModel::PageSize;++row) {
            bool visible=visible_ && first+row<model_.Count();rows_[row].SetShow(visible);
            if(visible) {
                std::wstring label=model_.Label(first+row);
                if(label.size()>72)label=label.substr(0,69)+L"...";
                rows_[row].SetText(label.c_str());
            }
        }
        selection_.SetShow(visible_ && model_.Selected()>=0);page_.SetShow(visible_);
        if(model_.Selected()>=0) {
            D3DXVECTOR3 position(300.0f,289.0f+38.0f*(model_.Selected()-first),0.0f);
            selection_.SetPosition(&position);
        }
        wchar_t page[64];int last=first+CatalogueModel::PageSize;if(last>model_.Count())last=model_.Count();
        swprintf_s(page,64,L"%d\u2013%d / %d",model_.Count()?first+1:0,last,model_.Count());page_.SetText(page);
    }
};
}
