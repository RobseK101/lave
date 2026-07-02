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

#include "ImageView.h"
#include <wx/dcclient.h>
#include <wx/graphics.h>
#include <wx/bitmap.h>

wxBEGIN_EVENT_TABLE(ImageView, wxWindow)
EVT_PAINT(ImageView::OnPaint)
EVT_SIZE(ImageView::OnResize)
wxEND_EVENT_TABLE();

ImageView::ImageView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, Mode mode) : 
	wxWindow(parent, id, pos, size), p_bitmap(nullptr), p_mode(mode)
{

}

ImageView::~ImageView()
{
	delete p_bitmap;
}

void ImageView::AssignBitmap(wxBitmap* bitmap)
{
	delete p_bitmap;
	p_bitmap = bitmap;
}

void ImageView::ClearBitmap()
{
	delete p_bitmap;
	p_bitmap = nullptr;
}

void ImageView::AssignImage(const wxImage& image)
{
	delete p_bitmap;
	p_bitmap = new wxBitmap(image);
}

void ImageView::SetMode(Mode mode)
{
	p_mode = mode;
}

void ImageView::OnPaint(wxPaintEvent& event)
{
	if (p_bitmap) {
		wxPaintDC dc(this);
		wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
		if (gc) {
			wxRect clientRect = GetClientRect();
			wxRect bitmapRect = { 0, 0, p_bitmap->GetWidth(), p_bitmap->GetHeight() };
			
			switch (p_mode) {
			case ORIGINAL:
			{
				if (clientRect.width >= bitmapRect.width) {
					bitmapRect.x = (clientRect.width - bitmapRect.width) / 2;
				}
				if (clientRect.height >= bitmapRect.height) {
					bitmapRect.y = (clientRect.height - bitmapRect.height) / 2;
				}
				break;
			}
			case FIT:
			{
				float imageAspect = (float)bitmapRect.height / (float)bitmapRect.width;
				float clientAspect = (float)clientRect.height / (float)clientRect.width;
				if (clientAspect < imageAspect) { // i.e. the client window is wider -> use its height
					bitmapRect.y = 0;
					bitmapRect.height = clientRect.height;
					bitmapRect.width = bitmapRect.height / imageAspect;
					bitmapRect.x = (clientRect.width - bitmapRect.width) / 2;
				}
				else { // -> use width
					bitmapRect.x = 0;
					bitmapRect.width = clientRect.width;
					bitmapRect.height = bitmapRect.width * imageAspect;
					bitmapRect.y = (clientRect.height - bitmapRect.height) / 2;
				}
				break;
			}
			case STRETCH:
			default:
			{
				bitmapRect = clientRect;
				break;
			}
			}

			//wxGraphicsBitmap gBitmap = gc->CreateBitmapFromImage(*p_image);
			// Apparently, wxBitmap can also be used directly!
			gc->DrawBitmap(*p_bitmap, bitmapRect.x, bitmapRect.y, bitmapRect.width, bitmapRect.height);
			delete gc;
		}
	}
}

void ImageView::OnResize(wxSizeEvent& event)
{
	Refresh();
}
