//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "SceneObject.h"
#include "DisplayChunk.h"
#include "ChunkObject.h"
#include "InputCommands.h"
#include <vector>

#include "Camera.h"

class Camera;
//FORWARD DECLARATIONS
class ControlHandle;
class DisplayObject;

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game : public DX::IDeviceNotify
{
public:
	Game();
	~Game();

	// Initialization and management
	void Initialize(HWND window, int width, int height);
	void SetGridState(bool state);

	// Basic game loop
	void Tick(InputCommands* Input);
	void Render();
	// Rendering helpers
	void Clear();

	// IDeviceNotify
	void OnDeviceLost() override;
	void OnDeviceRestored() override;

	// Messages
	void OnActivated();
	void OnDeactivated();
	void OnSuspending();
	void OnResuming();
	void OnWindowSizeChanged(int width, int height);

	DisplayObject* CreateDisplayObject(const SceneObject* object) const;
	//tool specific
	void RenderDisplayObject(const DisplayObject& obj) const;
	bool arcBallMode;
	void RenderDisplayObjectOnTop(const DisplayObject& obj) const;
	void BuildDisplayList(std::vector<SceneObject>* SceneGraph); //note vector passed by reference
	void BuildDisplayHierarchy(std::vector<SceneObject>* SceneGraph);
	void BuildDisplayChunk(ChunkObject* SceneChunk);
	void SaveDisplayChunk(ChunkObject* SceneChunk); //saves geometry et al
	void ClearDisplayList();
	void RemoveDisplayObject(int index);
	DisplayObject* GetDisplayObject(int index);

	//Synchronisation between scene object and display object
	void UpdateDisplayElementTransform(int index, SceneObject* sceneObj);
	bool UpdateDisplayElmentModel(int index, SceneObject* sceneObj);

	//Tool Control Handles
	std::vector<DisplayObject*> GetHandles();
	void CreateHandleObject(DisplayObject*, std::string model_path, DirectX::SimpleMath::Color) const;
	//Adds a certain prototype of a display object and returns
	// the pointer of the object actually added
	int AddVisualHandle(ControlHandle* display_object);


	//Mouse Selection Methods
	DisplayObject* MousePicking() const;
	ControlHandle* ControlHandleHitTest() const;
	DisplayObject* MousePicking(const std::vector<DisplayObject*>& objectList) const;

	DirectX::XMVECTOR GetWorldRay(float screen_x, float screen_y, float distance);
	DirectX::XMMATRIX GetObjectLocalMatrix(int i) const;


	Camera camera;
	RECT m_ScreenDimensions;
	std::vector<DisplayObject*> m_displayList;

#ifdef DXTK_AUDIO
	void NewAudioDevice();
#endif

private:
	void Update(const DX::StepTimer& timer);

	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();

	void XM_CALLCONV DrawGrid(DirectX::FXMVECTOR xAxis, DirectX::FXMVECTOR yAxis, DirectX::FXMVECTOR origin,
	                          size_t xdivs, size_t ydivs, DirectX::GXMVECTOR color);

	//tool specific
	std::vector<ControlHandle*> m_displayHandlesList;
	DisplayChunk m_displayChunk;
	InputCommands m_InputCommands;

	std::unique_ptr<DirectX::BasicEffect> m_handlesEffect;


	//control variables
	bool m_grid; //grid rendering on / off
	// Device resources.
	std::shared_ptr<DX::DeviceResources> m_deviceResources;

	// Rendering loop timer.
	DX::StepTimer m_timer;


	// Input devices.
	std::unique_ptr<DirectX::GamePad> m_gamePad;
	std::unique_ptr<DirectX::Keyboard> m_keyboard;
	std::unique_ptr<DirectX::Mouse> m_mouse;

	// DirectXTK objects.
	std::unique_ptr<DirectX::CommonStates> m_states;
	std::unique_ptr<DirectX::BasicEffect> m_batchEffect;
	std::unique_ptr<DirectX::EffectFactory> m_fxFactory;
	std::unique_ptr<DirectX::GeometricPrimitive> m_shape;
	std::unique_ptr<DirectX::Model> m_model;
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;
	std::unique_ptr<DirectX::SpriteBatch> m_sprites;
	std::unique_ptr<DirectX::SpriteFont> m_font;

#ifdef DXTK_AUDIO
	std::unique_ptr<DirectX::AudioEngine>                                   m_audEngine;
	std::unique_ptr<DirectX::WaveBank>                                      m_waveBank;
	std::unique_ptr<DirectX::SoundEffect>                                   m_soundEffect;
	std::unique_ptr<DirectX::SoundEffectInstance>                           m_effect1;
	std::unique_ptr<DirectX::SoundEffectInstance>                           m_effect2;
#endif

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture1;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture2;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_batchInputLayout;

#ifdef DXTK_AUDIO
	uint32_t                                                                m_audioEvent;
	float                                                                   m_audioTimerAcc;

	bool                                                                    m_retryDefault;
#endif

	DirectX::SimpleMath::Matrix m_world;
	DirectX::SimpleMath::Matrix m_view;
	DirectX::SimpleMath::Matrix m_projection;
};

std::wstring StringToWCHART(std::string s);
