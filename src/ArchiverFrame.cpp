/*
MIT License

Copyright (c) 2026 Dr. R. Klenk (a.k.a. RobseK, RobseK101)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "ArchiverFrame.h"
#include <wx/wx.h>
#include <string>
#include <wx/wfstream.h>
#include <wx/aboutdlg.h>
#include <wx/filefn.h>
#include <stdexcept>
#include <ren/ArchiveRCS.h>
#include <ren/ArchiveGTA3.h>
#include <ren/Exceptions.hpp>
#include <vector>
#include "AboutDialog.h"
#include "ren/Stringtools.hpp"
#include <algorithm>

#define RK_APPNAME "Linear Archive Viewer and Extractor"
#define RK_APPSHORTNAME "L.A.V.E."
#define RK_FULLAPPNAME RK_APPNAME " (" RK_APPSHORTNAME ")"
#define RK_APPVERSION "2.0 (2025-09-05)"
#define RK_APPAUTHOR "Dr. Robert Klenk"
#define RK_APPCOPYRIGHT "(c) 2025"
#define RK_APPCOPYRIGHTNOTICE RK_APPCOPYRIGHT " " RK_APPAUTHOR
#define RK_APPDESCRIPTION "A simple viewer end extractor for linear (i.e. non-branching) data archives."

#define RCS_OPEN_MSG "Open Davilex IMG file"
#define GTA3_OPEN_MSG "Open GTA III/VC IMG file"

#define RCS_OPEN_FILTER "Davilex IMG|*.img"
#define GTA3_OPEN_FILTER "GTA III IMG|*.img"

wxBEGIN_EVENT_TABLE(ArchiverFrame, wxFrame)
EVT_MENU(ID_MENUOPENRCS, ArchiverFrame::OnMenuFileOpenRcs)
EVT_MENU(ID_MENUOPENGTA3, ArchiverFrame::OnMenuFileOpenGta3)
EVT_MENU(ID_MENUOPENDIRECTORY, ArchiverFrame::OnMenuFileOpenDirectory)
EVT_MENU(ArchiverFrame::ID_MENUEXTRACT, ArchiverFrame::OnMenuFileExtract)
EVT_MENU(ID_MENUEXIT, ArchiverFrame::OnMenuFileExit)
EVT_MENU(ID_MENUABOUT, ArchiverFrame::OnMenuHelpAbout)
EVT_LIST_ITEM_RIGHT_CLICK(ID_LISTCTRL, ArchiverFrame::OnListCtrlRightClick)
EVT_LIST_COL_CLICK(ID_LISTCTRL, ArchiverFrame::OnListCtrlColClick)
wxEND_EVENT_TABLE();

ArchiverFrame::ArchiverFrame() : wxFrame(nullptr, wxID_ANY, RK_APPNAME, wxDefaultPosition, wxSize(500, 800))
{
	SetIcon(wxICON(APPICON)); 
	wxPanel* panel = new wxPanel(this, wxID_ANY);
	wxMenu* openSubmenu = new wxMenu();
	openSubmenu->Append(ID_MENUOPENRCS, "Davilex IMG");
	openSubmenu->Append(ID_MENUOPENGTA3, "GTA III/VC IMG");
	wxMenuItem* openMenuItem = new wxMenuItem((wxMenu*)0, wxID_ANY, "Open",
		wxEmptyString, wxITEM_NORMAL, openSubmenu);
	menus[0] = new wxMenu();
	menus[0]->Append(openMenuItem);
	menus[0]->Append(ArchiverFrame::ID_MENUEXTRACT, "Extract...");
	menus[0]->Append(ID_MENUEXIT, "Exit");
	menus[1] = new wxMenu();
	p_caseInsensitiveCheckbox = menus[1]->AppendCheckItem(ID_MENUSETTINGS_CI, "Case insensitive sort");
	menus[2] = new wxMenu();
	menus[2]->Append(ID_MENUABOUT, "About");
	m_menuBar = new wxMenuBar(3, menus, menuTitles);
	SetMenuBar(m_menuBar);

	m_listCtrl = new wxListCtrl(panel, ID_LISTCTRL, wxPoint(10, 10), wxSize(460, 440), wxLC_REPORT);
	m_listCtrl->InsertColumn(COLUMN_NAME, "Filename", wxLIST_FORMAT_LEFT, 200);
	m_listCtrl->InsertColumn(COLUMN_OFFSET, "Offset (Bytes)", wxLIST_FORMAT_RIGHT, 90);
	m_listCtrl->InsertColumn(COLUMN_FILESIZE, "Size (Bytes)", wxLIST_FORMAT_RIGHT, 80);
	m_listCtrl->InsertColumn(COLUMN_EXTENSION, "Extension", wxLIST_FORMAT_LEFT, 60);

	m_boxSizer = new wxBoxSizer(wxVERTICAL);
	m_boxSizer->Add(m_listCtrl, 1, wxEXPAND | wxALL, 5);
	panel->SetSizerAndFit(m_boxSizer);

	m_statusBar = new wxStatusBar(this, ArchiverFrame::ID_STATUSBAR);
	SetStatusBar(m_statusBar);
}

ArchiverFrame::~ArchiverFrame()
{
	if (archive != nullptr)
	{
		delete archive;
	}
}

void ArchiverFrame::OnMenuFileOpenRcs(wxCommandEvent& _event)
{
	OpenArchive(ArchiveType::RCS);
}

void ArchiverFrame::OnMenuFileOpenGta3(wxCommandEvent& _event)
{
	OpenArchive(ArchiveType::GTA3);
}

void ArchiverFrame::OnMenuFileOpenDirectory(wxCommandEvent& _event)
{
	OpenArchive(ArchiveType::DIRECTORY);
}

void ArchiverFrame::OnMenuFileExtract(wxCommandEvent& _event)
{
	int selected = m_listCtrl->GetSelectedItemCount();
	if (selected == 0)
	{
		wxMessageBox("No file selected!");
		return;
	}
	if (selected == 1)
	{
		int item = m_listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

		// convert the index:
		int index = p_entryList[item].index;

		wxFileDialog saveFileDialog(this, "Save as...", "", archive->getNameW(index), "all files (*.*) | *.*", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
		if (saveFileDialog.ShowModal() == wxID_CANCEL)
		{
			return;
		}
		wxFileOutputStream outputStream(saveFileDialog.GetPath());
		if (!outputStream.IsOk())
		{
			wxString msg("Cannot save file \"");
			msg += saveFileDialog.GetFilename();
			msg += "\"";
			wxMessageBox(msg, "Error", wxOK | wxICON_ERROR);
			return;
		}

		ren::FileInputHandle* handle = archive->getHandle(index);
		std::vector<char> data(handle->filesize());
		handle->read(0, data.data(), data.size());
		outputStream.Write(data.data(), data.size());
		delete handle;
		return;
	}
	else
	{
		wxDirDialog saveToDialog(this, "Save to directory...", "", wxDD_DEFAULT_STYLE);
		if (saveToDialog.ShowModal() == wxID_CANCEL)
		{
			return;
		}
		std::vector<int> remainder; // conflicts are saved here
		// int index = m_listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
		for (int item = m_listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
			item != -1; 
			item = m_listCtrl->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED))
		{
			int index = p_entryList[item].index;
			wxString filepath(saveToDialog.GetPath());
			filepath += L'\\';
			filepath += archive->getNameW(index);
			if (wxFileExists(filepath))
			{
				remainder.push_back(index);
				continue;
			}
			wxFileOutputStream outputStream(filepath);
			if (!outputStream.IsOk())
			{
				wxMessageBox("File could not be created", "Error", wxOK | wxICON_ERROR);
				remainder.push_back(index);
				continue;
			}
			ren::FileInputHandle* handle = archive->getHandle(index);
			std::vector<char> data(handle->filesize());
			handle->read(0, data.data(), data.size());
			outputStream.Write(data.data(), data.size());
			delete handle;
		}
		if (remainder.size() > 0)
		{
			wxString msg("The following files already existed and were therefore not written:\n");
			for (int i = 0; i < remainder.size(); i++)
			{
				msg += L'<';
				msg << remainder[i];
				msg += L"> ";
				msg += archive->getNameW(remainder[i]);
				msg += L" at offset ";
				msg << archive->getOffset(remainder[i]);
				msg += '\n';
			}
			wxMessageBox(msg, "Files not written", wxOK | wxICON_EXCLAMATION);
		}
		return;
	}
}

void ArchiverFrame::OnMenuFileExit(wxCommandEvent& _event)
{
	Destroy();
}

void ArchiverFrame::OnMenuHelpAbout(wxCommandEvent& _event)
{
	AboutDialog aboutDlg;
	aboutDlg.ShowModal();

	/*
	wxAboutDialogInfo info;
	info.SetName(RK_FULLAPPNAME);
	info.SetVersion(RK_APPVERSION);
	info.SetDescription(RK_APPDESCRIPTION);
	info.SetCopyright(RK_APPCOPYRIGHTNOTICE);
	wxAboutBox(info, this);*/
}

void ArchiverFrame::OnListCtrlRightClick(wxListEvent& _event)
{
	wxMenu menu;
	menu.Append(ID_MENUEXTRACT, "Extract...");
	PopupMenu(&menu);
}

void ArchiverFrame::OnListCtrlColClick(wxListEvent& _event)
{
	switch (_event.GetColumn()) {
		case COLUMN_NAME:
		{
			if (p_currentSorting == SORT_NAME || p_currentSorting == SORT_NAME_CI) {
				if (p_caseInsensitiveCheckbox->IsChecked()) {
					sortList(SORT_NAME_CI_REVERSE);
				}
				else {
					sortList(SORT_NAME_REVERSE);
				}
			}
			else {
				if (p_caseInsensitiveCheckbox->IsChecked()) {
					sortList(SORT_NAME_CI);
				}
				else {
					sortList(SORT_NAME);
				}
			}
			break;
		}
		case COLUMN_OFFSET:
		{
			if (p_currentSorting == SORT_OFFSET) {
				sortList(SORT_OFFSET_REVERSE);
			}
			else {
				sortList(SORT_OFFSET);
			}
			break;
		}
		case COLUMN_FILESIZE:
		{
			if (p_currentSorting == SORT_SIZE) {
				sortList(SORT_SIZE_REVERSE);
			}
			else {
				sortList(SORT_SIZE);
			}
			break;
		}
		case COLUMN_EXTENSION:
		{
			if (p_currentSorting == SORT_EXTENSION) {
				sortList(SORT_EXTENSION_REVERSE);
			}
			else {
				sortList(SORT_EXTENSION);
			}
			break;
		}
		default: return;
	}
	m_listCtrl->DeleteAllItems();
	fillListCtrl();
}

void ArchiverFrame::OpenArchive(ArchiveType _type)
{
	ren::ArchiveLinear* newArchive = nullptr;
	wxFileDialog* openFileDialog = nullptr;
	try {
		switch (_type) {
		case ArchiveType::RCS:
		{
			openFileDialog = new wxFileDialog(this, RCS_OPEN_MSG, "", "", RCS_OPEN_FILTER, wxFD_OPEN | wxFD_FILE_MUST_EXIST);
			if (openFileDialog->ShowModal() == wxID_CANCEL)
			{
				delete openFileDialog;
				return;
			}
			wxString filepathImg = openFileDialog->GetPath();
			int dot = filepathImg.find_last_of(L'.');
			wxString filepathInd(filepathImg, 0, dot + 1);
			filepathInd += L"ind";
			ren::CFileInputHandle* imgFile = new ren::CFileInputHandle(filepathImg.wchar_str());
			if (!imgFile->isGood()) {
				delete imgFile;
				delete openFileDialog;
				ren::throwException<std::runtime_error>("IMG file could not be opened!");
			}
			ren::CFileInputHandle* indFile = new ren::CFileInputHandle(filepathInd.wchar_str());
			if (!indFile->isGood()) {
				delete imgFile;
				delete indFile;
				delete openFileDialog;
				ren::throwException<std::runtime_error>("IND file could not be opened!");
			}
			try {
				newArchive = new ren::ArchiveRCS(imgFile, indFile, true);
			}
			catch (const std::exception& l_e) {
				delete imgFile;
				delete indFile;
				delete openFileDialog;
				throw l_e;
			}
			break;
		}
		case ArchiveType::GTA3:
		{
			openFileDialog = new wxFileDialog(this, GTA3_OPEN_MSG, "", "", GTA3_OPEN_FILTER, wxFD_OPEN | wxFD_FILE_MUST_EXIST);
			if (openFileDialog->ShowModal() == wxID_CANCEL)
			{
				delete openFileDialog;
				return;
			}
			wxString filepathImg = openFileDialog->GetPath();
			int dot = filepathImg.find_last_of(L'.');
			wxString filepathDir(filepathImg, 0, dot + 1);
			filepathDir += L"dir";
			ren::CFileInputHandle* imgFile = new ren::CFileInputHandle(filepathImg.wchar_str());
			if (!imgFile->isGood()) {
				delete imgFile;
				delete openFileDialog;
				ren::throwException<std::runtime_error>("IMG file could not be opened!");
			}
			ren::CFileInputHandle* dirFile = new ren::CFileInputHandle(filepathDir.wchar_str());
			if (!dirFile->isGood()) {
				delete dirFile;
				dirFile = nullptr;
			}
			try {
				newArchive = new ren::ArchiveGTA3(imgFile, dirFile, true);
			}
			catch (const std::exception& l_e) {
				delete imgFile;
				delete dirFile;
				delete openFileDialog;
				throw l_e;
			}
			break;
		}
		default:
		{
			throw std::invalid_argument("Invalid archive type.");
		}
		}
		if (!newArchive->isGood())
		{
			delete newArchive;
			newArchive = nullptr;
			throw std::runtime_error("Defective archive.");
		}
	} 
	catch (const std::exception& e) {
		newArchive = nullptr;
		wxMessageBox(e.what(), "Error", wxOK | wxICON_ERROR);
		return;
	}
	if (archive != nullptr)
	{
		delete archive;
		m_listCtrl->DeleteAllItems();
	}
	archive = newArchive;
	delete openFileDialog;
	getEntries();
	fillListCtrl();
	return;
}

void ArchiverFrame::getEntries()
{
	int filecount = archive->getFilecount();
	p_entryList.clear();
	p_entryList.resize(filecount);
	for (int i = 0; i < filecount; i++) {
		std::wstring name = archive->getNameW(i);
		std::wstring extension;
		size_t dotPosition = name.find_last_of('.');
		if (dotPosition != std::wstring::npos) {
			extension = ren::st::toUpper(std::wstring(name, dotPosition + 1));
		}
		p_entryList[i] = { i, name, ren::st::toUpper(name), archive->getOffset(i), archive->getSize(i), extension };
	}
	p_currentSorting = SORT_NONE;
}

void ArchiverFrame::sortList(SortModality _sortModality)
{
	switch (_sortModality)
	{
		case SORT_INDEX:
		{
			std::sort(p_entryList.begin(), p_entryList.end(), [](const ListEntry& _lhs, const ListEntry& _rhs) {return _lhs.index < _rhs.index; });
			p_currentSorting = SORT_INDEX;
			break;
		}
		case SORT_INDEX_REVERSE:
		{
			std::sort(p_entryList.begin(), p_entryList.end(), [](const ListEntry& _lhs, const ListEntry& _rhs) {return _lhs.index > _rhs.index; });
			p_currentSorting = SORT_INDEX_REVERSE;
			break;
		}
		case SORT_NAME:
		{
			std::sort(p_entryList.begin(), p_entryList.end(), [](const ListEntry& _lhs, const ListEntry& _rhs) {return _lhs.name < _rhs.name; });
			p_currentSorting = SORT_NAME;
			break;
		}
		case SORT_NAME_REVERSE:
		{
			std::sort(p_entryList.begin(), p_entryList.end(), [](const ListEntry& _lhs, const ListEntry& _rhs) {return _lhs.name > _rhs.name; });
			p_currentSorting = SORT_NAME_REVERSE;
			break;
		}
		case SORT_NAME_CI:
		{
			std::sort(p_entryList.begin(), p_entryList.end(), [](const ListEntry& _lhs, const ListEntry& _rhs) {return _lhs.nameUpper < _rhs.nameUpper; });
			p_currentSorting = SORT_NAME_CI;
			break;
		}
		case SORT_NAME_CI_REVERSE:
		{
			std::sort(p_entryList.begin(), p_entryList.end(), [](const ListEntry& _lhs, const ListEntry& _rhs) {return _lhs.nameUpper > _rhs.nameUpper; });
			p_currentSorting = SORT_NAME_CI_REVERSE;
			break;
		}
		case SORT_OFFSET:
		{
			std::sort(p_entryList.begin(), p_entryList.end(), [](const ListEntry& _lhs, const ListEntry& _rhs) {return _lhs.offset < _rhs.offset; });
			p_currentSorting = SORT_OFFSET;
			break;
		}
		case SORT_OFFSET_REVERSE:
		{
			std::sort(p_entryList.begin(), p_entryList.end(), [](const ListEntry& _lhs, const ListEntry& _rhs) {return _lhs.offset > _rhs.offset; });
			p_currentSorting = SORT_OFFSET_REVERSE;
			break;
		}
		case SORT_SIZE:
		{
			std::sort(p_entryList.begin(), p_entryList.end(), [](const ListEntry& _lhs, const ListEntry& _rhs) {return _lhs.filesize < _rhs.filesize; });
			p_currentSorting = SORT_SIZE;
			break;
		}
		case SORT_SIZE_REVERSE:
		{
			std::sort(p_entryList.begin(), p_entryList.end(), [](const ListEntry& _lhs, const ListEntry& _rhs) {return _lhs.filesize > _rhs.filesize; });
			p_currentSorting = SORT_SIZE_REVERSE;
			break;
		}
		case SORT_EXTENSION:
		{
			std::stable_sort(p_entryList.begin(), p_entryList.end(), [](const ListEntry& _lhs, const ListEntry& _rhs) {return _lhs.extension < _rhs.extension; });
			p_currentSorting = SORT_EXTENSION;
			break;
		}
		case SORT_EXTENSION_REVERSE:
		{
			std::stable_sort(p_entryList.begin(), p_entryList.end(), [](const ListEntry& _lhs, const ListEntry& _rhs) {return _lhs.extension > _rhs.extension; });
			p_currentSorting = SORT_EXTENSION_REVERSE;
			break;
		}
	}
}

void ArchiverFrame::fillListCtrl()
{
	for (size_t i = 0; i < p_entryList.size(); i++) {
		m_listCtrl->InsertItem(i, p_entryList[i].name);
		m_listCtrl->SetItem(i, COLUMN_OFFSET, std::to_wstring(p_entryList[i].offset));
		m_listCtrl->SetItem(i, COLUMN_FILESIZE, std::to_wstring(p_entryList[i].filesize));
		m_listCtrl->SetItem(i, COLUMN_EXTENSION, p_entryList[i].extension);
	}
}

