#pragma once

#include <wx/dialog.h>
#include <wx/image.h>
#include "ImageView.h"
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/button.h>

class AboutDialog : public wxDialog
{
public:
	AboutDialog();
	virtual ~AboutDialog();

private:

	void OnButtonClose(wxCommandEvent& event);

	ImageView* p_imageView;
	wxStaticText* p_text;
	wxButton* p_closeButton;
	wxBoxSizer* p_topSizer;
	wxBoxSizer* p_mainSizer;

	wxDECLARE_EVENT_TABLE();

	enum IDs
	{
		ID_CLOSE = 101
	};
};