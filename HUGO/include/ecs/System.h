#pragma once

#include <memory>
namespace HUGO {

	class IScene;

	// Systems are only functionality
	class System {
	protected:
		std::shared_ptr<HUGO::IScene> scene = nullptr;

	public:
		explicit System(std::shared_ptr<HUGO::IScene> scene): scene(scene) {}
		virtual ~System() {}

		//Called when the System is created
		virtual void init() = 0;

		//Called every time the scene is loaded
		virtual void onLoad() = 0;

		virtual void fixedUpdate(double dt) = 0;
		virtual void update(double dt) = 0;
	};

}