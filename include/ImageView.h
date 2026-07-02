#pragma once

#include <wx/window.h>
#include <wx/image.h>

class ImageView : public wxWindow
{
public:
	enum Mode
	{
		ORIGINAL, FIT, STRETCH
	};

	ImageView() = delete;
	ImageView(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize, Mode mode = STRETCH);
	~ImageView();

	void AssignBitmap(wxBitmap* bitmap);
	void ClearBitmap();

	void AssignImage(const wxImage& image);

	void SetMode(Mode mode);

private:
	void OnPaint(wxPaintEvent& event);
	void OnResize(wxSizeEvent& event);

	wxDECLARE_EVENT_TABLE();

	wxBitmap* p_bitmap;
	Mode p_mode;
};