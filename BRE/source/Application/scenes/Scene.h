#pragma once

//////////////////////////////////////////////////////////////////////////
//
// Scene that shows Texture + Normal + Displacement mapping
// with fully deferred rendering
//
//////////////////////////////////////////////////////////////////////////

#include <vector>

#include <general/Component.h>
#include <rendering/lights/DirectionalLight.h>
#include <utils/updaters/PositionUpdater.h>

struct ID3D11Device1;
struct ID3D11DeviceContext1;

class Scene : public BRE::Component {
public:
	Scene();
	virtual void Update(const float elapsedTime) override;

private:
	void InitDirectionalLights();
	void InitPointLights();

	void UpdateDirectionalLight(const float elapsedTime);

	BRE::DirectionalLight mDirectionalLight;
	BRE::PositionUpdater mPosUpdater;
};
