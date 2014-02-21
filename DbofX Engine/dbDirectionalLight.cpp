#include "DbofX.h"

using std::string;

namespace dbx
{
namespace scene
{
	using namespace core;
dbDirectionalLight::dbDirectionalLight(const string identifier) : dbLight(identifier)
{
	dbSceneNode::identifier_ = identifier;
}

dbDirectionalLight::~dbDirectionalLight(void)
{
}

}}	// namespace