#include "LaveApp.h"
#include "ArchiverFrame.h"
#include "xrp.h"
#include <wx/msgdlg.h>
#include <wx/string.h>

wxIMPLEMENT_APP(LaveApp);

LaveApp::LaveApp()
{

}

LaveApp::~LaveApp()
{

}

bool LaveApp::OnInit()
{
	if (!xrp::init()) {
		wxString msg = "Initialization of the XRP Resource Manager failed. Reason: ";
		msg += xrp::getLastErrorMessage();
		wxMessageBox(msg);
	}
	mainFrame = new ArchiverFrame();
	mainFrame->Show();
	return true;
}

int LaveApp::OnExit()
{
	xrp::shutdown();
	return 0;
}
