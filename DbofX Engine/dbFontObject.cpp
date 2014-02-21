#include "DbofX.h"

using std::string;

namespace dbx
{
namespace scene
{
	using namespace core;

	std::vector<LPD3DXFONT> dbFontObject::d3d_fonts_vec_;
//**********************************************************************
// FUNCTIONS

void dbFontObject::AddToVec()
{
	d3d_fonts_vec_.push_back(this->d3d_font_);
}


bool dbFontObject::InitFromDialog()
{
	if (!dbDevice::isWindowed())
		return false;

	CHOOSEFONT cf; 
	LOGFONT lf; 

	// Initialize members of the CHOOSEFONT structure.  
	cf.lStructSize = sizeof(CHOOSEFONT); 
	cf.hwndOwner = (HWND)NULL; 
	cf.hDC = (HDC)NULL; 
	cf.lpLogFont = &lf; 
	cf.iPointSize = 0; 
	cf.Flags = CF_SCREENFONTS; 
	cf.rgbColors = RGB(0,0,0); 
	cf.lCustData = 0L; 
	cf.lpfnHook = (LPCFHOOKPROC)NULL; 
	cf.lpTemplateName = (LPSTR)NULL; 
	cf.hInstance = (HINSTANCE) NULL; 
	cf.lpszStyle = (LPSTR)NULL; 
	cf.nFontType = SCREEN_FONTTYPE; 
	cf.nSizeMin = 0; 
	cf.nSizeMax = 0; 

	// Display the CHOOSEFONT common-dialog box.  
	if (!ChooseFont(&cf))
		return false;

	// Create a logical font based on the user's  
	// selection and return a handle identifying  
	// that font.  
	D3DXFONT_DESC font_desc;
	font_desc.CharSet = lf.lfCharSet;
	memcpy(&font_desc.FaceName, &lf.lfFaceName, 32);
	font_desc.Height = lf.lfHeight;
	font_desc.Italic = lf.lfItalic;
	font_desc.MipLevels = D3DX_DEFAULT;
	font_desc.OutputPrecision = lf.lfOutPrecision;
	font_desc.PitchAndFamily = lf.lfPitchAndFamily;
	font_desc.Quality = lf.lfQuality;
	font_desc.Weight = lf.lfWeight;
	font_desc.Width = lf.lfWidth;

	if (FAILED( D3DXCreateFontIndirect(dbDevice::getDevice(), &font_desc, &d3d_font_)))
	{
		DBX_ERROR("Could not init font object!");
		return false;
	}
	AddToVec();
	return true;
}

bool dbFontObject::Init(string fontname, int height, bool bold, bool italics)
{
	font_style_bold_ = bold;
	font_style_italic_ = italics;
	font_name_ = fontname;
	font_style_height_ = height;

	if (FAILED(D3DXCreateFont(dbDevice::getDevice(), height, 0, (bold) ? FW_BOLD : FW_NORMAL, 0, italics, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, 
		ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, fontname.c_str(), &d3d_font_ )))
	{
		DBX_ERROR("Could not init font object!");
		return false;
	}
	AddToVec();
	return true;
}

void dbFontObject::Draw(string text, dbColor color, int top, int left, int bottom, int right)
{
	RECT position;
	position.top = top;
	position.left = left;
	
	if (right == -1)
		position.right = left + text.length() * 10;
	
	if (bottom == -1)
		position.bottom = top + text.length() * 5;

	d3d_font_->DrawText(NULL, text.c_str(), -1, &position, 0, D3DCOLOR(color));
}

//**********************************************************************

void dbFontObject::Lost()
{
	for (int i = 0; i < (signed)d3d_fonts_vec_.size(); i++)
	{
		if (d3d_fonts_vec_[i])
			d3d_fonts_vec_[i]->OnLostDevice();
	}
}

bool dbFontObject::Reset()
{
	for (int i = 0; i < (signed)d3d_fonts_vec_.size(); i++)
	{
		if (d3d_fonts_vec_[i])
			d3d_fonts_vec_[i]->OnResetDevice();
	}
	return true;
}

void dbFontObject::Exit()
{
	for (int i = 0; i < (signed)d3d_fonts_vec_.size(); i++)
	{
		DB_MEM_RELEASE(d3d_fonts_vec_[i]);
	}
	
	DBX_INFO("Font objects were shut down.");
}

//**********************************************************************

}}	// namespace