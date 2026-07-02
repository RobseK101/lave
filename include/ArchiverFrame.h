#pragma once
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <vector>
#include <wx/wfstream.h>
#include <ren/ArchiveLinear.h>

class ArchiverFrame : public wxFrame
{
public:
	ArchiverFrame();
	~ArchiverFrame();

	enum class ArchiveType
	{
		RCS, GTA3, XRP, DIRECTORY
	};
	
private:
	void OnMenuFileOpenRcs(wxCommandEvent& _event);
	void OnMenuFileOpenGta3(wxCommandEvent& _event);
	void OnMenuFileOpenXrp(wxCommandEvent& _event);
	void OnMenuFileOpenDirectory(wxCommandEvent& _event);
	void OnMenuFileExtract(wxCommandEvent& _event);
	void OnMenuFileExit(wxCommandEvent& _event);
	void OnMenuHelpAbout(wxCommandEvent& _event);
	void OnListCtrlRightClick(wxListEvent& _event);
	void OnListCtrlColClick(wxListEvent& _event);

	void OpenArchive(ArchiveType _type);

	struct ListEntry
	{
		int index;
		std::wstring name;
		std::wstring nameUpper;
		int offset;
		int filesize;
		std::wstring extension;
	};

	enum SortModality : int
	{
		SORT_NONE = -1, 
		SORT_INDEX = 0,		SORT_INDEX_REVERSE, 
		SORT_NAME,			SORT_NAME_REVERSE, 
		SORT_NAME_CI,		SORT_NAME_CI_REVERSE, 
		SORT_OFFSET,		SORT_OFFSET_REVERSE, 
		SORT_SIZE,			SORT_SIZE_REVERSE,
		SORT_EXTENSION,		SORT_EXTENSION_REVERSE
	};

	enum Column : int
	{
		COLUMN_NAME = 0, COLUMN_OFFSET, COLUMN_FILESIZE, COLUMN_EXTENSION
	};

	void getEntries();
	void sortList(SortModality _sortModality);

	void fillListCtrl();
	
	wxMenuBar* m_menuBar;
	wxMenu* menus[3];
	wxString menuTitles[3] = { "File", "Settings", "Help" };
	wxListCtrl* m_listCtrl;
	wxBoxSizer* m_boxSizer;
	wxStatusBar* m_statusBar;
	wxMenuItem* p_caseInsensitiveCheckbox;
	ren::ArchiveLinear* archive = nullptr;

	wxDECLARE_EVENT_TABLE();

	enum
	{
		ID_MENUOPENRCS = 201,
		ID_MENUOPENGTA3,
		ID_MENUOPENXRP,
		ID_MENUOPENDIRECTORY,
		ID_MENUEXTRACT,
		ID_MENUEXIT,
		ID_MENUABOUT,
		ID_LISTCTRL,
		ID_STATUSBAR,
		ID_MENUSETTINGS_CI
	};

	std::vector<ListEntry> p_entryList;
	SortModality p_currentSorting = SORT_NONE;
	
};

