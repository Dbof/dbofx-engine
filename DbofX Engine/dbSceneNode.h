#ifndef SCENENODE_H
#define SCENENODE_H

namespace dbx
{
namespace scene
{
	class DBX_API dbSceneNode
	{
	public:
		~dbSceneNode();

		// connection methods
		dbSceneNode* AttachChild(dbSceneNode* snode);		// return snode
		dbSceneNode* DetachChild(dbSceneNode* snode);		// return snode
		dbSceneNode* DetachChild(std::string identifier);		// return matching node

		// movement methods
		void Move(const core::dbVector3& direction, const float& units);

		void MoveUp(const float& units);
		void MoveRight(const float& units);
		void MoveForward(const float& units);

		void RotateX(const float& angle, bool relative = false);
		void RotateY(const float& angle, bool relative = false);
		void RotateZ(const float& angle, bool relative = false);

		void RotateAroundX(const core::dbVector3& position, float angle);
		void RotateAroundY(const core::dbVector3& position, float angle);
		void RotateAroundZ(const core::dbVector3& position, float angle);
		void RotateAroundXYZ(const core::dbVector3& position, const core::dbVector3& axis, float angle);

		void LookAt(const core::dbVector3& target);

		// setter methods
		void SetId(std::string newId);
		void SetPos(const core::dbVector3& newPos);
		void SetDirection(const core::dbVector3& newDirection);
		void SetUp(const core::dbVector3& newUp);
		void SetRight(const core::dbVector3& newRight);
	
		// getter methods
		std::string GetId()				{ return identifier_; }
		core::dbVector3 GetPos()		{ return node_pos_; }
		core::dbVector3 GetDirection()	{ return node_direction_; }
		core::dbVector3 GetUp()			{ return node_up_; }
		core::dbVector3 GetRight()		{ return node_right_; }

	protected:
		//dbSceneNode();
		dbSceneNode(std::string id);

		// variables for connection
		std::string					identifier_;		// unique identifier
		std::vector< dbSceneNode* >	child_nodes_vec_;		// and * children nodes

		// variables for movement
		core::dbMatrix				mat_node_;
		core::dbMatrix				mat_rotationx_;
		core::dbMatrix				mat_rotationy_;
		core::dbMatrix				mat_rotationz_;

		core::dbVector3				node_pos_;			// position in world space
		core::dbVector3				node_right_;			// x-axis
		core::dbVector3				node_up_;			// y-axis
		core::dbVector3				node_direction_;		// z-axis
	
		float						node_movespeed_;
		float						node_rotationspeed_;	
		float						node_rotationx_;		// angle of rotation
		float						node_rotationy_;		// angle of rotation
		float						node_rotationz_;		// angle of rotation

		static float				elapsed_time_;			// elapsed factor for constant frame count

	private:
		friend class dbScene;		// used for easy access to constructors

		void Init();
	};

}}	// namespace
#endif