# Appoint the version of cmake
cmake_minimum_required(VERSION 3.0)

# Appoint the name of project
PROJECT(coinexTool)

# support C++ 11
add_compile_options(-std=c++11)

# Appoint the head file path
INCLUDE_DIRECTORIES(
include
IF (WIN32)  
	include/libevent_win
ELSEIF (UNIX)  
	include/libevent
ENDIF ()  
)

# Appoint the source directory
AUX_SOURCE_DIRECTORY(src DIR_SRCS)
AUX_SOURCE_DIRECTORY(include/LeftMyCodes Left_MyCodes)
AUX_SOURCE_DIRECTORY(include/CompleteConfidence Left_CC)
AUX_SOURCE_DIRECTORY(include/easylogging Git_EL)
AUX_SOURCE_DIRECTORY(include/nlohmann JSON)
IF (WIN32)  
	AUX_SOURCE_DIRECTORY(include/libevent_win DIR_LE)
ELSEIF (UNIX)  
	AUX_SOURCE_DIRECTORY(include/libevent DIR_LE)
ENDIF ()  

# Set environement variable
SET(_MATH
${DIR_SRCS}
${Left_MyCodes}
${Left_CC}
${Git_EL}
${JSON}
# ${DIR_LE}
)

# Add excutable file
ADD_EXECUTABLE(${PROJECT_NAME} ${_MATH})

# Add link library
IF (WIN32)  
	TARGET_LINK_LIBRARIES(${PROJECT_NAME}
	../include/libevent
	../include/libcurl_imp)
ELSEIF (UNIX)  
	TARGET_LINK_LIBRARIES(${PROJECT_NAME} m pthread event curl)
ENDIF ()  
