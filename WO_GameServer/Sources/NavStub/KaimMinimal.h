//=========================================================================
// Minimal Kaim stubs for local WarZ server builds without Autodesk Nav libs.
//=========================================================================
#pragma once

#include <cmath>
#include <cstring>

typedef int KyResult;
typedef float KyFloat32;
typedef unsigned int KyUInt32;
typedef int KyInt32;
#ifndef KY_NULL
#define KY_NULL 0
#endif
#ifndef KY_SUCCESS
#define KY_SUCCESS 0
#endif
#ifndef KY_ERROR
#define KY_ERROR 1
#endif

namespace Kaim
{
	template<typename T>
	class Ptr
	{
	public:
		T* m_ptr;
		Ptr() : m_ptr(0) {}
		Ptr(T* p) : m_ptr(p) {}
		Ptr(const Ptr& o) : m_ptr(o.m_ptr) {}
		Ptr& operator=(T* p) { m_ptr = p; return *this; }
		Ptr& operator=(const Ptr& o) { m_ptr = o.m_ptr; return *this; }
		T* operator->() const { return m_ptr; }
		T& operator*() const { return *m_ptr; }
		operator T*() const { return m_ptr; }
		T* Get() const { return m_ptr; }
		bool operator!() const { return m_ptr == 0; }
		operator bool() const { return m_ptr != 0; }
	};

	class Vec3f
	{
	public:
		float x, y, z;
		Vec3f() : x(0), y(0), z(0) {}
		Vec3f(float X, float Y, float Z) : x(X), y(Y), z(Z) {}
		float& operator[](int i) { return (&x)[i]; }
		float operator[](int i) const { return (&x)[i]; }
		float GetSquareLength() const { return x*x + y*y + z*z; }
		float GetSquareLength2d() const { return x*x + z*z; }
		float GetLength() const { return sqrtf(GetSquareLength()); }
	};

	class Vec2f
	{
	public:
		float x, y;
		Vec2f() : x(0), y(0) {}
		Vec2f(float X, float Y) : x(X), y(Y) {}
	};

	class WorldElement
	{
	public:
		virtual ~WorldElement() {}
		virtual int GetType() const { return 0; }
	};

	enum { TypeBoxObstacle = 1, TypeCylinderObstacle = 2 };

	class SpatializedCylinder
	{
	public:
		Vec3f GetPosition() const { return Vec3f(); }
		float GetRadius() const { return 1.f; }
	};

	class BoxObstacle : public WorldElement
	{
	public:
		virtual int GetType() const { return TypeBoxObstacle; }
		KyUInt32 GetSpatializedCylinderCount() const { return 1; }
		const SpatializedCylinder& GetSpatializedCylinder(int) const { static SpatializedCylinder c; return c; }
	};

	class CylinderObstacle : public WorldElement
	{
	public:
		virtual int GetType() const { return TypeCylinderObstacle; }
	};

	class Database {};
	class World
	{
	public:
		Database* GetDatabase(int) { return &m_db; }
		Database m_db;
	};
	class NavData {};
	class CoordSystem {};
	class GeneratorParameters {};
	class NavGraphEdgePtr {};
	enum NavGraphEdgeDirection { NAVGRAPHEDGE_BIDIRECTIONAL = 0 };
	enum PointOfInterestType
	{
		PointOfInterestType_Undefined = 0,
		PointOfInterestType_FirstClient = 1,
	};

	class LivePath
	{
	public:
		int GetPathValidityStatus() const { return 0; }
	};

	namespace IAvoidanceComputer
	{
		enum AvoidanceResult
		{
			NoAvoidance = 0,
			SlowDown = 1,
			Accelerate = 2,
			Turn = 3,
			Stop = 4,
			SolutionNotFound = 5,
		};
	}

	class Trajectory
	{
	public:
		IAvoidanceComputer::AvoidanceResult m_avoidanceResult;
		Trajectory() : m_avoidanceResult(IAvoidanceComputer::NoAvoidance) {}
		IAvoidanceComputer::AvoidanceResult GetAvoidanceResult() const { return m_avoidanceResult; }
	};

	class IPathFinderQuery
	{
	public:
		int m_processStatus;
		IPathFinderQuery() : m_processStatus(0) {}
		virtual ~IPathFinderQuery() {}
		int GetPathFinderResult() const { return 0; }
		int GetResult() const { return 0; }
	};

	class BaseAStarQuery : public IPathFinderQuery
	{
	public:
		Vec3f m_dest;
		const Vec3f& GetDestPos() const { return m_dest; }
	};

	template<typename T>
	class AStarQuery : public BaseAStarQuery {};

	struct AStarCustomizer_Default {};

	class BotUpdateConfig {};

	class Bot
	{
	public:
		LivePath m_livePath;
		Trajectory m_traj;
		IPathFinderQuery* m_pathFinderQuery;
		int m_visualDebugId;
		Bot() : m_pathFinderQuery(0), m_visualDebugId(0) {}
		int GetVisualDebugId() const { return m_visualDebugId; }
		int GetProgressOnLivePathStatus() const { return 0; }
		int GetTargetOnLivePathStatus() const { return 0; }
		LivePath& GetLivePath() { return m_livePath; }
		const LivePath& GetLivePath() const { return m_livePath; }
		Trajectory* GetTrajectory() { return &m_traj; }
		IPathFinderQuery* GetPathFinderQuery() { return m_pathFinderQuery; }
	};

	class PointOfInterest {};
	class IPerfMarkerInterface
	{
	public:
		virtual ~IPerfMarkerInterface() {}
		virtual void Begin(const char*) {}
		virtual void End() {}
	};
	class IVisualGeometry {};
	class IParallelForInterface {};
	class GeneratorInputProducer {};
}
