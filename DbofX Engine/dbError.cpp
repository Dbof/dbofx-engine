#include "DbofX.h"

using std::string;

namespace dbx
{
namespace core
{

dbError* dbError::instance_ = NULL;

dbError* dbError::GetInstance()
{
	if (!instance_)
		instance_ = new dbError();
	return instance_;
}

void dbError::AddError(string msg, MsgType mt)
{
	switch (mt)
	{
	case MT_INFO:
		std::cout << "DbofX Engine [Info]: ";
	break;

	case MT_WARNING:
		std::cout << "DbofX Engine [Warning]: ";
	break;

	case MT_ERROR:
		std::cout << "DbofX Engine [Error]: ";
	break;
	}

	std::cout << msg << std::endl;

}

}}	// namespace