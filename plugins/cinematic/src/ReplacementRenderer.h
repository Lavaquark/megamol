/*
 * ReplacementRenderer.h
*
* Copyright (C) 2018 by VISUS (Universitaet Stuttgart).
* Alle Rechte vorbehalten.
*/

#ifndef MEGAMOL_CINEMATIC_REPLACEMENTRENDERER_H_INCLUDED
#define MEGAMOL_CINEMATIC_REPLACEMENTRENDERER_H_INCLUDED

#include "mmcore/CallerSlot.h"
#include "mmcore/view/CallRender3D.h"
#include "mmcore/view/Renderer3DModule.h"
#include "mmcore/view/AbstractCallRender3D.h"
#include "mmcore/view/Input.h"
#include "mmcore/param/ParamSlot.h"

namespace megamol {
namespace cinematic {

    /*
     * Replacement rendering.
     */
    class ReplacementRenderer : public megamol::core::view::Renderer3DModule
    {
    public:


        /**
         * Answer the name of this module.
         *
         * @return The name of this module.
         */
        static const char *ClassName(void)
        {
            return "ReplacementRenderer";
        }

        /**
         * Answer a human readable description of this module.
         *
         * @return A human readable description of this module.
         */
        static const char *Description(void)
        {
            return "Offers replacement rendering.";
        }

        /**
         * Answers whether this module is available on the current system.
         *
         * @return 'true' if the module is available, 'false' otherwise.
         */
        static bool IsAvailable(void)
        {
            return true;
        }

        /** Ctor. */
        ReplacementRenderer(void);

        /** Dtor. */
        virtual ~ReplacementRenderer(void);

    protected:

        /**
         * Implementation of 'Create'.
         *
         * @return 'true' on success, 'false' otherwise.
         */
        virtual bool create(void);

        /**
         * Implementation of 'release'.
         */
        virtual void release(void);

        /**
         * The get extents callback. The module should set the members of
         * 'call' to tell the caller the extents of its data (bounding boxes
         * and times).
         *
         * @param call The calling call.
         *
         * @return The return value of the function.
         */
        virtual bool GetExtents(megamol::core::view::CallRender3D& call);

        /**
         * The Open GL Render callback.
         *
         * @param call The calling call.
         * @return The return value of the function.
         */
        virtual bool Render(megamol::core::view::CallRender3D& call);

    private:

        /**********************************************************************
         * variables
         **********************************************************************/
        
        // Enum of available key assignments
        enum keyAssignment {
            KEY_ASSIGN_NONE = 0,
            KEY_ASSIGN_O = 1,
            KEY_ASSIGN_I = 2,
            KEY_ASSIGN_J = 3,
            KEY_ASSIGN_K = 4
        };

        vislib::math::Cuboid<float> bbox;

        bool toggleReplacementRendering;

         /**********************************************************************
         * functions
         **********************************************************************/

        void drawBoundingBox(void);

        /**********************************************************************
        * callback stuff
        **********************************************************************/

        /** The renderer caller slot */
        core::CallerSlot rendererCallerSlot;

        /**********************************************************************
        * parameters
        **********************************************************************/

        core::param::ParamSlot replacementRenderingParam;
        core::param::ParamSlot toggleReplacementRenderingParam;
        core::param::ParamSlot replacementKeyParam;
        core::param::ParamSlot alphaParam;

    };

} /* end namespace cinematic */
} /* end namespace megamol */

#endif // MEGAMOL_CINEMATIC_REPLACEMENTRENDERER_H_INCLUDED