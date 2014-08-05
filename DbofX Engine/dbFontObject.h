#ifndef TEXTMGR_H
#define TEXTMGR_H

namespace dbx
{
namespace scene
{
	class DBX_API dbFontObject
	{
	public:
		dbFontObject() {}
		~dbFontObject() { Exit(); }

		// opens a Windows dialog to select font
		bool InitFromDialog();
		bool Init(std::string fontname, int height, bool bold, bool italics);
		void Draw(std::string text, core::dbColor color, int top, int left, int bottom, int right);
	
		static void Exit();
		static void Lost();
		static bool Reset();
	private:
		void AddToVec();

		// font variables
		LPD3DXFONT		d3d_font_;
		std::string		font_name_;
		bool			font_style_bold_;
		bool			font_style_italic_;
		int				font_style_height_;

		static std::vector<LPD3DXFONT> d3d_fonts_vec_;
	};

}}	// namespace
#endif