#pragma once

struct ID3D11Buffer;
struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11InputLayout;
struct ID3D11VertexShader;

namespace BRE {
	class FullscreenVertexShaderData {
	public:
		FullscreenVertexShaderData();

		void PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context);
		void PostDraw(ID3D11DeviceContext1& context);

		void DrawIndexed(ID3D11DeviceContext1& context);

	private:
		void InitializeShader();
		void InitializeGeometryBuffers();

		ID3D11InputLayout* mInputLayout;
		ID3D11VertexShader* mShader;
		ID3D11Buffer* mVertexBuffer;
		ID3D11Buffer* mIndexBuffer;
		unsigned int mIndexCount;
	};
}