#pragma once
#include <string>
#include <vector>
namespace soundstudio {
// Presentation model shared by the image library and WAV picker.
class CatalogueModel {
public:
    enum { PageSize = 6 };
    CatalogueModel():selected_(-1) {}
    int Count() const { return (int)labels_.size(); }
    int Selected() const { return selected_; }
    int FirstVisible() const { return selected_<0?0:(selected_/PageSize)*PageSize; }
    const std::wstring& Label(int index) const { return labels_[index]; }
    void Insert(int index,int count) {
        if(index<0||index>Count()||count<=0)return;
        labels_.insert(labels_.begin()+index,count,L"");
        if(selected_<0)selected_=0;
        else if(index<=selected_)selected_+=count;
    }
    void Delete(int index,int count) {
        if(index<0||index>=Count()||count<=0)return;
        int end=index+count;if(end>Count())end=Count();
        labels_.erase(labels_.begin()+index,labels_.begin()+end);
        if(labels_.empty())selected_=-1;
        else if(selected_>=end)selected_-=end-index;
        else if(selected_>=index)selected_=index<Count()?index:Count()-1;
    }
    void SetLabel(int index,const wchar_t* text) { if(index>=0&&index<Count())labels_[index]=text?text:L""; }
    void Select(int index) { if(index>=0&&index<Count())selected_=index; }
    bool Move(int direction) {
        int next=selected_+direction;
        if(next<0||next>=Count()||next==selected_)return false;
        selected_=next;return true;
    }
private:
    std::vector<std::wstring> labels_;
    int selected_;
};
}
