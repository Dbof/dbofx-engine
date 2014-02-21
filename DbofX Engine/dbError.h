#ifndef ERROR_H
#define ERROR_H

namespace dbx
{
namespace core
{
	enum MsgType{ MT_INFO, MT_WARNING, MT_ERROR };

	// Singleton class
	class DBX_API dbError 
	{
	public:
		static dbError* GetInstance();

	   static void AddError(std::string msg, MsgType mt);
	   ~dbError() {};

	private:
	   dbError() {};
	   static dbError* instance_;

	   DISALLOW_COPY_AND_ASSIGN(dbError);
	};

}}	// namespace

#endif