#pragma once
#include <iostream>
#include <memory>
#include <chrono>
#include "SoundSystem.h"

namespace dae
{
	class LoggingSoundSystem final : public SoundSystem
	{
		std::unique_ptr<SoundSystem> _real_ss;
	public:
		LoggingSoundSystem(std::unique_ptr<SoundSystem>&& ss) : _real_ss(std::move(ss)) {}
		virtual ~LoggingSoundSystem() = default;
		void Update() override 
		{
			_real_ss->Update();
		}
		void play(const std::string name, const int volume) override {
			_real_ss->play(name, volume);
			std::cout << "playing " << name << " at volume " << volume << std::endl;
		}

		void Stop() override { _real_ss->Stop(); };

		LoggingSoundSystem(const LoggingSoundSystem& other) = delete;
		LoggingSoundSystem(LoggingSoundSystem&& other) = delete;
		LoggingSoundSystem& operator=(const LoggingSoundSystem& other) = delete;
		LoggingSoundSystem& operator=(LoggingSoundSystem&& other) = delete;
	};
}