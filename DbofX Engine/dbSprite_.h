#ifndef SPRITE_H
#define SPRITE_H

namespace dbx
{
namespace scene
{
	class dbSprite
	{
		friend class dbScene;

	public:
		void setTexture(res::TEXTURE_ITEM& texture) { curr_texture = texture; }
		void setPosition(dbVector3 position, float rotation = 0.f, dbVector3 scaling = dbVector3(1.f, 1.f, 1.f));
		void setPosition(dbMatrix transMatrix);
		bool render(RECT* sourceRect = NULL, dbColor color = dbColor(1.f));
		
		static bool init();	// for dbScene
	//	dbVector3 getPosition()	{ return ; }

		//static dbMatrix getMatrix() { return transformMatrix; }
	private:
		dbSprite();
		~dbSprite();

		static dbVector3 center;

		static bool isInit;	// ID3DXSPRITE initiated
		static bool isSetup;	// setup() was called
	
		//static dbMatrix	transformMatrix;
		res::TEXTURE_ITEM	curr_texture;
		static LPD3DXSPRITE		sprite;		// you only need one instance to draw x sprites
	};

}}	// namespace
#endif