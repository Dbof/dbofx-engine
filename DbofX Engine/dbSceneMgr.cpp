#include "DbofX.h"

using std::string;

namespace dbx
{
namespace scene
{
	using namespace core;

dbSceneMgr* dbSceneMgr::instance_ = NULL;

dbSceneMgr::dbSceneMgr(void)
{
}

dbSceneMgr::~dbSceneMgr(void)
{
}


dbSceneMgr* dbSceneMgr::GetInstance()
{
	if (!instance_)
		instance_ = new dbSceneMgr();
	return instance_;
}

dbScene* dbSceneMgr::AddScene(dbScene* newscene)
{
	if (!newscene)
		return NULL;

	string id = newscene->GetId();

	if (scenes_map_.count(id) > 0)
		return NULL;

	scenes_map_[id] = newscene;
	return newscene;
}



void dbSceneMgr::Exit()
{
	for (std::unordered_map<string, dbScene*>::iterator it = scenes_map_.begin(); it != scenes_map_.end(); it++)
	{
		it->second->Release();
	}
}


void dbSceneMgr::OnLostDevice()
{
	for (std::unordered_map<string, dbScene*>::iterator it = scenes_map_.begin(); it != scenes_map_.end(); it++)
	{
		it->second->OnLostDevice();
	}
}
void dbSceneMgr::OnResetDevice()
{
	for (std::unordered_map<string, dbScene*>::iterator it = scenes_map_.begin(); it != scenes_map_.end(); it++)
	{
		it->second->OnResetDevice();
	}
}


}}	// namespace