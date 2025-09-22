/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** client
*/

#include "Include/client.hpp"
#include <tuple>

#include "Game.hpp"


GameClient::GameClient(Game &game, const std::string &serverIp, uint16_t serverPort, const std::string &name)
	: clientName(name), _game(game) {
	socketFd = socket(AF_INET, SOCK_DGRAM, 0);

	if (socketFd < 0) {
		throw std::runtime_error("Failed to create socket");
	}
	serverAddr = {};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(serverPort);
	if (inet_pton(AF_INET, serverIp.c_str(), &serverAddr.sin_addr) <= 0) {
		throw std::runtime_error("Invalid server IP");
	}
}

GameClient::~GameClient() {
	running = false;
	if (rxThread.joinable()) rxThread.join();
	close(socketFd);
}

void GameClient::run() {
	std::cout << "Client démarré. En attente du début du jeu..." << std::endl;
	running = true;
	rxThread = std::thread(&GameClient::recvLoop, this);

	while (_game.getGameStatus() != GameStatus::RUNNING) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	std::cout << "Le jeu commence, ouverture de la fenêtre client..." << std::endl;
}

void GameClient::sendHello() {
	ClientHelloMessage msg;
	msg.type = MessageType::ClientHello;
	msg.clientId = htonl(0);
	strncpy(msg.clientName, clientName.c_str(), sizeof(msg.clientName) - 1);
	msg.clientName[sizeof(msg.clientName) - 1] = '\0';
	ssize_t sentBytes = sendto(
		socketFd, &msg, sizeof(msg), 0,
		(struct sockaddr *) &serverAddr, sizeof(serverAddr)
	);

	if (sentBytes < 0) {
		std::cerr << "[ERREUR] Échec de l'envoi de ClientHello" << std::endl;
	}
}


void GameClient::handleMessage(MessageType type, const std::vector<uint8_t> &buffer) {
	switch (type) {
		case MessageType::ServerAssignId:
			handleServerAssignId(buffer);
			break;
		case MessageType::GameStart:
			handleGameStart(buffer);
			break;
		case MessageType::StateUpdate:
			handlePlayerUpdate(buffer);
			break;
		case MessageType::EnemySpawn:
			handleEnemySpawn(buffer);
			break;
		case MessageType::EnemyUpdate:
			handleEnemyUpdate(buffer);
			break;
		case MessageType::EnemyDespawn:
			handleEnemyDespawn(buffer);
			break;
		case MessageType::ObstacleSpawn:
			handleObstacleSpawn(buffer);
			break;
		case MessageType::ObstacleDespawn:
			handleObstacleDespawn(buffer);
			break;
		default:
			break;
	}
}

void GameClient::recvLoop() {
	while (running) {
		std::vector<uint8_t> buffer(1024);
		sockaddr_in fromAddr = {};
		socklen_t fromAddrLen = sizeof(fromAddr);
		ssize_t bytesReceived = recvfrom(
			socketFd, buffer.data(), buffer.size(), MSG_DONTWAIT,
			(struct sockaddr *) &fromAddr, &fromAddrLen
		);
		if (bytesReceived <= 0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(2));
			continue;
		}
		if (bytesReceived < (ssize_t) sizeof(MessageType)) {
			continue;
		}
		MessageType type = *reinterpret_cast<MessageType *>(buffer.data());
		handleMessage(type, buffer);
	}
}

// --- Gestion des messages ---
void GameClient::handleServerAssignId(const std::vector<uint8_t> &buffer) {
	if (buffer.size() < sizeof(ServerAssignIdMessage)) return;
	const ServerAssignIdMessage *msg = reinterpret_cast<const ServerAssignIdMessage *>(buffer.data());
	clientId = ntohl(msg->clientId);
	std::cout << "[Client] Reçu clientId=" << clientId << std::endl;
}

void GameClient::handleGameStart(const std::vector<uint8_t> &buffer) {
	if (buffer.size() < sizeof(GameStartMessage)) return;
	const GameStartMessage *msg = reinterpret_cast<const GameStartMessage *>(buffer.data());
	std::cout << "Le jeu commence ! Nombre de joueurs : " << ntohl(msg->clientCount) << std::endl;
	_game.setGameStatus(GameStatus::RUNNING);
}

void GameClient::handlePlayerUpdate(const std::vector<uint8_t> &buffer) {
	if (buffer.size() < sizeof(StateUpdateMessage)) return;
	const StateUpdateMessage *msg = reinterpret_cast<const StateUpdateMessage *>(buffer.data());
	uint32_t id = ntohl(msg->clientId);
	uint32_t xb = ntohl(msg->posXBits), yb = ntohl(msg->posYBits);
	float x, y;
	std::memcpy(&x, &xb, sizeof(float));
	std::memcpy(&y, &yb, sizeof(float));
	std::lock_guard<std::mutex> g(stateMutex);
	players[id] = {x, y};
}

// --- Gestion des ennemis ---
void GameClient::handleEnemySpawn(const std::vector<uint8_t> &buffer) {
	if (buffer.size() < sizeof(EnemySpawnMessage)) return;
	const EnemySpawnMessage *msg = reinterpret_cast<const EnemySpawnMessage *>(buffer.data());
	uint32_t id = ntohl(msg->enemyId);
	uint32_t xb = ntohl(msg->posXBits), yb = ntohl(msg->posYBits);
	float x, y;
	std::memcpy(&x, &xb, sizeof(float));
	std::memcpy(&y, &yb, sizeof(float));
	std::lock_guard<std::mutex> g(stateMutex);
	enemies[id] = {x, y};
}

void GameClient::handleEnemyUpdate(const std::vector<uint8_t> &buffer) {
	if (buffer.size() < sizeof(EnemyUpdateMessage)) return;
	const EnemyUpdateMessage *msg = reinterpret_cast<const EnemyUpdateMessage *>(buffer.data());
	uint32_t id = ntohl(msg->enemyId);
	uint32_t xb = ntohl(msg->posXBits), yb = ntohl(msg->posYBits);
	float x, y;
	std::memcpy(&x, &xb, sizeof(float));
	std::memcpy(&y, &yb, sizeof(float));
	std::lock_guard<std::mutex> g(stateMutex);
	enemies[id] = {x, y};
}

void GameClient::handleEnemyDespawn(const std::vector<uint8_t> &buffer) {
	if (buffer.size() < sizeof(EnemyDespawnMessage)) return;
	const EnemyDespawnMessage *msg = reinterpret_cast<const EnemyDespawnMessage *>(buffer.data());
	uint32_t id = ntohl(msg->enemyId);
	std::lock_guard<std::mutex> g(stateMutex);
	enemies.erase(id);
}

// --- Gestion des obstacles ---
void GameClient::handleObstacleSpawn(const std::vector<uint8_t> &buffer) {
	if (buffer.size() < sizeof(ObstacleSpawnMessage)) return;
	const ObstacleSpawnMessage *msg = reinterpret_cast<const ObstacleSpawnMessage *>(buffer.data());
	uint32_t id = ntohl(msg->obstacleId);
	uint32_t xb = ntohl(msg->posXBits), yb = ntohl(msg->posYBits);
	uint32_t wb = ntohl(msg->widthBits), hb = ntohl(msg->heightBits);
	float x, y, w, h;
	std::memcpy(&x, &xb, 4);
	std::memcpy(&y, &yb, 4);
	std::memcpy(&w, &wb, 4);
	std::memcpy(&h, &hb, 4);
	std::lock_guard<std::mutex> g(stateMutex);
	obstacles[id] = std::make_tuple(x, y, w, h);
}

void GameClient::handleObstacleDespawn(const std::vector<uint8_t> &buffer) {
	if (buffer.size() < sizeof(ObstacleDespawnMessage)) return;
	const ObstacleDespawnMessage *msg = reinterpret_cast<const ObstacleDespawnMessage *>(buffer.data());
	uint32_t id = ntohl(msg->obstacleId);
	std::lock_guard<std::mutex> g(stateMutex);
	obstacles.erase(id);
}


void GameClient::sendInput(float inputX, float inputY) {
	ClientInputMessage m{};
	m.type = MessageType::ClientInput;
	m.clientId = htonl(clientId);
	uint32_t xbits, ybits;
	std::memcpy(&xbits, &inputX, sizeof(float));
	std::memcpy(&ybits, &inputY, sizeof(float));
	m.inputXBits = htonl(xbits);
	m.inputYBits = htonl(ybits);
	sendto(
		socketFd, &m, sizeof(m), 0,
		(struct sockaddr *) &serverAddr, sizeof(serverAddr)
	);
}

void GameClient::runRenderLoop() {
	/*raylib.setResizableFlag(FLAG_WINDOW_RESIZABLE);
	raylib.initWindow(800, 600, "R-Type Client");
	raylib.enableCursor();
	raylib.setTargetFPS(60);
	while (!raylib.windowShouldClose()) {
		float ix = 0.f, iy = 0.f;
		if (raylib.isKeyDown(KEY_A) || raylib.isKeyDown(KEY_LEFT)) ix -= 1.f;
		if (raylib.isKeyDown(KEY_D) || raylib.isKeyDown(KEY_RIGHT)) ix += 1.f;
		if (raylib.isKeyDown(KEY_W) || raylib.isKeyDown(KEY_UP)) iy -= 1.f;
		if (raylib.isKeyDown(KEY_S) || raylib.isKeyDown(KEY_DOWN)) iy += 1.f;
		sendInput(ix, iy);

		std::unordered_map<uint32_t, std::pair<float, float> > pls;
		std::unordered_map<uint32_t, std::pair<float, float> > ens;
		std::unordered_map<uint32_t, std::tuple<float, float, float, float> > obs; {
			std::lock_guard<std::mutex> g(stateMutex);
			pls = players;
			ens = enemies;
			obs = obstacles;
		}
		raylib.beginDrawing();
		raylib.clearBackground(BLACK);
		// Obstacles from server
		for (auto const &kv: obs) {
			float x = std::get<0>(kv.second);
			float y = std::get<1>(kv.second);
			float w = std::get<2>(kv.second);
			float h = std::get<3>(kv.second);
			raylib.drawRectangle((int) (x - w / 2), (int) (y - h / 2), (int) w, (int) h, GRAY);
		}
		// Players with per-id color
		auto colorForId = [](uint32_t id) -> Color {
			static Color palette[] = {RAYWHITE, BLUE, GREEN, YELLOW, ORANGE, PURPLE, PINK, GOLD, LIME, SKYBLUE};
			return palette[id % (sizeof(palette) / sizeof(palette[0]))];
		};
		for (auto const &kv: pls) {
			raylib.drawRectangle((int) (kv.second.first - 15), (int) (kv.second.second - 15), 30, 30, colorForId(kv.first));
		}
		for (auto const &kv: ens) {
			raylib.drawRectangle((int) (kv.second.first - 20), (int) (kv.second.second - 20), 40, 40, RED);
		}
		raylib.endDrawing();
	}
	raylib.closeWindow();
	running = false;*/
}
