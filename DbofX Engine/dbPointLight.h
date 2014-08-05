#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

namespace dbx
{
namespace scene
{
	class DBX_API dbPointLight : public dbLight
	{
	public:
		// Attenuation formula = ( 1 / (att0 + att1 * d + att2 * d^2) )
		void SetAttenuation(const float& att0, const float& att1, const float& att2);
		void SetRange(const float& newRange);

	private:
		friend class dbLightManager;

		bool InitLight();

		dbPointLight(const std::string identifier);
		~dbPointLight(void);
	};
	

}}	// namespace

#endif