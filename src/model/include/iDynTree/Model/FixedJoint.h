/*
 * Copyright (C) 2015 Fondazione Istituto Italiano di Tecnologia
 * Authors: Silvio Traversaro
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef IDYNTREE_FIXED_JOINT_H
#define IDYNTREE_FIXED_JOINT_H

#include <iDynTree/Core/Transform.h>

#include <iDynTree/Model/Indeces.h>
#include <iDynTree/Model/IJoint.h>

namespace iDynTree
{
    /**
     * Class representing a fixed joint, i.e. a joint that rigidly attaches two links.
     *
     * \ingroup iDynTreeModel
     */
    class FixedJoint : public IJoint
    {
    private:
        JointIndex  m_index;
        std::size_t m_posCoordsOffset;
        std::size_t m_DOFsOffset;
        LinkIndex link1;
        LinkIndex link2;
        Transform link1_X_link2;
        Transform link2_X_link1;

    public:
        /**
         * Constructor
         */
        FixedJoint(const LinkIndex link1, const LinkIndex link2,
                   const Transform& link1_X_link2);

        /**
         * Copy constructor
         */
        FixedJoint(const FixedJoint & other);

        /**
         * Destructor
         */
        virtual ~FixedJoint();

        // Documentation inherited
        virtual IJoint * clone() const;

        // Documentation inherited
        virtual unsigned int getNrOfPosCoords() const;

        // Documentation inherited
        virtual unsigned int getNrOfDOFs() const;

        // Documentation inherited
        virtual void setAttachedLinks(const LinkIndex link1, const LinkIndex link2);

        // Documentation inherited
        virtual void setRestTransform(const Transform& link1_X_link2);

        // Documentation inherited
        virtual LinkIndex getFirstAttachedLink() const;

        // Documentation inherited
        virtual LinkIndex getSecondAttachedLink() const;

        // Documentation inherited
        virtual Transform getRestTransform(const LinkIndex child,
                                            const LinkIndex parent) const;


        // Documentation inherited
        virtual const Transform & getTransform(const VectorDynSize & jntPos,
                                               const LinkIndex child,
                                               const LinkIndex parent) const;

        // Documentation inherited
        TransformDerivative getTransformDerivative(const VectorDynSize & jntPos,
                                                   const LinkIndex child,
                                                   const LinkIndex parent,
                                                   const int posCoord_i) const;

        // Documentation inherited
        virtual SpatialMotionVector getMotionSubspaceVector(int dof_i,
                                                            const LinkIndex child,
                                                            const LinkIndex parent) const;

         // Documentation inherited
        virtual void computeChildPosVelAcc(const VectorDynSize & jntPos,
                                           const VectorDynSize & jntVel,
                                           const VectorDynSize & jntAcc,
                                           LinkPositions & linkPositions,
                                           LinkVelArray & linkVels,
                                           LinkAccArray & linkAccs,
                                           const LinkIndex child,
                                           const LinkIndex parent) const;

        // Documentation inherited
        virtual void computeChildVelAcc(const VectorDynSize & jntPos,
                                        const VectorDynSize & jntVel,
                                        const VectorDynSize & jntAcc,
                                        LinkVelArray & linkVels,
                                        LinkAccArray & linkAccs,
                                        const LinkIndex child, const LinkIndex parent) const;

        // Documentation inherited
        virtual void computeJointTorque(const VectorDynSize & jntPos, const Wrench & internalWrench,
                                        const LinkIndex linkThatAppliesWrench, const LinkIndex linkOnWhichWrenchIsApplied,
                                        VectorDynSize & jntTorques) const;

        // Documentation inherited
        virtual void setIndex(JointIndex & _index);

        // Documentation inherited
        virtual JointIndex getIndex() const;

        // Documentation inherited
        virtual void setPosCoordsOffset(const size_t _index);

        // Documentation inherited
        virtual size_t getPosCoordsOffset() const;

        // Documentation inherited
        virtual void setDOFsOffset(const size_t _index);

        // Documentation inherited
        virtual size_t getDOFsOffset() const;
    };
}

#endif /* IDYNTREE_FIXED_JOINT_H */