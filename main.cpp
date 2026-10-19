#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <algorithm>
#include <cstdint>

using namespace geode::prelude;

class $modify(InfiniteTPSLayer, GJBaseGameLayer) {

	// Reimplementa el cálculo de "delta modificado" del juego, que decide
	// cuántos pasos de física se ejecutan por frame y de qué duración.
	float getModifiedDelta(float dt) {
		auto mod = Mod::get();

		if (!mod->getSettingValue<bool>("enabled")) {
			return GJBaseGameLayer::getModifiedDelta(dt);
		}

		double tps = static_cast<double>(mod->getSettingValue<int64_t>("tps"));
		int64_t maxSteps = mod->getSettingValue<int64_t>("max-steps");

		// Duración de un paso (respeta el time warp como el juego original)
		float modifier = std::min(1.f, m_gameState.m_timeWarp);
		double spt = (1.0 / tps) / modifier;

		// Tiempo acumulado a simular (este frame + resto del anterior)
		double total = static_cast<double>(dt) + static_cast<double>(m_extraDelta);

		int64_t steps = std::max<int64_t>(1, static_cast<int64_t>(total / spt));

		if (steps > maxSteps) {
			// Tope de seguridad: descartamos el exceso -> el juego va más lento
			steps = maxSteps;
			m_extraDelta = 0.f;
		}
		else {
			m_extraDelta = static_cast<float>(total - steps * spt);
		}

		return static_cast<float>(steps * spt);
	}
};
