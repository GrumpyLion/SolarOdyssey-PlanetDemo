//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright (c) 2008-2021 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

#ifndef PX_PHYSICS_NP_CAST
#define PX_PHYSICS_NP_CAST

#include "PxPhysXConfig.h"
#include "NpScene.h"
#include "NpRigidDynamic.h"
#include "NpRigidStatic.h"
#include "NpArticulation.h"
#include "NpArticulationReducedCoordinate.h"
#include "NpArticulationLink.h"
#include "NpArticulationJoint.h"
#include "NpAggregate.h"

namespace physx
{
	// PT: Scb-to-Np casts

	PX_FORCE_INLINE const NpScene* getNpScene(const Scb::Scene* scene)
	{
		return reinterpret_cast<const NpScene*>(reinterpret_cast<const char*>(scene) - NpScene::getScbSceneOffset());
	}

	PX_FORCE_INLINE const NpRigidDynamic* getNpRigidDynamic(const Scb::Body* scbBody)
	{
		return reinterpret_cast<const NpRigidDynamic*>(reinterpret_cast<const char*>(scbBody) - NpRigidDynamic::getScbBodyOffset());
	}

	PX_FORCE_INLINE const NpRigidStatic* getNpRigidStatic(const Scb::RigidStatic* scbRigidStatic)
	{
		return reinterpret_cast<const NpRigidStatic*>(reinterpret_cast<const char*>(scbRigidStatic) - NpRigidStatic::getScbRigidStaticOffset());
	}

	PX_FORCE_INLINE const NpShape* getNpShape(const Scb::Shape* scbShape)
	{
		return reinterpret_cast<const NpShape*>(reinterpret_cast<const char*>(scbShape) - NpShape::getScbShapeOffset());
	}

	PX_FORCE_INLINE const NpArticulationLink* getNpArticulationLink(const Scb::Body* scbArticulationLink)
	{
		return reinterpret_cast<const NpArticulationLink*>(reinterpret_cast<const char*>(scbArticulationLink) - NpArticulationLink::getScbBodyOffset());
	}

	PX_FORCE_INLINE const NpArticulation* getNpArticulation(const Scb::Articulation* scbArticulation)
	{
		return reinterpret_cast<const NpArticulation*>(reinterpret_cast<const char*>(scbArticulation) - NpArticulation::getScbArticulationOffset());
	}

	PX_FORCE_INLINE const NpArticulationReducedCoordinate* getNpArticulationRC(const Scb::Articulation* scbArticulation)
	{
		return reinterpret_cast<const NpArticulationReducedCoordinate*>(reinterpret_cast<const char*>(scbArticulation) - NpArticulationReducedCoordinate::getScbArticulationOffset());
	}

	PX_FORCE_INLINE const NpArticulationJoint* getNpArticulationJoint(const Scb::ArticulationJoint* scbArticulationJoint)
	{
		return reinterpret_cast<const NpArticulationJoint*>(reinterpret_cast<const char*>(scbArticulationJoint) - NpArticulationJoint::getScbArticulationJointOffset());
	}

	PX_FORCE_INLINE const NpArticulationJointReducedCoordinate* getNpArticulationJointReducedCoordinate(const Scb::ArticulationJoint* scbArticulationJoint)
	{
		return reinterpret_cast<const NpArticulationJointReducedCoordinate*>(reinterpret_cast<const char*>(scbArticulationJoint) - NpArticulationJointReducedCoordinate::getScbArticulationJointOffset());
	}

	PX_FORCE_INLINE const NpAggregate* getNpAggregate(const Scb::Aggregate* aggregate)
	{
		return reinterpret_cast<const NpAggregate*>(reinterpret_cast<const char*>(aggregate) - NpAggregate::getScbAggregateOffset());
	}

}

#endif
