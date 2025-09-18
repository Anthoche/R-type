/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** MenuScene
*/

#ifndef MENUSCENE_HPP
	#define MENUSCENE_HPP

	#include "../../Engine/Rendering/scene/AScene.hpp"
	#include "../../Engine/Rendering/scene/SceneHandler.hpp"

namespace scene {
	class MenuScene: public AScene {
		public:
			MenuScene(SceneHandler &sceneHandler);
			~MenuScene() override = default;

			// Overriding methods
			void init() override;
			void render() override;
			void handleEvents() override;
			void onClose() override;

		private:
			SceneHandler &_sceneHandler;

	};
}


#endif //MENUSCENE_HPP