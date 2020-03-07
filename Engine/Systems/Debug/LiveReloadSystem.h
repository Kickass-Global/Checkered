//
// Created by root on 15/1/20.
//

#pragma once

#ifndef ENGINE_LIVERELOADSYSTEM_H
#define ENGINE_LIVERELOADSYSTEM_H

#include <string>
#include <filesystem>
#include <map>
#include "../../main.h"
#include "Engine.h"
#include "../Events/Events.h"
#include "systeminterface.hpp"

namespace Debug {

	namespace {
		char module[] = "Debug";
	}

	/**
	 * This system will handle watching assets on disk and reloading them while the game is already running
	 */
	class LiveReloadSystem : public ::Engine::SystemInterface {

		struct AssetDetails {
			std::filesystem::file_time_type last_modified_time;
		};

		std::map<std::string, AssetDetails> modified;

	public:

		EventDelegate<std::string> onAssetModified{ "onAssetModified" };

		LiveReloadSystem() : modified() {}

		void watch(const std::string &filename) {
			try {
				::Engine::assertLog(std::filesystem::exists(filename), "File to watch '", filename, "' does exists.");				
				modified[filename] = { std::filesystem::last_write_time(filename) };
			}
			catch (std::exception &/*ignored*/) {
				::Engine::assertLog(false, "Adding ", filename, " to watch");
			}
		}

		void update(::Engine::deltaTime /*elapsed*/) {

			for (const auto&[filename, details] : modified) {
				try {
					auto last_modified_time = std::filesystem::last_write_time(
						filename);
					if (last_modified_time != details.last_modified_time) {

						modified[filename].last_modified_time = last_modified_time;

						Engine::log<module>("Asset was modified: ", filename);

						onAssetModified(filename);

					}
				}
				catch (std::exception &/*ignored*/) {
					continue;
				}
			}
		}
	};

}

#endif //ENGINE_LIVERELOADSYSTEM_H
