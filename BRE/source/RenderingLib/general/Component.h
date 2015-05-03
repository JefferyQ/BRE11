#pragma once

namespace BRE {
	class Component {
	public:
		virtual void Update(const float elapsedTime) = 0;
	};
}