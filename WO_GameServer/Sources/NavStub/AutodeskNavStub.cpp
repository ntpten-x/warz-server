//=========================================================================
// Autodesk Nav stubs for local GameServer (straight-line movement, no gwnav).
//=========================================================================
#include "r3dPCH.h"
#include "r3d.h"

#ifndef WARZ_LOCAL_NAV_STUB
#error AutodeskNavStub.cpp requires WARZ_LOCAL_NAV_STUB
#endif

#include "AutodeskNavStub.h"
#include "../../GameEngine/ai/AI_Brain.h"

AutodeskNavMesh gAutodeskNavMesh;

AutodeskNavAgent::AutodeskNavAgent()
	: m_status(Idle)
	, m_prevAvoidanceResult(AutodeskNavAgentEnums::NoAvoidance)
	, m_avoidanceResult(AutodeskNavAgentEnums::NoAvoidance)
	, m_arrivalPrecisionRadius(0.5f)
	, m_pathStartTime(0)
	, m_targetSpeed(2.0f)
	, m_pathFinderQuery(0)
	, m_brain(0)
{
	m_navBot = new Kaim::Bot();
	m_pathFinderQuery = new Kaim::AStarQuery<Kaim::AStarCustomizer_Default>();
	m_navBot->m_pathFinderQuery = m_pathFinderQuery;
}

AutodeskNavAgent::~AutodeskNavAgent()
{
	delete m_brain;
	m_brain = 0;
	delete m_pathFinderQuery;
	m_pathFinderQuery = 0;
	delete m_navBot.Get();
	m_navBot = 0;
}

bool AutodeskNavAgent::Init(Kaim::World*, const r3dVector &pos, EPoiType)
{
	m_position = R3D_KY(pos);
	m_status = Idle;
	m_brain = new AI_Brain();
	return true;
}

AutodeskNavAgentEnums::EAvoidanceResult AutodeskNavAgent::GetAvoidanceResult()
{
	return m_avoidanceResult;
}

bool AutodeskNavAgent::IsAvoidanceResultChanged()
{
	bool changed = (m_prevAvoidanceResult != m_avoidanceResult);
	m_prevAvoidanceResult = m_avoidanceResult;
	return changed;
}

void AutodeskNavAgent::Move(KyFloat32 dt)
{
	if(m_status != Moving)
		return;

	Kaim::Vec3f delta(m_targetPos.x - m_position.x, m_targetPos.y - m_position.y, m_targetPos.z - m_position.z);
	float distSq = delta.GetSquareLength();
	float arrive = m_arrivalPrecisionRadius * m_arrivalPrecisionRadius;
	if(distSq <= arrive)
	{
		m_position = m_targetPos;
		m_velocity = Kaim::Vec3f();
		m_status = Arrived;
		return;
	}

	float dist = sqrtf(distSq);
	float step = m_targetSpeed * dt;
	if(step >= dist)
	{
		m_position = m_targetPos;
		m_velocity = Kaim::Vec3f();
		m_status = Arrived;
		return;
	}

	float inv = 1.0f / dist;
	m_velocity = Kaim::Vec3f(delta.x * inv * m_targetSpeed, delta.y * inv * m_targetSpeed, delta.z * inv * m_targetSpeed);
	m_position.x += m_velocity.x * dt;
	m_position.y += m_velocity.y * dt;
	m_position.z += m_velocity.z * dt;
}

KyResult AutodeskNavAgent::ComputeNewPathToDestination(const Kaim::Vec3f& destPos, const Kaim::NavGraphEdgePtr&, Kaim::NavGraphEdgeDirection)
{
	m_targetPos = destPos;
	m_status = Moving;
	return KY_SUCCESS;
}

void AutodeskNavAgent::Update(float timeStep)
{
	if(m_status == ComputingPath)
		m_status = Moving;
	Move(timeStep);
}

bool AutodeskNavAgent::StartMove(const r3dPoint3D &target, float)
{
	m_targetPos = R3D_KY(target);
	m_status = Moving;
	m_pathStartTime = r3dGetTime();
	if(m_brain)
	{
		m_brain->Cancel();
		m_brain->m_TacticParams.m_StartPos = GetPosition();
		m_brain->m_TacticParams.m_DestPos = target;
	}
	return true;
}

void AutodeskNavAgent::StopMove()
{
	m_status = Idle;
	m_velocity = Kaim::Vec3f();
}

void AutodeskNavAgent::SetTargetSpeed(float speed)
{
	m_targetSpeed = speed;
}

r3dVector AutodeskNavAgent::GetPosition() const
{
	return KY_R3D(m_position);
}

AutodeskNavMesh::AutodeskNavMesh()
	: m_visualDebugPort(0)
	, m_NavProfileIdSuperZombie(0)
	, doDebugDraw(false)
	, initialized(false)
{
	world = &m_world;
}

AutodeskNavMesh::~AutodeskNavMesh()
{
	Close();
}

bool AutodeskNavMesh::Init(USHORT visualDebugPort)
{
	m_visualDebugPort = visualDebugPort;
	initialized = true;
	r3dOutToLog("AutodeskNav STUB Init (port %u) — no gwnav libs, straight-line AI\n", (unsigned)visualDebugPort);
	return true;
}

bool AutodeskNavMesh::LoadPathData()
{
	return true;
}

void AutodeskNavMesh::RemovePathData()
{
}

void AutodeskNavMesh::Close()
{
	for(int i = 0; i < (int)agents.Count(); ++i)
		delete agents[i];
	agents.Clear();
	for(int i = 0; i < (int)obstacles.Count(); ++i)
		delete obstacles[i].Get();
	obstacles.Clear();
	initialized = false;
}

void AutodeskNavMesh::Update()
{
	for(int i = 0; i < (int)agents.Count(); ++i)
	{
		if(agents[i])
			agents[i]->Update(r3dGetFrameTime());
	}
}

int AutodeskNavMesh::AddObstacle(GameObject*, const r3dBoundBox&, float)
{
	Kaim::BoxObstacle* box = new Kaim::BoxObstacle();
	int idx = (int)obstacles.Count();
	obstacles.PushBack(Kaim::Ptr<Kaim::WorldElement>(box));
	return idx;
}

int AutodeskNavMesh::AddObstacle(GameObject*, const r3dCylinder&)
{
	Kaim::CylinderObstacle* cyl = new Kaim::CylinderObstacle();
	int idx = (int)obstacles.Count();
	obstacles.PushBack(Kaim::Ptr<Kaim::WorldElement>(cyl));
	return idx;
}

bool AutodeskNavMesh::RemoveObstacle(int idx)
{
	if(idx < 0 || idx >= (int)obstacles.Count())
		return false;
	delete obstacles[idx].Get();
	obstacles[idx] = 0;
	return true;
}

AutodeskNavAgent * AutodeskNavMesh::CreateNavAgent(const r3dVector &pos)
{
	AutodeskNavAgent* a = new AutodeskNavAgent();
	a->Init(GetWorld(), pos);
	AddNavAgent(a);
	return a;
}

void AutodeskNavMesh::AddNavAgent(AutodeskNavAgent *a)
{
	agents.PushBack(a);
}

void AutodeskNavMesh::DeleteNavAgent(AutodeskNavAgent *a)
{
	for(int i = 0; i < (int)agents.Count(); ++i)
	{
		if(agents[i] == a)
		{
			agents.Erase(i);
			delete a;
			return;
		}
	}
	delete a;
}

bool AutodeskNavMesh::GetClosestNavMeshPoint(r3dPoint3D &inOut, float, float)
{
	return true;
}

bool AutodeskNavMesh::IsNavPointValid(const r3dPoint3D &)
{
	return true;
}

bool AutodeskNavMesh::AdjustNavPointHeight(r3dPoint3D &inOut, float searchHeightRange)
{
	// Keep XY, try to snap height via physics if available; otherwise accept as-is.
	(void)searchHeightRange;
	return true;
}

AutodeskNavAgent** AutodeskNavMesh::GetNavAgentsInAABB(const r3dPoint3D&, const r3dPoint3D&, std::unordered_set<uint32_t>&, uint32_t& foundNumNavAgents, bool skipListCreation)
{
	foundNumNavAgents = 0;
	if(skipListCreation)
		return 0;
	return 0;
}
