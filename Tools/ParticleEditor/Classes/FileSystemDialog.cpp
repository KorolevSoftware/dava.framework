/*
 *  FileSystemDialog.cpp
 *  ParticlesEditor
 *
 *  Created by Alexey Prosin on 8/25/11.
 *  Copyright 2011 DAVA Consulting. All rights reserved.
 *
 */

#include "FileSystemDialog.h"

FileSystemDialog::FileSystemDialog(const String &_fontPath)
: UIControl(Rect(GetScreenWidth()/2, GetScreenHeight()/2, GetScreenWidth()*2/3, GetScreenHeight()*4/5))
{
    fontPath = _fontPath;
    
    background->SetDrawType(UIControlBackground::DRAW_FILL);
    background->SetColor(Color(0.5, 0.5, 0.5, 0.75));
    pivotPoint = size / 2;
    
    operationType = OPERATION_LOAD;
    delegate = NULL;
    extensionFilter.push_back("*");
    
    SetCurrentDir(FileSystem::Instance()->GetCurrentWorkingDirectory());
    
    cellH = GetScreenHeight()/20;
    cellH = Max(cellH, 32);
    int32 border = GetScreenHeight()/64;
    fileListView = new UIList(Rect(border, border + cellH, size.x - border*2 - cellH, size.y - cellH*3 - border*3), UIList::ORIENTATION_VERTICAL);
    fileListView->SetDelegate(this);
    fileListView->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
    fileListView->GetBackground()->SetColor(Color(0.25, 0.25, 0.25, 0.25));
    AddControl(fileListView);
    
    fileListScroll = new UIScrollBar(Rect(size.x - border - cellH, border + cellH, cellH, fileListView->size.y), UIScrollBar::ORIENTATION_VERTICAL);
    fileListScroll->GetBackground()->SetColor(Color(0.0, 0.0, 0.0, 0.05));
    fileListScroll->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
    fileListScroll->GetSlider()->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
    fileListScroll->GetSlider()->GetBackground()->SetColor(Color(1.0, 1.0, 1.0, 0.85));
    fileListScroll->SetDelegate(fileListView);
    AddControl(fileListScroll);
    
    lastSelectionTime = 0;
    
    Font *f = FTFont::Create(fontPath);
    f->SetSize(cellH * 2 / 3);
    f->SetColor(Color(1,1,1,1));
    
    title = new UIStaticText(Rect(border, border/2, size.x - border*2, cellH));
    title->SetFont(f);
    title->SetFittingOption(TextBlock::FITTING_REDUCE);
    title->SetText(L"Select file:");
    AddControl(title);

    workingPath = new UIStaticText(Rect(border, border/2 + fileListView->size.y + fileListView->relativePosition.y, size.x - border*2, cellH));
    workingPath->SetFont(f);
    workingPath->SetAlign(ALIGN_LEFT|ALIGN_VCENTER);
    workingPath->SetFittingOption(TextBlock::FITTING_REDUCE);
    workingPath->SetText(L"c:");
    AddControl(workingPath);
    
    
    int32 buttonW = cellH * 3;
    positiveButton = new UIButton(Rect(size.x - border - buttonW, workingPath->relativePosition.y + border/2 + cellH , buttonW, cellH));
    positiveButton->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    positiveButton->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.5, 0.6, 0.5, 0.5));
    positiveButton->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    positiveButton->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.75, 0.85, 0.75, 0.5));
    positiveButton->SetStateDrawType(UIControl::STATE_DISABLED, UIControlBackground::DRAW_FILL);
    positiveButton->GetStateBackground(UIControl::STATE_DISABLED)->SetColor(Color(0.2, 0.2, 0.2, 0.2));
    positiveButton->SetStateFont(UIControl::STATE_NORMAL, f);
    positiveButton->SetStateText(UIControl::STATE_NORMAL, L"OK");
	positiveButton->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &FileSystemDialog::ButtonPressed));
    AddControl(positiveButton);

    negativeButton = new UIButton(Rect(positiveButton->relativePosition.x - buttonW - border, positiveButton->relativePosition.y, buttonW, cellH));
    negativeButton->SetStateDrawType(UIControl::STATE_NORMAL, UIControlBackground::DRAW_FILL);
    negativeButton->GetStateBackground(UIControl::STATE_NORMAL)->SetColor(Color(0.6, 0.5, 0.5, 0.5));
    negativeButton->SetStateDrawType(UIControl::STATE_PRESSED_INSIDE, UIControlBackground::DRAW_FILL);
    negativeButton->GetStateBackground(UIControl::STATE_PRESSED_INSIDE)->SetColor(Color(0.85, 0.75, 0.75, 0.5));
    negativeButton->SetStateDrawType(UIControl::STATE_DISABLED, UIControlBackground::DRAW_FILL);
    negativeButton->GetStateBackground(UIControl::STATE_DISABLED)->SetColor(Color(0.2, 0.2, 0.2, 0.2));
    negativeButton->SetStateFont(UIControl::STATE_NORMAL, f);
    negativeButton->SetStateText(UIControl::STATE_NORMAL, L"Cancel");
	negativeButton->AddEvent(UIControl::EVENT_TOUCH_UP_INSIDE, Message(this, &FileSystemDialog::ButtonPressed));
    AddControl(negativeButton);
    

    textField = new UITextField(Rect(border, positiveButton->relativePosition.y, negativeButton->relativePosition.x - border*2, cellH));
    textField->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
    textField->GetBackground()->SetColor(Color(0.25, 0.25, 0.25, 0.25));
    textField->SetFont(f);
    textField->SetDelegate(this);
    
    SafeRelease(f);
    
    files = NULL;
}

void FileSystemDialog::ButtonPressed(BaseObject *obj, void *data, void *callerData)
{
    if (obj == negativeButton) 
    {
        Retain();
        GetParent()->RemoveControl(this);
        if (delegate)
        {
            delegate->OnFileSytemDialogCanceled(this);
        }
        Release();
    }
    else if (obj == positiveButton)
    {
        if (operationType == OPERATION_LOAD)
        {
            OnIndexSelected(lastSelectedIndex);
        }
        else if(operationType == OPERATION_CHOOSE_DIR)
        {
            if (lastSelectedIndex >= 0)
            {
                if (delegate)
                {
                    delegate->OnFileSelected(this, files->GetPathname(fileUnits[lastSelectedIndex].indexInFileList));
                }
            }
            else 
            {
                if (delegate)
                {
                    delegate->OnFileSelected(this, currentDir);
                }
            }

            GetParent()->RemoveControl(this);
        }
        else if(operationType == OPERATION_SAVE)
        {
            SaveFinishing();
        }
            

    }

}

void FileSystemDialog::SaveFinishing()
{
    if (!textField->GetText().empty())
    {
        if (delegate)
        {
            if (textField->GetText().find(L".") != textField->GetText().npos)
            {
                delegate->OnFileSelected(this, currentDir + "/" + WStringToString(textField->GetText()));
            }
            else 
            {
                delegate->OnFileSelected(this, currentDir + "/" + WStringToString(textField->GetText()) + extensionFilter[0]);
            }
            
        }
        GetParent()->RemoveControl(this);
    }
}


void FileSystemDialog::Show(UIControl *parentControl)
{
    parentControl->AddControl(this);
    RemoveControl(textField);
    switch (operationType) 
    {
        case OPERATION_LOAD:
            positiveButton->SetStateText(UIControl::STATE_NORMAL, L"Load");
            break;
        case OPERATION_SAVE:
            positiveButton->SetStateText(UIControl::STATE_NORMAL, L"Save");
            AddControl(textField);
            break;
        case OPERATION_CHOOSE_DIR:
            positiveButton->SetStateText(UIControl::STATE_NORMAL, L"Choose");
            break;
    }

    RefreshList();
}


void FileSystemDialog::SetCurrentDir(const String &newDirPath)
{
    Logger::Info("Setting path: %s", newDirPath.c_str());
    currentDir = newDirPath;
    if (GetParent()) 
    {
        RefreshList();
    }
}

const String &FileSystemDialog::GetCurrentDir()
{
    return currentDir;
}

void FileSystemDialog::SetExtensionFilter(const Vector<String> &newExtensionFilter)
{
    DVASSERT(!GetParent());
    extensionFilter.clear();
    extensionFilter = newExtensionFilter;
}

const Vector<String> &FileSystemDialog::GetExtensionFilter()
{
    return extensionFilter;
}

void FileSystemDialog::OnIndexSelected(int32 index)
{
    if (fileUnits[index].type == FUNIT_DIR_OUTSIDE) 
    {
        int32 p = currentDir.rfind("/");
        SetCurrentDir(String(currentDir, 0, p));
    }
    else if (fileUnits[index].type == FUNIT_DIR_INSIDE)
    {
        SetCurrentDir(files->GetPathname(fileUnits[index].indexInFileList));
    }
    else if (fileUnits[index].type == FUNIT_FILE)
    {
        if (operationType == OPERATION_LOAD) 
        {
            if (delegate)
            {
                delegate->OnFileSelected(this, files->GetPathname(fileUnits[index].indexInFileList));
            }
            GetParent()->RemoveControl(this);
        }
        else
        {
            SaveFinishing();
        }
        
    }
}

void FileSystemDialog::RefreshList()
{
    workingPath->SetText(StringToWString(currentDir));
    if (operationType != OPERATION_CHOOSE_DIR) 
    {
        positiveButton->SetDisabled(true);
    }
    else 
    {
        positiveButton->SetDisabled(false);
    }

    SafeRelease(files);
    lastSelected = NULL;
    lastSelectedIndex = -1;
    files = new FileList(currentDir);
    fileUnits.clear();
    int32 cnt = files->GetCount();
    int32 outCnt = 0;
    int32 p = currentDir.npos;

    while (true) 
    {
        p = currentDir.rfind("/", p);
        if(p != currentDir.npos)
        {
            p--;
            outCnt++;
            if (p <= 0) 
            {
                break;
            }
        }
        else 
        {
            break;
        }
    }

    if(outCnt > 1)
    {
        DialogFileUnit fud;
        fud.name = "..";
        fud.type = FUNIT_DIR_OUTSIDE;
        fud.indexInFileList = -1;
        fileUnits.push_back(fud);
    }
        
    for (int i = 0; i < cnt; i++)
    {
        if (!files->IsNavigationDirectory(i))
        {
            DialogFileUnit fu;
            fu.name = files->GetFilename(i);
            fu.indexInFileList = i;
            fu.type = FUNIT_FILE;
            if (files->IsDirectory(i)) 
            {
                fu.type = FUNIT_DIR_INSIDE;
            }
            else 
            {
                if (operationType == OPERATION_CHOOSE_DIR) 
                {
                    continue;
                }
                String ext = FileSystem::GetExtension(fu.name);
                bool isPresent = false;
                for (int n = 0; n < extensionFilter.size(); n++) 
                {
                    if (extensionFilter[n] == "*" || ext == extensionFilter[n])
                    {
                        isPresent = true;
                        break;
                    }
                }
                if (!isPresent) 
                {
                    continue;
                }
            }
            
            fileUnits.push_back(fu);
        }
    }
    fileListView->ResetScrollPosition();
    fileListView->RefreshList();
}


void FileSystemDialog::TextFieldShouldReturn(UITextField * textField)
{
    SaveFinishing();
}

bool FileSystemDialog::TextFieldKeyPressed(UITextField * textField, int32 replacementLocation, int32 replacementLength, const WideString & replacementString)
{
    if (textField->GetText().size() + replacementLength > 0) 
    {
        positiveButton->SetDisabled(false);
    }
    else 
    {
        positiveButton->SetDisabled(true);
    }

    return true;
}


int32 FileSystemDialog::ElementsCount(UIList *forList)
{
    return fileUnits.size();
}

UIListCell *FileSystemDialog::CellAtIndex(UIList *forList, int32 index)
{
    UIListCell *c = forList->GetReusableCell("File cell"); //try to get cell from the reusable cells store
    if(!c)
    { //if cell of requested type isn't find in the store create new cell
        c = new UIListCell(Rect(0, 0, forList->size.x, cellH), "File cell");
        UIStaticText *text = new UIStaticText(Rect(0, 0, forList->size.x, cellH));
        c->AddControl(text);
        text->SetName("CellText");
        text->SetFittingOption(TextBlock::FITTING_REDUCE);
        text->SetAlign(ALIGN_LEFT|ALIGN_VCENTER);
        Font *f = FTFont::Create(fontPath);
        f->SetSize(cellH * 2 / 3);
        f->SetColor(Color(1,1,1,1));
        text->SetFont(f);
        SafeRelease(f);
        c->GetBackground()->SetColor(Color(0.75, 0.75, 0.75, 0.5));
    }
    UIStaticText *t = (UIStaticText *)c->FindByName("CellText");
    if (fileUnits[index].type == FUNIT_FILE) 
    {
        t->SetText(StringToWString(fileUnits[index].name));
    }
    else 
    {
        t->SetText(StringToWString("[" + fileUnits[index].name + "]"));
    }
    
    if (index != lastSelectedIndex) 
    {
        c->GetBackground()->SetDrawType(UIControlBackground::DRAW_ALIGNED);
    }
    else
    {
        c->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
    }

    return c;
}

int32 FileSystemDialog::CellHeight(UIList *forList, int32 index)
{
    return cellH;
}

void FileSystemDialog::OnCellSelected(UIList *forList, UIListCell *selectedCell)
{
    if (lastSelected) 
    {
        lastSelected->GetBackground()->SetDrawType(UIControlBackground::DRAW_ALIGNED);
    }
    uint64 curTime = SystemTimer::Instance()->AbsoluteMS();
    if (curTime - lastSelectionTime < 330 && lastSelected == selectedCell)
    {
        lastSelected = selectedCell;
        lastSelectedIndex = lastSelected->GetIndex();
        lastSelected->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
        lastSelectionTime = curTime;
        OnIndexSelected(lastSelectedIndex);
    }
    else 
    {
        lastSelected = selectedCell;
        lastSelectedIndex = lastSelected->GetIndex();
        lastSelected->GetBackground()->SetDrawType(UIControlBackground::DRAW_FILL);
        lastSelectionTime = curTime;
        if (operationType == OPERATION_LOAD) 
        {
            if (fileUnits[selectedCell->GetIndex()].type == FUNIT_FILE)
            {
                positiveButton->SetDisabled(false);
            }
            else 
            {
                positiveButton->SetDisabled(true);
            }
        }
        else if (operationType == OPERATION_SAVE)
        {
            if (fileUnits[selectedCell->GetIndex()].type == FUNIT_FILE)
            {
                textField->SetText(StringToWString(files->GetFilename(fileUnits[lastSelectedIndex].indexInFileList)));
                positiveButton->SetDisabled(false);
            }
        }
    }
}

