#ifndef DIRECTIONAL_LIGHT_H
#define DIRECTIONAL_LIGHT_H

namespace dbx
{
namespace scene
{
	class DBX_API dbDirectionalLight : public dbLight
	{
	public:

	private:
		friend class dbLightManager;

		dbDirectionalLight(const std::string identifier);
		~dbDirectionalLight(void);
	};

}}	// namespace

#endif