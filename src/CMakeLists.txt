# At some point I should remove that second directory and just prepend all includes with "nta/"
include_directories(${NTA_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../include/nta)

add_library(${NTA_LIBRARIES}
  ${CMAKE_CURRENT_SOURCE_DIR}/AudioManager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Camera2D.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/FontMap.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/FPSLimiter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/GLMConsoleOutput.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/GLSLProgram.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/GLTexture.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/InputManager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/IOManager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Logger.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/nta.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ParticleBatch2D.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ParticleEngine2D.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/PrimitiveBatch.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Random.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ResourceManager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Screen.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ScreenManager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/SpriteBatch.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Sprite.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/SpriteFont.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/SystemManager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Timer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Window.cpp
)
