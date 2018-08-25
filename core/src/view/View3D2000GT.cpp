/*
 * View3D2000GT.cpp
 *
 * Copyright (C) 2018 by VISUS (Universitaet Stuttgart).
 * Alle Rechte vorbehalten.
 */

#include "stdafx.h"
#include "mmcore/view/View3D2000GT.h"
#include "vislib/graphics/gl/IncludeAllGL.h"
#ifdef _WIN32
#    include <windows.h>
#endif /* _WIN32 */
#include "mmcore/CoreInstance.h"
#include "mmcore/misc/PngBitmapCodec.h"
#include "mmcore/param/BoolParam.h"
#include "mmcore/param/ButtonParam.h"
#include "mmcore/param/EnumParam.h"
#include "mmcore/param/FilePathParam.h"
#include "mmcore/param/FloatParam.h"
#include "mmcore/param/StringParam.h"
#include "mmcore/param/Vector3fParam.h"
#include "mmcore/utility/ColourParser.h"
#include "mmcore/view/CallRender3D2000GT.h"
#include "mmcore/view/CallRenderView.h" // TODO new call?
#include "mmcore/view/CameraParamOverride.h"
#include "vislib/Exception.h"
#include "vislib/String.h"
#include "vislib/StringSerialiser.h"
#include "vislib/Trace.h"
#include "vislib/graphics/BitmapImage.h"
#include "vislib/graphics/CameraParamsStore.h"
#include "vislib/math/Point.h"
#include "vislib/math/Quaternion.h"
#include "vislib/math/Vector.h"
#include "vislib/math/mathfunctions.h"
#include "vislib/sys/KeyCode.h"
#include "vislib/sys/Log.h"
#include "vislib/sys/sysfunctions.h"

using namespace megamol::core;
using namespace megamol::core::view;

/*
 * View3D2000GT::View3D2000GT
 */
View3D2000GT::View3D2000GT(void)
    : view::AbstractView3D()
    , AbstractCamParamSync()
    , cursor2d()
    , rendererSlot("rendering", "Connects the view to a Renderer")
    , lightDir(0.5f, -1.0f, -1.0f)
    , isCamLight(true)
    , bboxs()
    , showBBox("showBBox", "Bool parameter to show/hide the bounding box")
    , showLookAt("showLookAt", "Flag showing the look at point")
    , cameraSettingsSlot("camsettings", "The stored camera settings")
    , storeCameraSettingsSlot("storecam", "Triggers the storage of the camera settings")
    , restoreCameraSettingsSlot("restorecam", "Triggers the restore of the camera settings")
    , resetViewSlot("resetView", "Triggers the reset of the view")
    , firstImg(false)
    , isCamLightSlot(
          "light::isCamLight", "Flag whether the light is relative to the camera or to the world coordinate system")
    , lightDirSlot("light::direction", "Direction vector of the light")
    , lightColDifSlot("light::diffuseCol", "Diffuse light colour")
    , lightColAmbSlot("light::ambientCol", "Ambient light colour")
    , stereoFocusDistSlot("stereo::focusDist", "focus distance for stereo projection")
    , stereoEyeDistSlot("stereo::eyeDist", "eye distance for stereo projection")
    , overrideCall(NULL)
    , viewKeyMoveStepSlot("viewKey::MoveStep", "The move step size in world coordinates")
    , viewKeyRunFactorSlot("viewKey::RunFactor", "The factor for step size multiplication when running (shift)")
    , viewKeyAngleStepSlot("viewKey::AngleStep", "The angle rotate step in degrees")
    , mouseSensitivitySlot("viewKey::MouseSensitivity", "used for WASD mode")
    , viewKeyRotPointSlot("viewKey::RotPoint", "The point around which the view will be roateted")
    , viewKeyRotLeftSlot("viewKey::RotLeft", "Rotates the view to the left (around the up-axis)")
    , viewKeyRotRightSlot("viewKey::RotRight", "Rotates the view to the right (around the up-axis)")
    , viewKeyRotUpSlot("viewKey::RotUp", "Rotates the view to the top (around the right-axis)")
    , viewKeyRotDownSlot("viewKey::RotDown", "Rotates the view to the bottom (around the right-axis)")
    , viewKeyRollLeftSlot("viewKey::RollLeft", "Rotates the view counter-clockwise (around the view-axis)")
    , viewKeyRollRightSlot("viewKey::RollRight", "Rotates the view clockwise (around the view-axis)")
    , viewKeyZoomInSlot("viewKey::ZoomIn", "Zooms in (moves the camera)")
    , viewKeyZoomOutSlot("viewKey::ZoomOut", "Zooms out (moves the camera)")
    , viewKeyMoveLeftSlot("viewKey::MoveLeft", "Moves to the left")
    , viewKeyMoveRightSlot("viewKey::MoveRight", "Moves to the right")
    , viewKeyMoveUpSlot("viewKey::MoveUp", "Moves to the top")
    , viewKeyMoveDownSlot("viewKey::MoveDown", "Moves to the bottom")
    , toggleBBoxSlot("toggleBBox", "Button to toggle the bounding box")
    , bboxCol{1.0f, 1.0f, 1.0f, 0.625f}
    , bboxColSlot("bboxCol", "Sets the colour for the bounding box")
    , enableMouseSelectionSlot("enableMouseSelection", "Enable selecting and picking with the mouse")
    , showViewCubeSlot("viewcube::show", "Shows the view cube helper")
    , resetViewOnBBoxChangeSlot("resetViewOnBBoxChange", "whether to reset the view when the bounding boxes change")
    , mouseX(0.0f)
    , mouseY(0.0f)
    , mouseFlags(0)
    , timeCtrl()
    , toggleMouseSelection(false)
    , hookOnChangeOnlySlot("hookOnChange", "whether post-hooks are triggered when the frame would be identical") {

    using vislib::sys::KeyCode;

    // TODO implement

    this->rendererSlot.SetCompatibleCall<CallRender3D2000GTDescription>();
    this->MakeSlotAvailable(&this->rendererSlot);

    // TODO implement
}

/*
 * View3D2000GT::~View3D2000GT
 */
View3D2000GT::~View3D2000GT(void) {
    this->Release();
    this->overrideCall = nullptr; // DO NOT DELETE
}

/*
 * View3D2000GT::GetCameraSyncNumber
 */
unsigned int view::View3D2000GT::GetCameraSyncNumber(void) const {
    // TODO implement
    return 0;
}


/*
 * View3D2000GT::SerialiseCamera
 */
void view::View3D2000GT::SerialiseCamera(vislib::Serialiser& serialiser) const {
    // TODO implement
}


/*
 * View3D2000GT::DeserialiseCamera
 */
void view::View3D2000GT::DeserialiseCamera(vislib::Serialiser& serialiser) {
    // TODO implement
}

/*
 * View3D2000GT::Render
 */
void View3D2000GT::Render(const mmcRenderViewContext& context) {
    const float* bkgndCol = (this->overrideBkgndCol != nullptr) ? this->overrideBkgndCol : this->bkgndColour();
    glClearColor(bkgndCol[0], bkgndCol[1], bkgndCol[2], 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/*
 * View3D2000GT::ResetView
 */
void View3D2000GT::ResetView(void) {
    // TODO implement
}

/*
 * View3D2000GT::Resize
 */
void View3D2000GT::Resize(unsigned int width, unsigned int height) {
    // TODO implement
}

/*
 * View3D2000GT::SetCursor2DButtonState
 */
void View3D2000GT::SetCursor2DButtonState(unsigned int btn, bool down) {
    // TODO implement
}

/*
 * View3D2000GT::SetCursor2DPosition
 */
void View3D2000GT::SetCursor2DPosition(float x, float y) {
    // TODO implement
}

/*
 * View3D2000GT::SetInputModifier
 */
void View3D2000GT::SetInputModifier(mmcInputModifier mod, bool down) {
    // TODO implement
}

/*
 * View3D2000GT::OnRenderView
 */
bool View3D2000GT::OnRenderView(Call& call) {
    // TODO implement
    return true;
}

/* 
 * View3D2000GT::UpdateFreeze
 */
void View3D2000GT::UpdateFreeze(bool freeze) {
    // intentionally empty?
}

/*
 * View3D2000GT::unpackMouseCoordinates
 */
void View3D2000GT::unpackMouseCoordinates(float& x, float& y) {
    // TODO implement
}

/*
 * View3D2000GT::create
 */
bool View3D2000GT::create(void) {
    vislib::graphics::gl::ShaderSource lineVertSrc;
    vislib::graphics::gl::ShaderSource lineFragSrc;
    if (!this->GetCoreInstance()->ShaderSourceFactory().MakeShaderSource("lines::vertex", lineVertSrc)) {
        vislib::sys::Log::DefaultLog.WriteError("Unable to load vertex shader source for bounding box line shader");
    }
    if (!this->GetCoreInstance()->ShaderSourceFactory().MakeShaderSource("lines::fragment", lineFragSrc)) {
        vislib::sys::Log::DefaultLog.WriteError("Unable to load fragment shader source for bounding box line shader");
    }
    try {
        if (!this->lineShader.Create(
                lineVertSrc.Code(), lineVertSrc.Count(), lineFragSrc.Code(), lineFragSrc.Count())) {
            throw vislib::Exception("Shader creation failure", __FILE__, __LINE__);
        }
    } catch (vislib::Exception e) {
        vislib::sys::Log::DefaultLog.WriteError("Unable to create bounding box line shader: %s\n", e.GetMsgA());
        return false;
    }

    // TODO implement

    return true;
}

/*
 * View3D2000GT::release
 */
void View3D2000GT::release(void) {
    this->removeTitleRenderer();
}

/*
 * View3D2000GT::mouseSensitivityChanged
 */
bool View3D2000GT::mouseSensitivityChanged(param::ParamSlot& p) {
    return true;
}

/*
 * View3D2000GT::renderBBox
 */
void View3D2000GT::renderBBox(void) {
    // TODO implement
}

/*
 * View3D2000GT::renderBBoxBackside
 */
void View3D2000GT::renderBBoxBackside(void) {
    // TODO implement
}

/*
 * View3D2000GT::renderBBoxFrontside
 */
void View3D2000GT::renderBBoxFrontside(void) {
    // TODO implement
}

/*
 * View3D2000GT::renderLookAt
 */
void View3D2000GT::renderLookAt(void) {
    // TODO implement
}

/*
 * View3D2000GT::OnGetCamParams
 */
bool View3D2000GT::OnGetCamParams(CallCamParamSync& c) {
    // TODO implement
    return true;
}

/*
 * View3D2000GT::onStoreCamera
 */
bool View3D2000GT::onStoreCamera(param::ParamSlot& p) {
    // TODO implement
    return true;
}

/*
 * View3D2000GT::onRestoreCamera
 */
bool View3D2000GT::onRestoreCamera(param::ParamSlot& p) {
    // TODO implement
    return true;
}

/*
 * View3D2000GT::onResetView
 */
bool View3D2000GT::onResetView(param::ParamSlot& p) {
    this->ResetView();
    return true;
}

/*
 * View3D2000GT::viewKeyPressed
 */
bool View3D2000GT::viewKeyPressed(param::ParamSlot& p) {
    // TODO implement
    return true;
}

/*
 * View3D2000GT::onToggleButton
 */
bool View3D2000GT::onToggleButton(param::ParamSlot& p) {
    // TODO implement
    return true;
}

/*
 * View3D2000GT::renderViewCube
 */
void View3D2000GT::renderViewCube(void) {
    // TODO implement
}
