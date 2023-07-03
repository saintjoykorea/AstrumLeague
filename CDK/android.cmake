set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -frtti -funwind-tables -fno-delete-null-pointer-checks -Wno-error=format-security -Wno-switch -DCDK_ANDROID")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG -Ofast3 -flto")

set(cdk_base_dir ${root_dir}/CDK/Sources)
set(cdk_extension_dir ${root_dir}/CDK/Extensions)

list(APPEND source_dirs
	${cdk_base_dir}
	${cdk_base_dir}/Audio
	${cdk_base_dir}/Collections
	${cdk_base_dir}/Core
	${cdk_base_dir}/Diagnostics
	${cdk_base_dir}/Graphics
	${cdk_base_dir}/Graphics/Animations
	${cdk_base_dir}/Graphics/Animations/Model
	${cdk_base_dir}/Graphics/Animations/Particle
	${cdk_base_dir}/Graphics/Animations/Particle/Shape
	${cdk_base_dir}/Graphics/Animations/Sprite
	${cdk_base_dir}/Graphics/Animations/Trail
	${cdk_base_dir}/Graphics/Animations/Chain
	${cdk_base_dir}/Graphics/Base
	${cdk_base_dir}/Graphics/Camera
	${cdk_base_dir}/Graphics/Collisions
	${cdk_base_dir}/Graphics/Imaging
	${cdk_base_dir}/Graphics/Light
	${cdk_base_dir}/Graphics/Mesh
	${cdk_base_dir}/Graphics/Shader
	${cdk_base_dir}/Graphics/Supports
	${cdk_base_dir}/Graphics/Supports/Shaders
	${cdk_base_dir}/Interface
	${cdk_base_dir}/IO
	${cdk_base_dir}/Locale
	${cdk_base_dir}/Network
	${cdk_base_dir}/Platform
	${cdk_base_dir}/Platform/include
	${cdk_base_dir}/Platform/android
	${cdk_base_dir}/Platform/android/include/libunwind
	${cdk_base_dir}/Platform/android/include/unzip
	${cdk_base_dir}/Platform/android/java
	${cdk_base_dir}/Platform/android/java/src
	${cdk_base_dir}/Security
	${cdk_base_dir}/Utilities
	${cdk_base_dir}/Utilities/include
	${cdk_base_dir}/Utilities/Movements
	${cdk_base_dir}/Utilities/Supports)
	
file(GLOB lib_files ${cdk_base_dir}/Platform/android/lib/${ANDROID_ABI}/*.a)

foreach(lib_file ${lib_files})
	get_filename_component(lib_name ${lib_file} NAME_WE)
	message(STATUS "Import library:" ${lib_file})
	add_library(${lib_name}-lib STATIC IMPORTED)
	set_target_properties(${lib_name}-lib PROPERTIES IMPORTED_LOCATION ${lib_file})
	list(APPEND libraries ${lib_name}-lib)
endforeach()

##############################################################	
# AndroidPermissions
option(UseAndroidPermissions "UseAndroidPermissions" FALSE)
if (UseAndroidPermissions)
message(STATUS "UseAndroidPermissions")
list(APPEND source_dirs
	${cdk_extension_dir}/AndroidPermissions
	${cdk_extension_dir}/AndroidPermissions/android/src)
endif()
##############################################################	
# GooglePlay
option(UseGooglePlay "UseGooglePlay" FALSE)
if (UseGooglePlay)
message(STATUS "UseGooglePlay")
list(APPEND source_dirs
	${cdk_extension_dir}/GooglePlay
	${cdk_extension_dir}/GooglePlay/android/src)
endif ()
##############################################################	
# GoogleIAP
option(UseGoogleIAP "UseGoogleIAP" FALSE)
if (UseGoogleIAP)
message(STATUS "UseGoogleIAP")
list(APPEND source_dirs
	${cdk_extension_dir}/GoogleIAP
	${cdk_extension_dir}/GoogleIAP/android/src)
endif ()
##############################################################	
# GoogleAdMob
option(UseGoogleAdMob "UseGoogleAdMob" FALSE)
if (UseGoogleAdMob)
message(STATUS "UseGoogleAdMob")
list(APPEND source_dirs
	${cdk_extension_dir}/GoogleAdMob
	${cdk_extension_dir}/GoogleAdMob/android/src)
endif ()
##############################################################	
# VungleAd
option(UseVungleAd "UseVungleAd" FALSE)
if (UseVungleAd)
message(STATUS "UseVungleAd")
list(APPEND source_dirs
	${cdk_extension_dir}/VungleAd
	${cdk_extension_dir}/VungleAd/android/src)
endif ()
##############################################################	
# UnityAds
option(UseUnityAds "UseUnityAds" FALSE)
if (UseUnityAds)
message(STATUS "UseUnityAds")
list(APPEND source_dirs
	${cdk_extension_dir}/UnityAds
	${cdk_extension_dir}/UnityAds/android/src)
endif ()
##############################################################	
# Firebase
option(UseFirebase "UseFirebase" FALSE)
if (UseFirebase)
message(STATUS "UseFirebase")
list(APPEND source_dirs
	${cdk_extension_dir}/Firebase
	${cdk_extension_dir}/Firebase/android/src)
endif ()
##############################################################	
# Facebook
option(UseFacebook "UseFacebook" FALSE)
if (UseFacebook)
message(STATUS "UseFacebook")
list(APPEND source_dirs
	${cdk_extension_dir}/Facebook
	${cdk_extension_dir}/Facebook/android/src)
endif()
##############################################################	
# AppsFlyer
option(UseAppsFlyer "UseAppsFlyer" FALSE)
if (UseAppsFlyer)
message(STATUS "UseAppsFlyer")
list(APPEND source_dirs
	${cdk_extension_dir}/AppsFlyer
	${cdk_extension_dir}/AppsFlyer/android/src)
endif ()
##############################################################	
# Flurry
option(UseFlurry "UseFlurry" FALSE)
if (UseFlurry)
message(STATUS "UseFlurry")
list(APPEND source_dirs
	${cdk_extension_dir}/Flurry
	${cdk_extension_dir}/Flurry/android/src)
endif ()
##############################################################	
# NaverLogin
option(UseNaverLogin "UseNaverLogin" FALSE)
if (UseNaverLogin)
message(STATUS "UseNaverLogin")
list(APPEND source_dirs
	${cdk_extension_dir}/NaverLogin
	${cdk_extension_dir}/NaverLogin/android/src)
endif ()
##############################################################	
# NaverCafe
option(UseNaverCafe "UseNaverCafe" FALSE)
if (UseNaverCafe)
message(STATUS "UseNaverCafe")
list(APPEND source_dirs
	${cdk_extension_dir}/NaverCafe
	${cdk_extension_dir}/NaverCafe/android/src)
endif ()
##############################################################	
# LineLogin
option(UseLineLogin "UseLineLogin" FALSE)
if (UseLineLogin)
message(STATUS "UseLineLogin")
list(APPEND source_dirs
	${cdk_extension_dir}/LineLogin
	${cdk_extension_dir}/LineLogin/android/src)
endif ()
##############################################################	
# OneStoreIAP
option(UseOneStoreIAP "UseOneStoreIAP" FALSE)
if (UseOneStoreIAP)
message(STATUS "UseOneStoreIAP")
list(APPEND source_dirs
	${cdk_extension_dir}/OneStoreIAP
	${cdk_extension_dir}/OneStoreIAP/android/src)
endif ()
##############################################################	
# TeamSpeak
option(UseTeamSpeak "UseTeamSpeak" FALSE)
if (UseTeamSpeak)
message(STATUS "UseTeamSpeak")
list(APPEND source_dirs
	${cdk_extension_dir}/TeamSpeak
	${cdk_extension_dir}/TeamSpeak/include
	${cdk_extension_dir}/TeamSpeak/android/src)
	
add_library(ts3client-lib SHARED IMPORTED)
set_target_properties(ts3client-lib PROPERTIES IMPORTED_LOCATION ${cdk_extension_dir}/TeamSpeak/android/lib/${ANDROID_ABI}/libts3client.so)
list(APPEND libraries ts3client-lib)
endif ()
##############################################################	
# TCGME
option(UseTCGME "UseTCGME" FALSE)
if (UseTCGME)
message(STATUS "UseTCGME")
list(APPEND source_dirs
	${cdk_extension_dir}/TCGME
	${cdk_extension_dir}/TCGME/android/src)
endif ()
##############################################################	
# LiApp
option(UseLiApp "UseLiApp" FALSE)
if (UseLiApp)
message(STATUS "UseLiApp")
list(APPEND source_dirs
	${cdk_extension_dir}/LiApp
	${cdk_extension_dir}/LiApp/android/src)
endif ()
##############################################################	
# AppSafer
option(UseAppSafer "UseAppSafer" FALSE)
if (UseAppSafer)
message(STATUS "UseAppSafer")
list(APPEND source_dirs
	${cdk_extension_dir}/AppSafer
	${cdk_extension_dir}/AppSafer/android/src)
endif ()
##############################################################	
# ScreenRecord
option(UseScreenRecord "UseScreenRecord" FALSE)
if (UseScreenRecord)
message(STATUS "UseScreenRecord")
list(APPEND source_dirs
	${cdk_extension_dir}/ScreenRecord
	${cdk_extension_dir}/ScreenRecord/android/src)
endif ()
##############################################################	
# ZarinPalIAP
option(UseZarinPalIAP "UseZarinPalIAP" FALSE)
if (UseZarinPalIAP)
message(STATUS "UseZarinPalIAP")
list(APPEND source_dirs
	${cdk_extension_dir}/ZarinPalIAP
	${cdk_extension_dir}/ZarinPalIAP/android/src)
endif ()
##############################################################	
# MyketIAP
option(UseMyketIAP "UseMyketIAP" FALSE)
if (UseMyketIAP)
message(STATUS "UseMyketIAP")
list(APPEND source_dirs
	${cdk_extension_dir}/MyketIAP
	${cdk_extension_dir}/MyketIAP/android/src)
endif ()
##############################################################	
# BazaarIAP
option(UseBazaarIAP "UseBazaarIAP" FALSE)
if (UseBazaarIAP)
message(STATUS "UseBazaarIAP")
list(APPEND source_dirs
	${cdk_extension_dir}/BazaarIAP
	${cdk_extension_dir}/BazaarIAP/android/src)
endif ()
##############################################################	
# HuaweiIAP
option(UseHuaweiIAP "UseHuaweiIAP" FALSE)
if (UseHuaweiIAP)
message(STATUS "UseHuaweiIAP")
list(APPEND source_dirs
	${cdk_extension_dir}/HuaweiIAP
	${cdk_extension_dir}/HuaweiIAP/android/src)
endif ()
##############################################################
# HuaweiLogin
option(UseHuaweiLogin "UseHuaweiLogin" FALSE)
if (UseHuaweiLogin)
message(STATUS "UseHuaweiLogin")
list(APPEND source_dirs
	${cdk_extension_dir}/HuaweiLogin
	${cdk_extension_dir}/HuaweiLogin/android/src)
endif ()
##############################################################
# HuaweiAds
option(UseHuaweiAds "UseHuaweiAds" FALSE)
if (UseHuaweiAds)
message(STATUS "UseHuaweiAds")
list(APPEND source_dirs
	${cdk_extension_dir}/HuaweiAds
	${cdk_extension_dir}/HuaweiAds/android/src)
endif ()
##############################################################
# HuaweiAnalytics
option(UseHuaweiAnalytics "UseHuaweiAnalytics" FALSE)
if (UseHuaweiAnalytics)
message(STATUS "UseHuaweiAnalytics")
list(APPEND source_dirs
	${cdk_extension_dir}/HuaweiAnalytics
	${cdk_extension_dir}/HuaweiAnalytics/android/src)
endif ()
##############################################################
foreach(source_dir ${source_dirs})
	file(GLOB source_files ${source_dir}/*.cpp ${source_dir}/*.c ${source_dir}/*.hpp ${source_dir}/*.h)
	list(APPEND sources ${source_files})
endforeach()

add_library(cdkapp SHARED ${sources})
target_include_directories(cdkapp PRIVATE ${source_dirs})

find_library(android-lib android)
find_library(EGL-lib EGL)
find_library(GLESv3-lib GLESv3)
find_library(dl-lib dl)
find_library(log-lib log)
find_library(z-lib z)
find_library(jnigraphics-lib jnigraphics)

target_link_libraries(cdkapp
					  ${android-lib}
					  ${EGL-lib}
					  ${GLESv3-lib}
					  ${dl-lib}
					  ${log-lib}
					  ${z-lib}
					  ${jnigraphics-lib}
					  ${libraries})

