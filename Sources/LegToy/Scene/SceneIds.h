////////////////////////////////////////////////////////////////////////////////
// Scene/SceneIds.h (LegToy - LegToy)
//
// LegToy Project Scene Id Definitions
////////////////////////////////////////////////////////////////////////////////

#ifndef __LEGTOY__SCENE__SCENE_IDS_H
#define __LEGTOY__SCENE__SCENE_IDS_H


// Leggiero.Engine
#include <Engine/Toolbox/Scene/GameSceneTypes.h>


namespace LegToy
{
	class SceneIds
	{
	public:
		using IdType = Leggiero::Toolbox::Scene::GameSceneIdType;

		static constexpr IdType kINVALID = Leggiero::Toolbox::Scene::kInvalidSceneId;

		static constexpr IdType kInitialLoading = 1;
		static constexpr IdType kShowcase = 2;
	};
}

#endif
