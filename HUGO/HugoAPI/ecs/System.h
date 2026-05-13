#pragma once

#include <memory>

#ifdef __linux__
#include <stdint.h>
#endif

namespace HUGO {

	class IScene;

	// Systems are only functionality
	class System {
	protected:
		// std::shared_ptr<HUGO::IScene> scene = nullptr;
		IScene& scene;
	public:
		explicit System(IScene& scene): scene(scene) {}
		virtual ~System() {}

		//Called when the System is created
		virtual void init() = 0;

		//Called every time the scene is loaded
		virtual void onLoad() = 0;

		virtual void fixedUpdate(uint32_t dt) = 0;
		virtual void update(uint32_t dt) = 0;
	};
}