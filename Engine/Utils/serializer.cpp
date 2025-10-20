/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** serializer
*/

/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** serializer
*/

#include "Include/serializer.hpp"
#include "../../Core/Entities/Include/components.hpp"
#include <iostream>

namespace game::serializer {
	nlohmann::json serialize_entity(ecs::registry &reg, ecs::entity_t entity) {
		nlohmann::json j;
		j["entity_id"] = static_cast<std::size_t>(entity);

		if (auto type = reg.get_components<component::type>()[static_cast<std::size_t>(entity)]) {
			j["type"] = static_cast<int>(type->value);
		} else {
			return nlohmann::json();
		}

		if (auto pos = reg.get_components<component::position>()[static_cast<std::size_t>(entity)]) {
			j["x"] = pos->x;
			j["y"] = pos->y;
		}

		if (auto cid = reg.get_components<component::client_id>()[static_cast<std::size_t>(entity)]) {
			j["client_id"] = cid->id;
		}

		if (auto vel = reg.get_components<component::velocity>()[static_cast<std::size_t>(entity)]) {
			j["vx"] = vel->vx;
			j["vy"] = vel->vy;
		}

		if (auto health = reg.get_components<component::health>()[static_cast<std::size_t>(entity)]) {
			j["health_current"] = health->current;
			j["health_max"] = health->max;
		}

		if (auto sprite = reg.get_components<component::sprite>()[static_cast<std::size_t>(entity)]) {
			j["image_path"] = sprite->image_path;
		}

		if (auto drawable = reg.get_components<component::drawable>()[static_cast<std::size_t>(entity)]) {
			j["width"] = drawable->width;
			j["height"] = drawable->height;
		}

		if (auto cbox = reg.get_components<component::collision_box>()[static_cast<std::size_t>(entity)]) {
			j["collision_width"] = cbox->width;
			j["collision_height"] = cbox->height;
		}
		return j;
	}


	void deserialize_entities(ecs::registry &reg, const nlohmann::json &j) {
		try {
			if (!j.is_array() && !j.contains("entities")) {
				throw std::runtime_error("deserialize_entities: expected JSON array or 'entities' field");
			}

			const auto &entities_json = j.contains("entities") ? j["entities"] : j;

			for (const auto &entity_data: entities_json) {
				if (!entity_data.contains("type")) {
					std::cerr << "[WARNING] Entity without type field, skipping\n";
					continue;
				}

				int type_value = entity_data["type"];
				component::entity_type type_enum = static_cast<component::entity_type>(type_value);

				switch (type_enum) {
					case component::entity_type::PLAYER:
						game::parsing::parse_player(reg, entity_data);
						break;
					case component::entity_type::ENEMY:
						game::parsing::parse_enemy(reg, entity_data);
						break;
					case component::entity_type::BACKGROUND:
						game::parsing::parse_background(reg, entity_data);
						break;
					case component::entity_type::OBSTACLE:
						game::parsing::parse_obstacle(reg, entity_data);
						break;
					case component::entity_type::RANDOM_ELEMENT:
						game::parsing::parse_random_element(reg, entity_data);
						break;
					case component::entity_type::SOUND:
						game::parsing::parse_sound(reg, entity_data);
						break;
					case component::entity_type::TEXT:
						game::parsing::parse_text(reg, entity_data);
						break;
					default:
						std::cerr << "[WARNING] Unknown entity type: " << type_value << std::endl;
						continue;
				}
			}
		} catch (const std::exception &e) {
			std::cerr << "[ERROR] Failed to deserialize entities: " << e.what() << std::endl;
		}
	}

	nlohmann::json serialize_rooms(std::map<int, RoomData> const &rooms) {
		nlohmann::json json;

		json["rooms"] = nlohmann::json::array();
		auto &root = json["rooms"];
		for (auto &room: rooms) {
			nlohmann::json r = nlohmann::json::object();

			r["id"] = room.first;
			r["gameName"] = room.second.gameName;
			r["currentPlayers"] = room.second.currentPlayers;
			r["minPlayers"] = room.second.minPlayers;
			r["maxPlayers"] = room.second.maxPlayers;
			r["status"] = room.second.status;
			root.push_back(r);
		}
		return json;
	}

	std::map<int, RoomData> deserialize_rooms(nlohmann::json const &json) {
		std::map<int, RoomData> rooms;
		auto JsonRooms = json["rooms"];

		for (auto &room: JsonRooms) {
			RoomData r{};
			int id = room["id"];

			r.gameName = room["gameName"];
			r.currentPlayers = room["currentPlayers"];
			r.minPlayers = room["minPlayers"];
			r.maxPlayers = room["maxPlayers"];
			r.status = static_cast<GameStatus>(room["status"]);
			rooms.insert_or_assign(id, r);
		}
		return rooms;
	}
} // namespace game::serializer
