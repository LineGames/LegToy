cmake_minimum_required(VERSION 3.18.1)

include("../../../Engine/Platform/Platform.Android/cmake/AndroidCommon.cmake")


add_library(LegToy_Game STATIC)
le_tgt_setCppOptions(LegToy_Game)


target_include_directories(LegToy_Game PUBLIC .)

target_link_libraries(LegToy_Game
    PRIVATE
        Leggiero
)


target_sources(LegToy_Game
    PUBLIC
        Game.h
        
    PRIVATE
        Game.cpp CreateGame.cpp GameModules.cpp
        Common/SharedResources.h Common/SharedResources.cpp Common/IResource_CommonRendering.h Common/IResource_UI.h Common/BackgroundRenderer.h Common/BackgroundRenderer.cpp
        Scene/SceneIds.h Scene/Game_Scenes.cpp
        
        Scene/InitialLoadingScene/InitialLoadingScene.h Scene/InitialLoadingScene/InitialLoadingScene.cpp Scene/InitialLoadingScene/LoadingMarker.h Scene/InitialLoadingScene/LoadingMarker.cpp
        
        Scene/ShowcaseScene/ShowcaseScene.h Scene/ShowcaseScene/ShowcaseScene.cpp
        Scene/ShowcaseScene/CubeSpace.h Scene/ShowcaseScene/CubeSpace.cpp
        Scene/ShowcaseScene/ShowcaseLogicCenter.h Scene/ShowcaseScene/ShowcaseLogicCenter.cpp Scene/ShowcaseScene/ShowcaseLoadingTasks.h Scene/ShowcaseScene/ShowcaseLoadingTasks.cpp
        Scene/ShowcaseScene/ShowcaseControllerBase.h Scene/ShowcaseScene/ShowcaseControllerBase.cpp Scene/ShowcaseScene/ShowcaseMainMenuController.h Scene/ShowcaseScene/ShowcaseMainMenuController.cpp
        Scene/ShowcaseScene/ShowcaseUIExampleController.h Scene/ShowcaseScene/ShowcaseUIExampleController.cpp Scene/ShowcaseScene/ShowcaseSoundEffectController.h Scene/ShowcaseScene/ShowcaseSoundEffectController.cpp Scene/ShowcaseScene/ShowcaseBGMController.h Scene/ShowcaseScene/ShowcaseBGMController.cpp Scene/ShowcaseScene/ShowcaseTypesetController.h Scene/ShowcaseScene/ShowcaseTypesetController.cpp Scene/ShowcaseScene/ShowcaseHashController.h Scene/ShowcaseScene/ShowcaseHashController.cpp Scene/ShowcaseScene/ShowcaseWebViewController.h Scene/ShowcaseScene/ShowcaseWebViewController.cpp
)
