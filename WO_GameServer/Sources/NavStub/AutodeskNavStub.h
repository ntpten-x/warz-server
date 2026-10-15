//=========================================================================
// Autodesk Nav API stubs (no gwnav libs) for local GameServer builds.
//=========================================================================
#pragma once

#include "KaimMinimal.h"
#include <unordered_set>

namespace AutodeskNavAgentEnums
{
	enum EAvoidanceResult
	{
		NoAvoidance			= 0,
		SlowDown			= 1,
		Accelerate			= 2,
		Turn				= 3,
		Stop				= 4,
		SolutionNotFound	= 5,
	};
}

class AutodeskPerfCounter2 : public Kaim::IPerfMarkerInterface
{
public:
	void Reset() {}
	void Dump() {}
	AutodeskPerfCounter2() {}
	virtual ~AutodeskPerfCounter2() {}
	virtual void Begin(const char*) {}
	virtual void End() {}
};

class AI_Brain;
class GameObject;
class AutodeskNavAgent;

R3D_FORCEINLINE r3dPoint3D KY_R3D(const Kaim::Vec3f& vec)
{
	return r3dPoint3D(vec.x, vec.z, vec.y);
}

R3D_FORCEINLINE Kaim::Vec3f R3D_KY(const r3dPoint3D& v)
{
	return Kaim::Vec3f(v.x, v.z, v.y);
}

class AutodeskNavAgent
{
public:
	enum EPoiType
	{
		PoiUndefined = Kaim::PointOfInterestType_Undefined,
		PoiZombie = Kaim::PointOfInterestType_FirstClient,
		PoiSuperZombie,
	};
	enum EStatus
	{
		Idle = 0,
		ComputingPath,
		Moving,
		Arrived,
		PathNotFound,
		Failed,
	};
	EStatus		m_status;
	AutodeskNavAgentEnums::EAvoidanceResult m_prevAvoidanceResult;
	AutodeskNavAgentEnums::EAvoidanceResult m_avoidanceResult;

	Kaim::Vec3f	m_velocity;
	Kaim::Vec3f	m_position;
	Kaim::Vec3f	m_targetPos;
	float		m_arrivalPrecisionRadius;
	float		m_pathStartTime;
	float		m_targetSpeed;

	Kaim::Ptr<Kaim::Bot> m_navBot;
	Kaim::Ptr<Kaim::PointOfInterest> m_navPointOfInterest;
	Kaim::AStarQuery<Kaim::AStarCustomizer_Default>* m_pathFinderQuery;

	AI_Brain*	m_brain;

	AutodeskNavAgentEnums::EAvoidanceResult GetAvoidanceResult();
	bool IsAvoidanceResultChanged();

	void Move(KyFloat32 simulationTimeInSeconds);
	void MoveOnNavGraph(KyFloat32, const Kaim::Vec3f&, Kaim::BotUpdateConfig&) {}
	bool MoveOnNavMesh(KyFloat32, const Kaim::Vec3f&, Kaim::BotUpdateConfig&) { return true; }

	void HandleArrivalAndUpperBound() {}
	void HandleArrival() {}
	void HandleUpperBound() {}

protected:
	KyResult ComputeNewPathToDestination(const Kaim::Vec3f& destPos);
	KyResult ComputeNewPathToDestination(const Kaim::Vec3f& destPos, const Kaim::NavGraphEdgePtr&, Kaim::NavGraphEdgeDirection = Kaim::NAVGRAPHEDGE_BIDIRECTIONAL);

public:
	AutodeskNavAgent();
	virtual ~AutodeskNavAgent();

	bool Init(Kaim::World* world, const r3dVector &pos, EPoiType poiType = AutodeskNavAgent::PoiUndefined);
	void Update(float timeStep);
	bool StartMove(const r3dPoint3D &target, float maxAstarDist = 999999);
	void StopMove();
	void SetTargetSpeed(float speed);
	r3dVector GetPosition() const;
};

__forceinline KyResult AutodeskNavAgent::ComputeNewPathToDestination(const Kaim::Vec3f& destPos)
{
	return ComputeNewPathToDestination(destPos, Kaim::NavGraphEdgePtr());
}

class AutodeskNavMesh
{
	uint32_t m_visualDebugPort;
	Kaim::World m_world;
	Kaim::Ptr<Kaim::World> world;
	Kaim::Ptr<Kaim::NavData> navData;
	Kaim::CoordSystem m_coordSystem;
	r3dTL::TArray<AutodeskNavAgent*> agents;

public:
	KyUInt32 m_NavProfileIdSuperZombie;
	AutodeskPerfCounter2 perfBridge;
	r3dTL::TArray<Kaim::Ptr<Kaim::WorldElement>> obstacles;
	Kaim::GeneratorParameters buildGlobalConfig;
	bool doDebugDraw;
	bool initialized;

	AutodeskNavMesh();
	~AutodeskNavMesh();
	bool Init(USHORT visualDebugPort=48888);
	bool LoadPathData();
	void RemovePathData();
	void Close();
	void Update();
	void BuildForCurrentLevel() {}
	void DebugDraw() {}
	void DebugDrawObstacles() {}
	void ExportToObj() {}
	void LoadBuildConfig() {}
	void SaveBuildConfig() {}
	void SaveNavGenProj() {}

	int AddObstacle(GameObject* obstacle, const r3dBoundBox &bb, float rotX);
	int AddObstacle(GameObject* obstacle, const r3dCylinder &c);
	bool RemoveObstacle(int idx);

	AutodeskNavAgent * CreateNavAgent(const r3dVector &pos);
	void AddNavAgent(AutodeskNavAgent *a);
	void DeleteNavAgent(AutodeskNavAgent *a);

	Kaim::World* GetWorld() { return world; }
	Kaim::Database * GetDB() { return world->GetDatabase(0); }

	bool GetClosestNavMeshPoint(r3dPoint3D &inOut, float, float);
	bool IsNavPointValid(const r3dPoint3D &);
	bool AdjustNavPointHeight(r3dPoint3D &inOut, float searchHeightRange);

	AutodeskNavAgent** GetNavAgentsInAABB(const r3dPoint3D&, const r3dPoint3D&, std::unordered_set<uint32_t>&, uint32_t& foundNumNavAgents, bool skipListCreation = false);
};

extern AutodeskNavMesh gAutodeskNavMesh;
