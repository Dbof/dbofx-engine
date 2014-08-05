#ifndef D2D_MANAGER_H
#define D2D_MANAGER_H

namespace dbx
{
namespace video
{
	class DBX_API db2DManager
	{
	public:
		static db2DManager* GetInstance();
		void Exit();

		// these functions only work on one draw**** call
		void SetRotation(const float& radian);
		void SetScaling(core::dbVector3& scaling);

		void Begin2D();
		void Begin3D();
		void End();

		void DrawImage2D(res::TEXTURE_ITEM* item, RECT* sourceRect, core::dbVector3 destination, core::dbColor color, bool useAlpha);
		void DrawImage2D(res::TEXTURE_ITEM* item, RECT* sourceRect, core::dbVector3 destination);
		void DrawImage3D(res::TEXTURE_ITEM* item, RECT* sourceRect, core::dbVector3 destination, core::dbColor color, bool useAlpha);
		void DrawImage3D(res::TEXTURE_ITEM* item, RECT* sourceRect, core::dbVector3 destination);
		void DrawBillboard(res::TEXTURE_ITEM* item, RECT* sourceRect, core::dbVector3 destination, core::dbColor color, bool useAlpha);
		void DrawBillboard(res::TEXTURE_ITEM* item, RECT* sourceRect, core::dbVector3 destination);

		void OnLostDevice();
		void OnResetDevice();
	private:
		db2DManager(void);
		~db2DManager(void) { Exit(); }

		void ResetSettings();
		bool Draw(DWORD Flags, const res::TEXTURE_ITEM* item, RECT* &sourceRect, core::dbVector3* center, core::dbVector3* dest, const core::dbColor &color);

		static db2DManager* instance_;
		ID3DXSprite* d3dx_sprite_;

		bool batched_draw_2D_;
		bool batched_draw_3D_;

		// settings used only once per draw
		float sprite_rotation_;
		core::dbVector3 sprite_scaling_;
	};

}};	// namespace
#endif