#pragma once

#include <wx/wx.h>

class LaveApp : public wxApp
{
public:
	LaveApp();
	~LaveApp();

	virtual bool OnInit() override;
	virtual int OnExit() override;

private:
	wxFrame* mainFrame = nullptr;
};