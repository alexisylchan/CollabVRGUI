# Create an auto-start plugin. Auto start plugins provide callbacks that get
# called when the plugin is loaded and when the application shutsdown.

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

# Find and Include ParaView
if(NOT ParaView_SOURCE_DIR)
  FIND_PACKAGE(ParaView REQUIRED)
  INCLUDE(${PARAVIEW_USE_FILE})
endif()

# Checks to see if we want to use VRPN or VRUI.
if(APPLE OR UNIX) #  APPLE and UNIX will use VRUI by default
  SET(PARAVIEW_USE_VRPN OFF CACHE BOOL "Build ParaView with VRPN support")
  SET(PARAVIEW_USE_VRUI ON CACHE BOOL "Build ParaView with VRUI support")
else() # Windows will use VRPN by default
  SET(PARAVIEW_USE_VRPN ON CACHE BOOL "Build ParaView with VRPN support")
  SET(PARAVIEW_USE_VRUI OFF CACHE BOOL "Build ParaView with VRUI support")
endif()

# if VRPN
mark_as_advanced(VRPN_INCLUDE_DIR)
mark_as_advanced(VRPN_LIBRARY)
if(PARAVIEW_USE_VRPN)
  if(NOT EXISTS VRPN_INCLUDE_DIR)
    find_path(VRPN_INCLUDE_DIR vrpn_Tracker.h)
  endif()
  find_library(VRPN_LIBRARY vrpn)
  set(VRPN_MOC_SRC vtkVRPNConnection.h)
  set(VRPN_SOURCE_FILES
    vtkVRPNConnection.cxx
    vtkVRPNCallBackHandlers.cxx)
endif(PARAVIEW_USE_VRPN)

# if VRUI
if(PARAVIEW_USE_VRUI)
  set(VRUI_MOC_SRC vtkVRUIConnection.h)
  set(VRUI_SOURCE_FILES
    vtkVRUIConnection.cxx
    vtkVRUIPipe.cxx
    vtkVRUIServerState.cxx
    vtkVRUITrackerState.cxx)
endif(PARAVIEW_USE_VRUI)

# if BUILD_QT_GUI
if (PARAVIEW_BUILD_QT_GUI)
  set(VR_PANEL_MOC_SRC pqVRConfigPanel.h)
  set(VR_PANEL_SOURCE_FILES pqVRConfigPanel.cxx)
  set(VR_PANEL_UI Resources/pqVRConfigPanel.ui)
endif(PARAVIEW_BUILD_QT_GUI)

QT4_WRAP_CPP(MOC_SRCS
  pqVRStarter.h
  vtkVRConnectionManager.h
  ${VRPN_MOC_SRC}
  ${VRUI_MOC_SRC}
  ${VR_PANEL_MOC_SRC}
  vtkVRQueue.h
  vtkVRQueueHandler.h
  vtkVRInteractorStyle.h
  vtkVRStyleTracking.h
  vtkVRStylePhantomPick.h
  vtkVRStyleGrabNUpdateMatrix.h
  vtkVRStyleGrabNRotateSliceNormal.h
  vtkVRStyleGrabNTranslateSliceOrigin.h
  vtkVRActiveObjectManipulationStyle.h
  )

if (PARAVIEW_BUILD_QT_GUI)
  QT4_WRAP_UI(UI_BUILT_SOURCES
      ${VR_PANEL_UI})   
  QT4_ADD_RESOURCES(RCS_SRCS Resources/pqVRConfigPanel.qrc) 
endif (PARAVIEW_BUILD_QT_GUI)


SET (QT_USE_QTNETWORK TRUE)
INCLUDE(${QT_USE_FILE})

INCLUDE_DIRECTORIES(
  ${VTK_INCLUDE_DIR}
  ${PARAVIEW_INCLUDE_DIRS}
  ${PARAVIEW_GUI_INCLUDE_DIRS}
  ${PARAVIEW_KWSYS_INCLUDE_DIRS}
  ${pqCore_SOURCE_DIR}
  ${pqCore_BINARY_DIR}
  ${pqComponents_SOURCE_DIR}
  ${pqComponents_BINARY_DIR}
  )

if(PARAVIEW_USE_VRPN)
  INCLUDE_DIRECTORIES(${VRPN_INCLUDE_DIR})
endif(PARAVIEW_USE_VRPN)

SET( VR_SOURCE_FILES
  vtkVRConnectionManager.cxx
  ${VRPN_SOURCE_FILES}
  ${VRUI_SOURCE_FILES}
  vtkVRQueue.cxx
  vtkVRQueueHandler.cxx
  vtkVRInteractorStyle.cxx
  vtkVRStyleTracking.cxx
  vtkVRStylePhantomPick.cxx
  vtkVRStyleGrabNUpdateMatrix.cxx
  vtkVRStyleGrabNRotateSliceNormal.cxx
  vtkVRStyleGrabNTranslateSliceOrigin.cxx
  vtkVRActiveObjectManipulationStyle.cxx
  ${MOC_SRCS}
  ${VR_PANEL_SOURCE_FILES})

if (PARAVIEW_BUILD_QT_GUI)
 
 SET(IFACES pqVRStarter)
 SET(ARG_CLASS_NAME pqVRStarter)
 SET(ARG_STARTUP onStartup)
 SET(ARG_SHUTDOWN onShutdown)
 SET(ARG_DOCK_AREA Bottom)
 SET(ARG_DOCK_CLASS_NAME pqVRConfigPanel)
 CONFIGURE_FILE (pqAutoStartAndDockWindowImplementation.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/${ARG_CLASS_NAME}Implementation.h @ONLY)
 CONFIGURE_FILE (pqAutoStartAndDockWindowImplementation.cxx.in
  ${CMAKE_CURRENT_BINARY_DIR}/${ARG_CLASS_NAME}Implementation.cxx @ONLY)

GET_DIRECTORY_PROPERTY(include_dirs_tmp INCLUDE_DIRECTORIES)

 SET_DIRECTORY_PROPERTIES(PROPERTIES INCLUDE_DIRECTORIES "${QT_INCLUDE_DIRS};${PARAVIEW_GUI_INCLUDE_DIRS}")

 SET(ACTION_MOC_SRCS)
 QT4_WRAP_CPP(ACTION_MOC_SRCS ${CMAKE_CURRENT_BINARY_DIR}/${ARG_CLASS_NAME}Implementation.h)
 SET_DIRECTORY_PROPERTIES(PROPERTIES INCLUDE_DIRECTORIES "${include_dirs_tmp}")

SET(IFACE_SRCS ${CMAKE_CURRENT_BINARY_DIR}/${ARG_CLASS_NAME}Implementation.cxx ${CMAKE_CURRENT_BINARY_DIR}/${ARG_CLASS_NAME}Implementation.h ${ACTION_MOC_SRCS})
 
else()
  ADD_PARAVIEW_AUTO_START(
    IFACES IFACE_SRCS CLASS_NAME pqVRStarter
                        STARTUP onStartup
                        SHUTDOWN onShutdown)

endif (PARAVIEW_BUILD_QT_GUI)
# create a plugin for this starter
ADD_PARAVIEW_PLUGIN(
  VRPlugin "1.0"
  GUI_INTERFACES ${IFACES}
   SOURCES pqVRStarter.cxx
          ${MOC_SRCS}
          ${VR_SOURCE_FILES}
          ${RCS_SRCS} ${IFACE_SRCS} 
  GUI_SOURCES ${UI_BUILT_SOURCES})

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/vtkPVVRConfig.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/vtkPVVRConfig.h
  ESCAPE_QUOTES IMMEDIATE)

# if VRPN is used then link to VRPN library
if(PARAVIEW_USE_VRPN)
  TARGET_LINK_LIBRARIES (VRPlugin "${VRPN_LIBRARY}")
endif(PARAVIEW_USE_VRPN)
