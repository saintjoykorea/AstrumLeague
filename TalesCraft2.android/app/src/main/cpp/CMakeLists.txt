cmake_minimum_required(VERSION 3.4.1)

set(root_dir ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../)
set(project_dir ${root_dir}/TalesCraft2/Sources)

list(APPEND source_dirs
	${project_dir}
	${project_dir}/Connection
	${project_dir}/Data
	${project_dir}/Data/Game
	${project_dir}/Data/Game/Terrain
	${project_dir}/Data/Menu
	${project_dir}/Element
	${project_dir}/Element/Common
	${project_dir}/Element/Game
	${project_dir}/Element/Game/Effect
	${project_dir}/Element/Game/Force
	${project_dir}/Element/Game/Map
	${project_dir}/Element/Game/Map/Element
	${project_dir}/Element/Game/Object
	${project_dir}/Element/Game/Terrain
	${project_dir}/Element/Menu
	${project_dir}/Element/Menu/Deck
	${project_dir}/Element/Menu/Map
	${project_dir}/Element/Menu/Match
	${project_dir}/Element/Menu/Unit
	${project_dir}/Element/Menu/User
	${project_dir}/Interface
	${project_dir}/Interface/Common
	${project_dir}/Interface/Game
	${project_dir}/Interface/Menu)

include(${root_dir}/CDK/android.cmake)
include(ndk-stl-config.cmake)
