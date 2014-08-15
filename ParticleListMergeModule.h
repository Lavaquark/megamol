/*
 * ParticleListMergeModule.h
 *
 * Copyright (C) 2014 by CGV TU Dresden
 * Alle Rechte vorbehalten.
 */

#ifndef MEGAMOLCORE_PARTICLELISTMERGEMODULE_H_INCLUDED
#define MEGAMOLCORE_PARTICLELISTMERGEMODULE_H_INCLUDED
#if (defined(_MSC_VER) && (_MSC_VER > 1000))
#pragma once
#endif /* (defined(_MSC_VER) && (_MSC_VER > 1000)) */

#include "Module.h"
#include "CalleeSlot.h"
#include "CallerSlot.h"
#include "moldyn/MultiParticleDataCall.h"
#include "vislib/Cuboid.h"


namespace megamol {
namespace stdplugin {
namespace datatools {


    /**
     * In-Between management module to change time codes of a data set
     */
    class ParticleListMergeModule : public core::Module {
    public:

        /**
         * Answer the name of this module.
         *
         * @return The name of this module.
         */
        static const char *ClassName(void) {
            return "ParticleListMergeModule";
        }

        /**
         * Answer a human readable description of this module.
         *
         * @return A human readable description of this module.
         */
        static const char *Description(void) {
            return "Module to merge all lists from the core::moldyn::MultiParticleDataCall into a single list";
        }

        /**
         * Answers whether this module is available on the current system.
         *
         * @return 'true' if the module is available, 'false' otherwise.
         */
        static bool IsAvailable(void) {
            return true;
        }

        /**
         * Disallow usage in quickstarts
         *
         * @return false
         */
        static bool SupportQuickstart(void) {
            return false;
        }

        /** Ctor. */
        ParticleListMergeModule(void);

        /** Dtor. */
        virtual ~ParticleListMergeModule(void);

    protected:

        /**
         * Implementation of 'Create'.
         *
         * @return 'true' on success, 'false' otherwise.
         */
        virtual bool create(void);

        /**
         * Implementation of 'Release'.
         */
        virtual void release(void);

    private:

        /**
         * Gets the data from the source.
         *
         * @param caller The calling call.
         *
         * @return 'true' on success, 'false' on failure.
         */
        bool getDataCallback(core::Call& caller);

        /**
         * Gets the data from the source.
         *
         * @param caller The calling call.
         *
         * @return 'true' on success, 'false' on failure.
         */
        bool getExtentCallback(core::Call& caller);

        /**
         * Copies the incoming data 'inDat' into the object's fields
         *
         * @param inDat The incoming data
         */
        void setData(core::moldyn::MultiParticleDataCall& inDat);

        /** The slot for publishing data to the writer */
        core::CalleeSlot outDataSlot;

        /** The slot for requesting data from the source */
        core::CallerSlot inDataSlot;

        /** The call for Transfer function */
        core::CallerSlot getTFSlot;

        /** The hash id of the data stored */
        size_t dataHash;

        /** The frame id of the data stored */
        unsigned int frameId;

        /** The single list of particles */
        core::moldyn::MultiParticleDataCall::Particles parts;

        /** The stored particle data */
        vislib::RawStorage data;

    };

} /* end namespace datatools */
} /* end namespace stdplugin */
} /* end namespace megamol */

#endif /* MEGAMOLCORE_PARTICLELISTMERGEMODULE_H_INCLUDED */
