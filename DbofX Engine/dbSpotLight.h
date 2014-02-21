#ifndef SPOT_LIGHT_H
#define SPOT_LIGHT_H

namespace dbx
{
namespace scene
{
	class DBX_API dbSpotLight : public dbLight
	{		
	public:
		void SetInnerAngle(const float &inAngle);
		void SetOuterAngle(const float &outAngle);

		// Attenuation formula = ( 1 / (att0 + att1 * d + att2 * d^2) )
		void SetAttenuation(const float& att0, const float& att1, const float& att2);
		void SetRange(const float &newRange);

	protected:
		bool InitLight();

	private:
		friend class dbLightManager;
		
		dbSpotLight(const std::string identifier);
		~dbSpotLight(void);
	};

}};	// namespace
#endif