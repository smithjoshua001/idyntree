/**
 * Copyright (C) 2015 IIT - Istituto Italiano di Tecnologia - http://www.iit.it
 * Author: Silvio Traversaro
 * CopyPolicy: Released under the terms of the GNU LGPL v2.0 (or any later version)
 *
 * The development of this software was supported by the FP7 EU project
 * CoDyCo (No. 600716 ICT 2011.2.1 Cognitive Systems and Robotics (b))
 * http://www.codyco.eu
 */

#include <iDynTree/KinDynComputations.h>

#include <iDynTree/Core/ClassicalAcc.h>
#include <iDynTree/Core/MatrixDynSize.h>
#include <iDynTree/Core/VectorDynSize.h>
#include <iDynTree/Core/Twist.h>
#include <iDynTree/Core/Transform.h>
#include <iDynTree/Core/Rotation.h>
#include <iDynTree/Core/Utils.h>
#include <iDynTree/Core/SpatialAcc.h>
#include <iDynTree/Core/SpatialInertia.h>
#include <iDynTree/Core/Wrench.h>

#include <iDynTree/Core/EigenHelpers.h>

#include <iDynTree/Model/Model.h>
#include <iDynTree/Model/Traversal.h>
#include <iDynTree/Model/FreeFloatingState.h>
#include <iDynTree/Model/LinkState.h>
#include <iDynTree/Model/ForwardKinematics.h>

#include <iDynTree/ModelIO/URDFModelImport.h>

#include <cassert>
#include <iostream>
#include <fstream>

namespace iDynTree
{

// \todo TODO find a better way to handle the world index, and
// in general to handle the key used for semantics
const int WORLD_INDEX = -100;
unsigned int DEFAULT_DYNAMICS_COMPUTATION_FRAME_INDEX=10000;
std::string DEFAULT_DYNAMICS_COMPUTATION_FRAME_NAME="iDynTreeDynCompDefaultFrame";

struct KinDynComputations::KinDynComputationsPrivateAttributes
{
private:
    // Disable copy constructor and copy operator (move them to = delete when we support C++11)
    KinDynComputationsPrivateAttributes(const KinDynComputationsPrivateAttributes&other)
    {
        assert(false);
    }

    KinDynComputationsPrivateAttributes& operator=(const Traversal& other)
    {
        assert(false);

        return *this;
    }


public:
    bool m_isModelValid;

    // Model used for dynamics computations
    iDynTree::Model m_robot_model;

    // Traversal (i.e. visit order of the links) used for dynamics computations
    // this defines the link that is used as a floating base
    iDynTree::Traversal m_traversal;

    // State of the model
    // Frame where the reference frame is the world one
    // and the frame is the base link one
    iDynTree::FreeFloatingPos m_pos;

    // Velocity of the floating system
    // (Warning: this members is designed to work with the low-level
    // dynamics algorithms of iDynTree , and so it is set throught
    //  setRobotState with an appropriate conversion). In particular
    // the setRobotState sets the mixed base velocity (i.e. orientation
    // of the velocity is the one of inertial/world frame, while the point
    // is the origin of the base frame) while this class encodes the
    // base velocity with the orientation of the base and pint is the base
    // origin (so-called "body" velocity).
    //iDynTree::FreeFloatingVel m_vel;

    // Forward kinematics data structure
    // true whenever computePosition has been called
    // since the last call to setRobotState
    bool m_isFwdKinematicsUpdated;

    // storage of forward position kinematics results
    iDynTree::LinkPositions m_linkPos;

    // storage of forward velocity kinematics results
    // (to add)
    //iDynTree::LinkVelArray m_linkVel;

    KinDynComputationsPrivateAttributes()
    {
        m_isModelValid = false;
        m_isFwdKinematicsUpdated = false;
    }
};

KinDynComputations::KinDynComputations():
pimpl(new KinDynComputationsPrivateAttributes)
{
}

KinDynComputations::KinDynComputations(const KinDynComputations & other)
{
    // copyng the class is disabled until we get rid of the legacy implementation
    assert(false);
}

KinDynComputations& KinDynComputations::operator=(const KinDynComputations& other)
{
    /*
    if(this != &other)
    {
        *pimpl = *(other.pimpl);
    }
    return *this;
    */
    // copyng the class is disable until we get rid of the legacy implementation
    assert(false);

    return *this;
}

KinDynComputations::~KinDynComputations()
{
    delete this->pimpl;
}

//////////////////////////////////////////////////////////////////////////////
////// Private Methods
//////////////////////////////////////////////////////////////////////////////

void KinDynComputations::invalidateCache()
{
    this->pimpl->m_isFwdKinematicsUpdated = false;
}

void KinDynComputations::resizeInternalDataStructures()
{
    assert(this->pimpl->m_isModelValid);

    this->pimpl->m_pos.resize(this->pimpl->m_robot_model);
    this->pimpl->m_linkPos.resize(this->pimpl->m_robot_model);
    //this->pimpl->m_linkVel.resize(this->pimpl->m_robot_model);
}

int KinDynComputations::getFrameIndex(const std::string& frameName) const
{
    // Currently KDL::CoDyCo::UndirectedTree mixes the concepts of frames and links
    // see https://github.com/robotology/codyco-modules/issues/39
    // Once we have a proper iDynTree::Model, we can properly implement
    // the difference between frame and link
    int index = this->pimpl->m_robot_model.getFrameIndex(frameName);
    reportErrorIf(index < 0, "KinDynComputations::getFrameIndex", "requested frameName not found in model");
    return index;
}

std::string KinDynComputations::getFrameName(int frameIndex) const
{
    return this->pimpl->m_robot_model.getFrameName(frameIndex);
}




void KinDynComputations::computeFwdKinematics()
{
    if( this->pimpl->m_isFwdKinematicsUpdated )
    {
        return;
    }

    // Compute position kinematics (to implement ForwardPosVelKinematics
    bool ok = ForwardPositionKinematics(this->pimpl->m_robot_model,
                                        this->pimpl->m_traversal,
                                        this->pimpl->m_pos,
                                        this->pimpl->m_linkPos);

    this->pimpl->m_isFwdKinematicsUpdated = ok;

    return;
}

bool KinDynComputations::loadRobotModelFromFile(const std::string& filename,
                                                  const std::string& filetype)
{
    if( filetype != "urdf" )
    {
        std::cerr << "[ERROR] unknown format " << filetype <<
                     " . Currently only the urdf format is supported." << std::endl;
        return false;
    }

    std::ifstream ifs(filename.c_str());

    if( !ifs )
    {
        std::cerr << "[ERROR] impossible to open file " << filename << std::endl;
        return false;
    }

    std::string model_string( (std::istreambuf_iterator<char>(ifs) ),
                            (std::istreambuf_iterator<char>()    ) );

    return this->loadRobotModelFromString(model_string);
}

bool KinDynComputations::loadRobotModelFromString(const std::string& modelString,
                                                  const std::string& filetype)
{
    if( filetype != "urdf" )
    {
        std::cerr << "[ERROR] unknown format " << filetype <<
                     " . Currently only the urdf format is supported." << std::endl;
        return false;
    }

    Model model;
    bool ok = modelFromURDFString(modelString,model);

    if( !ok )
    {
        std::cerr << "[ERROR] error in loading robot model" << std::endl;
        return false;
    }
    else
    {
        return this->loadRobotModel(model);
    }
}

bool KinDynComputations::loadRobotModel(const Model& model)
{
    this->pimpl->m_robot_model = model;
    this->pimpl->m_isModelValid = true;
    this->pimpl->m_robot_model.computeFullTreeTraversal(this->pimpl->m_traversal);
    this->resizeInternalDataStructures();
    this->invalidateCache();
    return true;
}



bool KinDynComputations::isValid()
{
    return (this->pimpl->m_isModelValid);
}

std::string KinDynComputations::getFloatingBase() const
{
    LinkIndex base_link = this->pimpl->m_traversal.getBaseLink()->getIndex();
    return this->pimpl->m_robot_model.getLinkName(base_link);
}

bool KinDynComputations::setFloatingBase(const std::string& floatingBaseName)
{
    LinkIndex newFloatingBaseLinkIndex = this->pimpl->m_robot_model.getLinkIndex(floatingBaseName);
    return this->pimpl->m_robot_model.computeFullTreeTraversal(this->pimpl->m_traversal,newFloatingBaseLinkIndex);
}

unsigned int KinDynComputations::getNrOfLinks() const
{
    return this->pimpl->m_robot_model.getNrOfLinks();
}

const Model& KinDynComputations::getRobotModel() const
{
    return this->pimpl->m_robot_model;
}


//////////////////////////////////////////////////////////////////////////////
//// Degrees of freedom related methods
//////////////////////////////////////////////////////////////////////////////

unsigned int KinDynComputations::getNrOfDegreesOfFreedom() const
{
    return (unsigned int)this->pimpl->m_robot_model.getNrOfDOFs();
}

std::string KinDynComputations::getDescriptionOfDegreeOfFreedom(int dof_index)
{
    return this->pimpl->m_robot_model.getJointName(dof_index);
}

std::string KinDynComputations::getDescriptionOfDegreesOfFreedom()
{
    std::stringstream ss;

    for(unsigned int dof = 0; dof < this->getNrOfDegreesOfFreedom(); dof++ )
    {
        ss << "DOF Index: " << dof << " Name: " <<  this->getDescriptionOfDegreeOfFreedom(dof) << std::endl;
    }

    return ss.str();
}

//////////////////////////////////////////////////////////////////////////////
//// Links related methods
//////////////////////////////////////////////////////////////////////////////

/*
unsigned int DynamicsRegressorGenerator::getNrOfLinks() const
{
    assert(false);
    return (unsigned int)this->pimpl->m_pLegacyGenerator->getNrOfDOFs();
}

std::string DynamicsRegressorGenerator::getDescriptionOfLink(int link_index)
{

}

std::string DynamicsRegressorGenerator::getDescriptionOfLinks()
{

}*/



bool KinDynComputations::setRobotState(const VectorDynSize& q,
                                       const VectorDynSize& q_dot,
                                       const Vector3& world_gravity)
{
    Transform world_T_base = Transform::Identity();
    Twist base_velocity = Twist::Zero();

    return setRobotState(world_T_base,q,
                         base_velocity,q_dot,
                         world_gravity);
}

bool KinDynComputations::setRobotState(const Transform& world_T_base,
                                       const VectorDynSize& q,
                                       const Twist& base_velocity,
                                       const VectorDynSize& q_dot,
                                       const Vector3& world_gravity)
{
    bool ok = true;

    if( !ok )
    {
        std::cerr << "DynamicsRegressorGenerator::setRobotState failed" << std::endl;
        return false;
    }

    this->invalidateCache();

    // Save gravity \todo
    // this->pimpl->m_gravityAcc = world_gravity;
    this->pimpl->m_pos.worldBasePos() = world_T_base;
    toEigen(this->pimpl->m_pos.jointPos()) = toEigen(q);

    // Handle mixed --> body transform

    return true;
}

Transform KinDynComputations::getWorldBaseTransform()
{
    return this->pimpl->m_pos.worldBasePos();
}

Twist KinDynComputations::getBaseTwist()
{
    // to implement
    assert(false);
    return Twist::Zero();
}

bool KinDynComputations::getJointPos(VectorDynSize& q)
{
    q.resize(this->pimpl->m_robot_model.getNrOfPosCoords());
    toEigen(q) = toEigen(this->pimpl->m_pos.jointPos());
    return true;
}

bool KinDynComputations::getJointVel(VectorDynSize& dq)
{
    //dq = this->pimpl->m_pos.jointPos();
    //return true;
    // to implement
    return false;
}


Transform KinDynComputations::getRelativeTransform(const std::string& refFrameName,
                                                     const std::string& frameName)
{
    int refFrameIndex = getFrameIndex(refFrameName);
    int frameIndex = getFrameIndex(frameName);
    if( frameIndex < 0 )
    {
        reportError("KinDynComputations","getRelativeTransform","unknown frameName");
        return Transform::Identity();
    }
    else if( refFrameIndex < 0 )
    {
        reportError("KinDynComputations","getRelativeTransform","unknown refFrameName");
        return Transform::Identity();
    }
    else
    {
        return this->getRelativeTransform(refFrameIndex,frameIndex);
    }
}

Transform KinDynComputations::getRelativeTransform(const iDynTree::FrameIndex refFrameIndex,
                                                   const iDynTree::FrameIndex frameIndex)
{
    if( frameIndex >= this->getNrOfFrames() )
    {
        reportError("KinDynComputations","getRelativeTransform","frameIndex out of bound");
        return iDynTree::Transform::Identity();
    }

    if( refFrameIndex >= this->getNrOfFrames() )
    {
        reportError("KinDynComputations","getRelativeTransform","refFrameIndex out of bound");
        return iDynTree::Transform::Identity();
    }

    // compute fwd kinematics (if necessary)
    this->computeFwdKinematics();

    Transform world_H_frame = getWorldTransform(frameIndex);
    Transform world_H_refFrame = getWorldTransform(refFrameIndex);

    Transform refFrame_H_frame = world_H_refFrame.inverse()*world_H_frame;

    // Set semantics
    // Setting position semantics
    PositionSemantics posSem;
    posSem.setCoordinateFrame(refFrameIndex);
    posSem.setReferencePoint(refFrameIndex);
    posSem.setPoint(frameIndex);

    refFrame_H_frame.getSemantics().setPositionSemantics(posSem);

    // Setting rotation semantics
    RotationSemantics rotSem;
    rotSem.setReferenceOrientationFrame(refFrameIndex);
    rotSem.setCoordinateFrame(refFrameIndex);
    rotSem.setOrientationFrame(frameIndex);

    refFrame_H_frame.getSemantics().setRotationSemantics(rotSem);

    return refFrame_H_frame;
}

Transform KinDynComputations::getRelativeTransformExplicit(const iDynTree::FrameIndex refFrameOriginIndex,
                                                           const iDynTree::FrameIndex refFrameOrientationIndex,
                                                           const iDynTree::FrameIndex    frameOriginIndex,
                                                           const iDynTree::FrameIndex    frameOrientationIndex)
{
    if( refFrameOriginIndex >= this->pimpl->m_robot_model.getNrOfFrames() )
    {
        reportError("KinDynComputations","getRelativeTransformExplicit","refFrameOriginIndex out of bound");
        return iDynTree::Transform::Identity();
    }

    if( refFrameOrientationIndex >= this->pimpl->m_robot_model.getNrOfFrames() )
    {
        reportError("KinDynComputations","getRelativeTransformExplicit","refFrameOrientationIndex out of bound");
        return iDynTree::Transform::Identity();
    }

    if( frameOriginIndex >= this->pimpl->m_robot_model.getNrOfFrames() )
    {
        reportError("KinDynComputations","getRelativeTransformExplicit","frameOriginIndex out of bound");
        return iDynTree::Transform::Identity();
    }

    if( frameOrientationIndex >= this->pimpl->m_robot_model.getNrOfFrames() )
    {
        reportError("KinDynComputations","getRelativeTransformExplicit","frameOrientationIndex out of bound");
        return iDynTree::Transform::Identity();
    }

    // compute fwd kinematics (if necessary)
    this->computeFwdKinematics();


    // This part can be probably made more efficient, but unless a need for performance
    // arise I prefer it to be readable for now

    Transform world_H_refFrameOrientation = getWorldTransform(refFrameOrientationIndex);
    Transform world_H_framOrientation = getWorldTransform(frameOrientationIndex);

    // Orientation part
    // refFrameOrientation_R_frameOrientation = world_R_refFrameOrientation^{-1} * world_R_frameOrientation
    Rotation refFrameOrientation_R_frameOrientation = world_H_refFrameOrientation.getRotation().inverse()*world_H_framOrientation.getRotation();

    // Position part
    // refFrameOrientation_p_refFrameOrigin_frameOrigin =
    //      refFrameOrientation_R_refFramePosition * refFramePosition_p_refFramePositon_framePosition
    Rotation refFrameOrientation_R_refFramePosition = getRelativeTransform(refFrameOrientationIndex,refFrameOriginIndex).getRotation();
    Position refFrameOrientation_p_refFrameOrigin_frameOrigin =
        refFrameOrientation_R_refFramePosition*(this->getRelativeTransform(refFrameOriginIndex,frameOriginIndex).getPosition());

    return Transform(refFrameOrientation_R_frameOrientation,refFrameOrientation_p_refFrameOrigin_frameOrigin);
}

Transform KinDynComputations::getWorldTransform(std::string frameName)
{
    int frameIndex = getFrameIndex(frameName);
    if( frameIndex < 0 )
    {
        return Transform::Identity();
    }
    else
    {
        return getWorldTransform(frameIndex);
    }
}

Transform KinDynComputations::getWorldTransform(const FrameIndex frameIndex)
{
    if( frameIndex >= this->getNrOfFrames() )
    {
        reportError("KinDynComputations","getWorldTransform","frameIndex out of bound");
        return iDynTree::Transform::Identity();
    }

    // compute fwd kinematics (if necessary)
    this->computeFwdKinematics();

    if( !this->pimpl->m_isFwdKinematicsUpdated )
    {
        reportError("KinDynComputations","getWorldTransform","error in computing fwd kinematics");
        return iDynTree::Transform::Identity();
    }


    iDynTree::Transform world_H_frame;

    // If the frame is associated to a link,
    // then return directly the content in linkPos
    if( this->pimpl->m_robot_model.isValidLinkIndex(frameIndex) )
    {
        world_H_frame = this->pimpl->m_linkPos(frameIndex);
    }
    else
    {
        // otherwise we extract from the result of position kinematics
        // the transform between the world and the link at which the
        // frame is attached
        iDynTree::Transform world_H_link =
            this->pimpl->m_linkPos(this->pimpl->m_robot_model.getFrameLink(frameIndex));
        iDynTree::Transform link_H_frame =
            this->pimpl->m_robot_model.getFrameTransform(frameIndex);

        world_H_frame = world_H_link*link_H_frame;
    }

    // Setting position semantics
    PositionSemantics posSem;
    posSem.setCoordinateFrame(WORLD_INDEX);
    posSem.setReferencePoint(WORLD_INDEX);
    posSem.setPoint(frameIndex);

    world_H_frame.getSemantics().setPositionSemantics(posSem);

    // Setting rotation semantics
    RotationSemantics rotSem;
    rotSem.setReferenceOrientationFrame(WORLD_INDEX);
    rotSem.setCoordinateFrame(WORLD_INDEX);
    rotSem.setOrientationFrame(frameIndex);

    world_H_frame.getSemantics().setRotationSemantics(rotSem);

    return world_H_frame;
}

unsigned int KinDynComputations::getNrOfFrames() const
{
    return this->pimpl->m_robot_model.getNrOfFrames();
}


}

