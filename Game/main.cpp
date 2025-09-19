#include "../Engine/Rendering/scene/SceneHandler.hpp"

int main() {
	// Ce main n'est plus utilisé directement
	// Le jeu est maintenant lancé par le client réseau après réception du message GameStart
	SceneHandler handler;
	handler.openMenu();
	return 0;
}
