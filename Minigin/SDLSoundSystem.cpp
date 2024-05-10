#include "SDLSoundSystem.h"
#include <future>
#include <mutex>

class dae::SDLSoundSystem::Impl final
{
public:
	Impl(const std::string& dataPath);
	~Impl();

	void Update();
	
	void play(const std::string name, const int volume);
	
	void Stop();

	void StartSound(Mix_Chunk* soundEffect, const int volume);

	Impl(const Impl& other) = delete;
	Impl(Impl&& other) = delete;
	Impl& operator=(const Impl& other) = delete;
	Impl& operator=(Impl&& other) = delete;

private:
	std::mutex m_Mutex;
	std::promise<void> m_Promise;
	std::future<void> m_Future;

	std::string m_dataPath;

	static const int MAX_PENDING = 16;

	PlayMessage pending_[MAX_PENDING];
	int numPending_;

	int m_Head;
	int m_Tail;
};

dae::SDLSoundSystem::Impl::Impl(const std::string& dataPath)
{
	m_Future = m_Promise.get_future();

	m_dataPath = dataPath;
	m_Head = 0;
	m_Tail = 0;

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
		return;
	}
}

dae::SDLSoundSystem::Impl::~Impl()
{
	Mix_CloseAudio();
}

void dae::SDLSoundSystem::Impl::Update()
{
	m_Future.wait();

	if (m_Head == m_Tail) return;

	std::lock_guard<std::mutex> lock(m_Mutex);

	Mix_Chunk* soundEffect = Mix_LoadWAV(pending_[m_Head].name.c_str());
	if (!soundEffect)
	{
		std::cerr << "Failed to load sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
		return;
	}
	StartSound(soundEffect, pending_[m_Head].volume);

	m_Head = (m_Head + 1) % MAX_PENDING;

	m_Promise = std::promise<void>();
	m_Future = m_Promise.get_future();
}

void dae::SDLSoundSystem::Impl::play(const std::string name, const int volume)
{
	std::lock_guard<std::mutex> lock(m_Mutex);

	std::string path = m_dataPath + name;

	for (int i = m_Head; i != m_Tail; i = (i + 1) % MAX_PENDING)
	{
		if (pending_[i].name == path)
		{
			pending_[i].volume = std::max(volume, pending_[i].volume);

			return;
		}
	}

	assert((m_Tail + 1) % MAX_PENDING != m_Head);

	pending_[m_Tail].name = path;
	pending_[m_Tail].volume = volume;
	m_Tail = (m_Tail + 1) % MAX_PENDING;

	m_Promise.set_value();
}

void dae::SDLSoundSystem::Impl::StartSound(Mix_Chunk* soundEffect, const int volume)
{
	Mix_VolumeChunk(soundEffect, volume);

	Mix_PlayChannel(-1, soundEffect, 0);
}

void dae::SDLSoundSystem::Impl::Stop()
{
	m_Promise.set_value();
}

dae::SDLSoundSystem::SDLSoundSystem(const std::string& dataPath)
{
	m_Pimpl = std::make_unique<Impl>(dataPath);
}

dae::SDLSoundSystem::~SDLSoundSystem()
{

}

void dae::SDLSoundSystem::Update()
{
	m_Pimpl.get()->Update();
}

void dae::SDLSoundSystem::play(const std::string name, const int volume)
{
	m_Pimpl.get()->play(name, volume);
}

void dae::SDLSoundSystem::Stop()
{
	m_Pimpl.get()->Stop();
}