/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** serializer (complete ECS version)
*/

#include "Include/serializer.hpp"
#include "../../Core/Entities/Include/components.hpp"
#include "Include/entity_parser.hpp"
#include <iostream>

namespace game::serializer {
	// =========================
	// SERIALIZATION
	// =========================
	nlohmann::json serialize_entity(ecs::registry &reg, ecs::entity_t entity) {
		nlohmann::json j;
		j["entity_id"] = static_cast<std::size_t>(entity);

		// Type
		if (auto type = reg.get_components<component::type>()[static_cast<std::size_t>(entity)]) {
			j["type"] = static_cast<int>(type->value);
		} else {
			return nlohmann::json(); // skip entities with no type
		}

		// Position
		if (auto pos = reg.get_components<component::position>()[static_cast<std::size_t>(entity)]) {
			j["x"] = pos->x;
			j["y"] = pos->y;
			j["z"] = pos->z;
		}

		// Previous position
		if (auto prev = reg.get_components<component::previous_position>()[static_cast<std::size_t>(entity)]) {
			j["prev_x"] = prev->x;
			j["prev_y"] = prev->y;
			j["prev_z"] = prev->z;
		}

		// Velocity
		if (auto vel = reg.get_components<component::velocity>()[static_cast<std::size_t>(entity)]) {
			j["vx"] = vel->vx;
			j["vy"] = vel->vy;
			j["vz"] = vel->vz;
		}

		// Rotation
		if (auto rot = reg.get_components<component::rotation>()[static_cast<std::size_t>(entity)]) {
			j["pitch"] = rot->pitch;
			j["yaw"] = rot->yaw;
			j["roll"] = rot->roll;
		}

		// Scale
		if (auto scale = reg.get_components<component::scale>()[static_cast<std::size_t>(entity)]) {
			j["scale_x"] = scale->x;
			j["scale_y"] = scale->y;
			j["scale_z"] = scale->z;
		}

		// Drawable
		if (auto draw = reg.get_components<component::drawable>()[static_cast<std::size_t>(entity)]) {
			j["width"] = draw->width;
			j["height"] = draw->height;
			j["depth"] = draw->depth;
			j["color"] = {draw->color.r, draw->color.g, draw->color.b, draw->color.a};
		}

		// Sprite
		if (auto sprite = reg.get_components<component::sprite>()[static_cast<std::size_t>(entity)]) {
			j["image_path"] = sprite->image_path;
			j["sprite_scale"] = sprite->scale;
			j["sprite_rotation"] = sprite->rotation;
		}

		// 3D model
		if (auto model = reg.get_components<component::model3D>()[static_cast<std::size_t>(entity)]) {
			j["model_path"] = model->model_path;
			j["model_scale"] = model->scale;
			j["model_rotation"] = {
				model->rotation_angles.pitch,
				model->rotation_angles.yaw,
				model->rotation_angles.roll
			};
		}

		// Audio
		if (auto audio = reg.get_components<component::audio>()[static_cast<std::size_t>(entity)]) {
			j["sound_path"] = audio->sound_path;
			j["volume"] = audio->volume;
			j["loop"] = audio->loop;
			j["autoplay"] = audio->autoplay;
		}

		// Text
		if (auto txt = reg.get_components<component::text>()[static_cast<std::size_t>(entity)]) {
			j["content"] = txt->content;
			j["font_size"] = txt->font_size;
			j["spacing"] = txt->spacing;
			j["text_color"] = {txt->color.r, txt->color.g, txt->color.b, txt->color.a};
		}

		// Font
		if (auto font = reg.get_components<component::font>()[static_cast<std::size_t>(entity)]) {
			j["font_path"] = font->font_path;
		}

		// Clickable
		if (auto click = reg.get_components<component::clickable>()[static_cast<std::size_t>(entity)]) {
			j["click_id"] = click->id;
			j["click_enabled"] = click->enabled;
		}

		// Hoverable
		if (auto hover = reg.get_components<component::hoverable>()[static_cast<std::size_t>(entity)]) {
			j["hover_id"] = hover->id;
			j["hovered"] = hover->isHovered;
		}

		// Controllable
		if (auto ctrl = reg.get_components<component::controllable>()[static_cast<std::size_t>(entity)]) {
			j["speed"] = ctrl->speed;
			j["can_jump"] = ctrl->can_jump;
			j["can_fly"] = ctrl->can_fly;
		}

		// Health
		if (auto health = reg.get_components<component::health>()[static_cast<std::size_t>(entity)]) {
			j["health_current"] = health->current;
			j["health_max"] = health->max;
		}

		// Damage
		if (auto dmg = reg.get_components<component::damage>()[static_cast<std::size_t>(entity)]) {
			j["damage"] = dmg->amount;
		}

		// Collision box
		if (auto cbox = reg.get_components<component::collision_box>()[static_cast<std::size_t>(entity)]) {
			j["collision_width"] = cbox->width;
			j["collision_height"] = cbox->height;
			j["collision_depth"] = cbox->depth;
		}

		// Hitbox link
		if (auto link = reg.get_components<component::hitbox_link>()[static_cast<std::size_t>(entity)]) {
			j["owner_id"] = static_cast<std::size_t>(link->owner);
			j["offsetX"] = link->offsetX;
			j["offsetY"] = link->offsetY;
			j["offsetZ"] = link->offsetZ;
		}

		// Client ID
		if (auto cid = reg.get_components<component::client_id>()[static_cast<std::size_t>(entity)]) {
			j["client_id"] = cid->id;
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
					case component::entity_type::PLAYER: game::parsing::parse_player(reg, entity_data);
						break;
					case component::entity_type::ENEMY: game::parsing::parse_enemy(reg, entity_data);
						break;
					case component::entity_type::OBSTACLE: game::parsing::parse_obstacle(reg, entity_data);
						break;
					case component::entity_type::BACKGROUND: game::parsing::parse_background(reg, entity_data);
						break;
					case component::entity_type::RANDOM_ELEMENT: game::parsing::parse_random_element(reg, entity_data);
						break;
					case component::entity_type::SOUND: game::parsing::parse_sound(reg, entity_data);
						break;
					case component::entity_type::TEXT: game::parsing::parse_text(reg, entity_data);
						break;
					case component::entity_type::ITEM: game::parsing::parse_item(reg, entity_data);
						break;
					case component::entity_type::POWERUP: game::parsing::parse_powerup(reg, entity_data);
						break;
					case component::entity_type::TRAP: game::parsing::parse_trap(reg, entity_data);
						break;
					case component::entity_type::GATE: game::parsing::parse_gate(reg, entity_data);
						break;
					case component::entity_type::WEAPON: game::parsing::parse_weapon(reg, entity_data);
						break;
					case component::entity_type::PLATFORM: game::parsing::parse_platform(reg, entity_data);
						break;
					case component::entity_type::DECORATION: game::parsing::parse_decoration(reg, entity_data);
						break;
					case component::entity_type::CHECKPOINT: game::parsing::parse_checkpoint(reg, entity_data);
						break;
					case component::entity_type::SPAWNER: game::parsing::parse_spawner(reg, entity_data);
						break;
					case component::entity_type::TRIGGERZONE: game::parsing::parse_triggerzone(reg, entity_data);
						break;
					case component::entity_type::PNG: game::parsing::parse_png(reg, entity_data);
						break;

					default:
						std::cerr << "[WARNING] Unknown entity type: " << type_value << std::endl;
						break;
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
