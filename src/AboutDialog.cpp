#include "AboutDialog.h"

#include <wx/imagpng.h>
#include <wx/mstream.h>
#include <wx/msgdlg.h>
#include <wx/dcclient.h>
#include <wx/graphics.h>
// #include "c_res.h"
#include "xrp.h"
#include "FileInputHandleWxStream.h"

#define RK_APPNAME "Linear Archive Viewer and Extractor"
#define RK_APPSHORTNAME "L.A.V.E."
#define RK_FULLAPPNAME RK_APPNAME " (" RK_APPSHORTNAME ")"
#define RK_APPVERSION "2.2 (2026-07-06)"
#define RK_APPAUTHOR "Dr. Robert Klenk"
#define RK_APPCOPYRIGHT "(c) 2026"
#define RK_APPCOPYRIGHTNOTICE RK_APPCOPYRIGHT " " RK_APPAUTHOR
#define RK_APPDESCRIPTION "A viewer end extractor for linear (i.e. non-branching) data archives."

#define PNG_FILENAME "LAVE-Logo.png"

wxBEGIN_EVENT_TABLE(AboutDialog, wxDialog)
//EVT_PAINT(AboutDialog::OnPaint)
EVT_BUTTON(AboutDialog::ID_CLOSE, AboutDialog::OnButtonClose)
wxEND_EVENT_TABLE();

AboutDialog::AboutDialog() :
	wxDialog(nullptr, wxID_ANY, L"About LAVE2...", wxDefaultPosition,
		wxDefaultSize, wxCAPTION | wxCLOSE_BOX | wxSTAY_ON_TOP)
{
	p_imageView = new ImageView(this, wxID_ANY, wxDefaultPosition, wxSize(96, 128), ImageView::ORIGINAL);

	wxImage image;
	ren::FileInputHandle* resourceHandle = xrp::getHandle(PNG_FILENAME);
	if (resourceHandle) {
		FileInputHandleWxStream fileHandle(resourceHandle, true);
		wxPNGHandler imageHandler;
		imageHandler.LoadFile(&image, fileHandle);
		p_imageView->AssignImage(image);
	}
	else {
		wxString msg = "Could not retrieve file handle for \"";
		msg << PNG_FILENAME;
		msg << "\". Reason: ";
		msg << xrp::getLastErrorMessage();
		wxMessageBox(msg);
	}

	wxString text = RK_FULLAPPNAME;
	text += "\n\n" RK_APPDESCRIPTION;
	text += "\nVersion ";
	text += RK_APPVERSION;
	text += "\n" RK_APPCOPYRIGHTNOTICE;
	
	p_text = new wxStaticText(this, wxID_ANY, text, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL);
	p_topSizer = new wxBoxSizer(wxHORIZONTAL);
	auto p_imageViewSizerItem = p_topSizer->Add(p_imageView);
	p_imageViewSizerItem->SetFlag(wxLEFT | wxTOP);
	p_imageViewSizerItem->SetBorder(5);
	auto p_textSizerItem = p_topSizer->Add(p_text);
	p_textSizerItem->SetFlag(wxALIGN_CENTER | wxRIGHT | wxLEFT | wxTOP);
	p_textSizerItem->SetBorder(5);

	p_closeButton = new wxButton(this, ID_CLOSE, "Close");

	p_mainSizer = new wxBoxSizer(wxVERTICAL);
	p_mainSizer->Add(p_topSizer);
	auto p_closeButtonItem = p_mainSizer->Add(p_closeButton);
	p_closeButtonItem->SetFlag(wxALIGN_CENTER | wxLEFT | wxTOP | wxRIGHT | wxBOTTOM);
	p_closeButtonItem->SetBorder(5);

	SetSizerAndFit(p_mainSizer);
}

AboutDialog::~AboutDialog()
{
	
}

void AboutDialog::OnButtonClose(wxCommandEvent& event)
{
	EndModal(wxID_CLOSE);
}
