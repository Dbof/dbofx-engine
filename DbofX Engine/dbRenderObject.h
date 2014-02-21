#ifndef _RENDER_OBJECT_H
#define _RENDER_OBJECT_H

namespace dbx
{
namespace scene
{
	class DBX_API dbRenderObject
	{
	public:		
		virtual bool isVisible()				= 0;
		virtual void render()					= 0;
		virtual bool setVisible(bool visible)	= 0;
	protected:
		bool visible;
	};

}}	// namespace

#endif