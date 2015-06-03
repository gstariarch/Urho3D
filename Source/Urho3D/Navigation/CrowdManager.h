//
// Copyright (c) 2008-2015 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include "../Scene/Component.h"

#ifdef DT_POLYREF64
typedef uint64_t dtPolyRef;
#else
typedef unsigned int dtPolyRef;
#endif

class dtCrowd;
struct dtCrowdAgent;

namespace Urho3D
{

class CrowdAgent;
class NavigationMesh;

/// Crowd manager scene component. Should be added only to the root scene node.
class URHO3D_API CrowdManager : public Component
{
    OBJECT(CrowdManager);
    friend class CrowdAgent;

public:
    /// Construct.
    CrowdManager(Context* context);
    /// Destruct.
    virtual ~CrowdManager();
    /// Register object factory.
    static void RegisterObject(Context* context);
    /// Apply attribute changes that can not be applied immediately. Called after scene load or a network update.
    virtual void ApplyAttributes();

    /// Draw the agents' pathing debug data.
    virtual void DrawDebugGeometry(DebugRenderer* debug, bool depthTest);
    /// Add debug geometry to the debug renderer.
    void DrawDebugGeometry(bool depthTest);

    /// Set the crowd target position. The target position is set to all crowd agents found in the specified node, excluding crowd agent which does not have acceleration. Defaulted to scene node.
    void SetCrowdTarget(const Vector3& position, Node* node = 0);
    /// Set the crowd move velocity. The move velocity is applied to all crowd agents found in the specified node, excluding crowd agent which does not have acceleration. Defaulted to scene node.
    void SetCrowdVelocity(const Vector3& velocity, Node* node = 0);
    /// Reset any crowd target for all crowd agents found in the specified node, excluding crowd agent which does not have acceleration. Defaulted to scene node.
    void ResetCrowdTarget(Node* node = 0);
    /// Set the maximum number of agents.
    void SetMaxAgents(unsigned maxAgents);
    /// Set the maximum radius of any agent.
    void SetMaxAgentRadius(float maxAgentRadius);
    /// Assigns the navigation mesh for the crowd.
    void SetNavigationMesh(NavigationMesh* navMesh);
    /// Set the cost of an area-type for the specified navigation filter type.
    void SetAreaCost(unsigned filterTypeID, unsigned areaID, float cost);

    /// Get all the crowd agent components in the specified node hierarchy. If the node is not specified then use scene node. When inCrowdFilter is set to true then only get agents that are in the crowd.
    PODVector<CrowdAgent*> GetAgents(Node* node = 0, bool inCrowdFilter = true) const;
    /// Find the nearest point on the navigation mesh to a given point using the crowd initialized query extent (based on maxAgentRadius) and the specified crowd filter type.
    Vector3 FindNearestPoint(const Vector3& point, int filterType, dtPolyRef* nearestRef = 0);
    /// Try to move along the surface from one point to another using the crowd initialized query extent (based on maxAgentRadius) and the specified crowd filter type.
    Vector3 MoveAlongSurface(const Vector3& start, const Vector3& end, int filterType, int maxVisited = 3);
    /// Find a path between world space points using the crowd initialized query extent (based on maxAgentRadius) and the specified crowd filter type. Return non-empty list of points if successful.
    void FindPath(PODVector<Vector3>& dest, const Vector3& start, const Vector3& end, int filterType);
    /// Return a random point on the navigation mesh using the crowd initialized query extent (based on maxAgentRadius) and the specified crowd filter type.
    Vector3 GetRandomPoint(int filterType, dtPolyRef* randomRef = 0);
    /// Return a random point on the navigation mesh within a circle using the crowd initialized query extent (based on maxAgentRadius) and the specified crowd filter type. The circle radius is only a guideline and in practice the returned point may be further away.
    Vector3 GetRandomPointInCircle(const Vector3& center, float radius, int filterType, dtPolyRef* randomRef = 0);
    /// Return distance to wall from a point using the crowd initialized query extent (based on maxAgentRadius) and the specified crowd filter type. Maximum search radius must be specified.
    float GetDistanceToWall(const Vector3& point, float radius, int filterType, Vector3* hitPos = 0, Vector3* hitNormal = 0);
    /// Perform a walkability raycast on the navigation mesh between start and end using the crowd initialized query extent (based on maxAgentRadius) and the specified crowd filter type. Return the point where a wall was hit, or the end point if no walls.
    Vector3 Raycast(const Vector3& start, const Vector3& end, int filterType, Vector3* hitNormal = 0);
    /// Get the maximum number of agents.
    unsigned GetMaxAgents() const { return maxAgents_; }
    /// Get the maximum radius of any agent.
    float GetMaxAgentRadius() const { return maxAgentRadius_; }
    /// Get the Navigation mesh assigned to the crowd.
    NavigationMesh* GetNavigationMesh() const { return navigationMesh_; }
    /// Get the cost of an area-type for the specified navigation filter type.
    float GetAreaCost(unsigned filterTypeID, unsigned areaID) const;

protected:
    /// Create internal Detour crowd object for the specified navigation mesh. If readdCrowdAgents is true then attempt to re-add existing agents in the previous crowd back to the newly created crowd.
    bool CreateCrowd(bool readdCrowdAgents = false);
    /// Create and adds an detour crowd agent, Agent's radius and height is set through the navigation mesh. Return -1 on error, agent ID on success.
    int AddAgent(CrowdAgent* agent, const Vector3& pos);
    /// Removes the detour crowd agent.
    void RemoveAgent(CrowdAgent* agent);

protected:
    /// Update the crowd simulation.
    void Update(float delta);
    /// Handle node being assigned.
    virtual void OnNodeSet(Node* node);
    /// Get the detour crowd agent.
    const dtCrowdAgent* GetCrowdAgent(int agent);
    /// Get the internal detour crowd component.
    dtCrowd* GetCrowd() const { return crowd_; }

private:
    /// Handle the scene subsystem update event.
    void HandleSceneSubsystemUpdate(StringHash eventType, VariantMap& eventData);
    /// Handle full rebuilds of the navigation mesh.
    void HandleNavMeshFullRebuild(StringHash eventType, VariantMap& eventData);

    /// Internal Detour crowd object.
    dtCrowd* crowd_;
    /// NavigationMesh for which the crowd was created.
    WeakPtr<NavigationMesh> navigationMesh_;
    /// The maximum number of agents the crowd can manage.
    unsigned maxAgents_;
    /// The maximum radius of any agent that will be added to the crowd.
    float maxAgentRadius_;
};

}
