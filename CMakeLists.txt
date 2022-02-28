CMAKE_MINIMUM_REQUIRED ( VERSION 3.12 )

PROJECT (Arachne LANGUAGES CXX C)

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-conversion-null")
ELSEIF(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-register")
ELSEIF(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
ELSEIF(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
ENDIF()

########################### SOURCE CODE ########################################
SET ( ARACHNE_SOURCE
    Source/main.cpp
    Source/Scheduler.cpp
  )

SET ( ARACHNE_HEADERS
    Source/Scheduler.h
    Source/Context.h
    Source/Fiber.h
    Source/Console.h
  )

############################# DEPENDENCIES ###########################################

ADD_EXECUTABLE ( Arachne
  ${ARACHNE_SOURCE}
  ${ARACHNE_HEADERS}
  )


INSTALL ( TARGETS
    Arachne
    DESTINATION
    bin
  )