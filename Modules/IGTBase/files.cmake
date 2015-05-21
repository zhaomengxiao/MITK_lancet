file(GLOB_RECURSE H_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/include/*")

set(CPP_FILES
  mitkRealTimeClock.cpp
  mitkNavigationData.cpp
  mitkNavigationDataSet.cpp
 mitkStaticIGTHelperFunctions.cpp
 mitkQuaternionAveraging.cpp
)

if(WIN32)
  set(CPP_FILES ${CPP_FILES} mitkWindowsRealTimeClock.cpp)
else()
  set(CPP_FILES ${CPP_FILES} mitkLinuxRealTimeClock.cpp)
endif(WIN32)
