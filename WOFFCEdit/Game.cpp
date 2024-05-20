//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

#include <map>

#include <string>

#include "ControlHandle.h"
using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game()

{
	m_deviceResources = std::make_unique<DX::DeviceResources>();
	m_deviceResources->RegisterDeviceNotify(this);
	m_displayList.clear();


	//initial Settings
	//modes
	m_grid = false;
}

Game::~Game()
{
#ifdef DXTK_AUDIO
	if (m_audEngine)
	{
		m_audEngine->Suspend();
	}
#endif
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
	m_gamePad = std::make_unique<GamePad>();

	m_keyboard = std::make_unique<Keyboard>();

	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(window);

	m_deviceResources->SetWindow(window, width, height);

	m_deviceResources->CreateDeviceResources();
	CreateDeviceDependentResources();

	m_deviceResources->CreateWindowSizeDependentResources();
	CreateWindowSizeDependentResources();

	GetClientRect(window, &m_ScreenDimensions);
	camera.m_screenRect = m_ScreenDimensions;
	m_arcBallCamera.m_screenRect = m_ScreenDimensions;
	active_camera = &camera;


#ifdef DXTK_AUDIO
	// Create DirectXTK for Audio objects
	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
	eflags = eflags | AudioEngine_Debug;
#endif

	m_audEngine = std::make_unique<AudioEngine>(eflags);

	m_audioEvent = 0;
	m_audioTimerAcc = 10.f;
	m_retryDefault = false;

	m_waveBank = std::make_unique<WaveBank>(m_audEngine.get(), L"adpcmdroid.xwb");

	m_soundEffect = std::make_unique<SoundEffect>(m_audEngine.get(), L"MusicMono_adpcm.wav");
	m_effect1 = m_soundEffect->CreateInstance();
	m_effect2 = m_waveBank->CreateInstance(10);

	m_effect1->Play(true);
	m_effect2->Play();
#endif
}

void Game::SetGridState(bool state)
{
	m_grid = state;
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick(InputCommands* Input)
{
	
	//copy over the input commands so we have a local version to use elsewhere.
	m_InputCommands = *Input;
	m_timer.Tick([&]()
	{
		Update(m_timer);
	});

#ifdef DXTK_AUDIO
	// Only update audio engine once per frame
	if (!m_audEngine->IsCriticalError() && m_audEngine->Update())
	{
		// Setup a retry in 1 second
		m_audioTimerAcc = 1.f;
		m_retryDefault = true;
	}
#endif

	for (ControlHandle* handle : m_displayHandlesList)
	{
		handle->Update();
	}

	Render();
}

// Updates the world.
void Game::Update(const DX::StepTimer& timer)
{
	active_camera->Update(m_InputCommands);
	m_view = active_camera->GetView();

	m_batchEffect->SetView(m_view);
	m_batchEffect->SetWorld(Matrix::Identity);
	m_displayChunk.m_terrainEffect->SetView(m_view);
	m_displayChunk.m_terrainEffect->SetWorld(Matrix::Identity);
	//	m_handlesEffect->SetView(m_view);
	//	m_handlesEffect->SetWorld(Matrix::Identity);

#ifdef DXTK_AUDIO
	m_audioTimerAcc -= (float)timer.GetElapsedSeconds();
	if (m_audioTimerAcc < 0)
	{
		if (m_retryDefault)
		{
			m_retryDefault = false;
			if (m_audEngine->Reset())
			{
				// Restart looping audio
				m_effect1->Play(true);
			}
		}
		else
		{
			m_audioTimerAcc = 4.f;

			m_waveBank->Play(m_audioEvent++);

			if (m_audioEvent >= 11)
				m_audioEvent = 0;
		}
	}
#endif
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
	{
		return;
	}

	Clear();

	m_deviceResources->PIXBeginEvent(L"Render");
	auto context = m_deviceResources->GetD3DDeviceContext();

	if (m_grid)
	{
		// Draw procedurally generated dynamic grid
		const XMVECTORF32 xaxis = {512.f, 0.f, 0.f};
		const XMVECTORF32 yaxis = {0.f, 0.f, 512.f};
		DrawGrid(xaxis, yaxis, g_XMZero, 512, 512, Colors::Gray);
	}
	//CAMERA POSITION ON HUD
	//	m_sprites->Begin();
	//	WCHAR Buffer[256];
	//	std::wstring var = L"Cam X: " + std::to_wstring(m_camPosition.x) + L"Cam Z: " + std::to_wstring(m_camPosition.z);
	//	m_font->DrawString(m_sprites.get(), var.c_str(), XMFLOAT2(100, 10), Colors::Yellow);
	//	m_sprites->End();

	//RENDER DISPLAY HANDLES
	//RENDER OBJECTS FROM SCENEGRAPH
	context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthDefault(), 0);
	context->RSSetState(m_states->CullNone());
	int numRenderObjects = m_displayList.size();
	for (int i = 0; i < numRenderObjects; i++)
	{
		RenderDisplayObject(*m_displayList[i]);
	}

	m_deviceResources->PIXEndEvent();

	//RENDER TERRAIN
	//context->RSSetState(m_states->Wireframe());		//uncomment for wireframe

	//Render the batch,  This is handled in the Display chunk becuase it has the potential to get complex
	m_displayChunk.RenderBatch(m_deviceResources);

	//m_handlesEffect->Apply(context);
	for (const DisplayObject* handle : m_displayHandlesList)
	{
		//context->OMSetBlendState(m_states->Additive(), nullptr, 0xFFFFFFFF);

		RenderDisplayObjectOnTop(*handle);
	}
	m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
	m_deviceResources->PIXBeginEvent(L"Clear");

	// Clear the views.
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTarget = m_deviceResources->GetBackBufferRenderTargetView();
	auto depthStencil = m_deviceResources->GetDepthStencilView();

	context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);

	// Set the viewport.
	auto viewport = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	m_deviceResources->PIXEndEvent();
}

void XM_CALLCONV Game::DrawGrid(FXMVECTOR xAxis, FXMVECTOR yAxis, FXMVECTOR origin, size_t xdivs, size_t ydivs,
                                GXMVECTOR color)
{
	m_deviceResources->PIXBeginEvent(L"Draw grid");

	auto context = m_deviceResources->GetD3DDeviceContext();
	context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthNone(), 0);
	context->RSSetState(m_states->CullCounterClockwise());

	m_batchEffect->Apply(context);

	context->IASetInputLayout(m_batchInputLayout.Get());

	m_batch->Begin();

	xdivs = std::max<size_t>(1, xdivs);
	ydivs = std::max<size_t>(1, ydivs);

	for (size_t i = 0; i <= xdivs; ++i)
	{
		float fPercent = float(i) / float(xdivs);
		fPercent = (fPercent * 2.0f) - 1.0f;
		XMVECTOR vScale = XMVectorScale(xAxis, fPercent);
		vScale = XMVectorAdd(vScale, origin);

		VertexPositionColor v1(XMVectorSubtract(vScale, yAxis), color);
		VertexPositionColor v2(XMVectorAdd(vScale, yAxis), color);
		m_batch->DrawLine(v1, v2);
	}

	for (size_t i = 0; i <= ydivs; i++)
	{
		float fPercent = float(i) / float(ydivs);
		fPercent = (fPercent * 2.0f) - 1.0f;
		XMVECTOR vScale = XMVectorScale(yAxis, fPercent);
		vScale = XMVectorAdd(vScale, origin);

		VertexPositionColor v1(XMVectorSubtract(vScale, xAxis), color);
		VertexPositionColor v2(XMVectorAdd(vScale, xAxis), color);
		m_batch->DrawLine(v1, v2);
	}

	m_batch->End();

	m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
}

void Game::OnDeactivated()
{
}

void Game::OnSuspending()
{
#ifdef DXTK_AUDIO
	m_audEngine->Suspend();
#endif
}

void Game::OnResuming()
{
	m_timer.ResetElapsedTime();

#ifdef DXTK_AUDIO
	m_audEngine->Resume();
#endif
}

void Game::OnWindowSizeChanged(int width, int height)
{
	if (!m_deviceResources->WindowSizeChanged(width, height))
		return;
}

void Game::UpdateDisplayElementTransform(int i, SceneObject* sceneObject)
{
	DisplayObject& newDisplayObject = *GetDisplayObject(i);
	sceneObject->posX    = newDisplayObject.m_position.x ;
	sceneObject->posY = newDisplayObject.m_position.y;
	sceneObject->posZ = newDisplayObject.m_position.z;

	//setorientation
	sceneObject->rotX = newDisplayObject.m_orientation.x;
	sceneObject->rotY = newDisplayObject.m_orientation.y;
	sceneObject->rotZ = newDisplayObject.m_orientation.z;

	//set scale
	sceneObject->scaX = newDisplayObject.m_scale.x;
	sceneObject->scaY = newDisplayObject.m_scale.y;
	sceneObject->scaZ = newDisplayObject.m_scale.z;

	//set wireframe / render flags
	sceneObject->editor_render = newDisplayObject.m_render;
	sceneObject->editor_wireframe = newDisplayObject.m_wireframe;
	sceneObject->light_type = newDisplayObject.m_light_type;
	sceneObject->light_diffuse_r = newDisplayObject.m_light_diffuse_r;
	sceneObject->light_diffuse_g = newDisplayObject.m_light_diffuse_g;
	sceneObject->light_diffuse_b = newDisplayObject.m_light_diffuse_b;
	sceneObject->light_specular_r = newDisplayObject.m_light_specular_r;
	sceneObject->light_specular_g = newDisplayObject.m_light_specular_g;
	sceneObject->light_specular_b = newDisplayObject.m_light_specular_b;
	sceneObject->light_spot_cutoff = newDisplayObject.m_light_spot_cutoff;
	sceneObject->light_constant = newDisplayObject.m_light_constant;
	sceneObject->light_linear = newDisplayObject.m_light_linear;
	newDisplayObject.m_light_quadratic = sceneObject->light_quadratic;
}

bool Game::UpdateDisplayElmentModel(int index, SceneObject* sceneObject)
{
	//Rearead model and texture

	DisplayObject* newDisplayObject = GetDisplayObject(index);

	auto device = m_deviceResources->GetD3DDevice();
	//load model
	std::wstring modelwstr =
		StringToWCHART(newDisplayObject->model_path); //convect string to Wchar
	try
	{
		newDisplayObject->m_model =
			Model::CreateFromCMO(
				device,
				modelwstr.c_str(),
				*m_fxFactory,
				true);
	}
	catch (...)
	{
		//Loading model failed
		return false;
	}
	//get DXSDK to load model "False" for LH coordinate system (maya)

	//Load Texture
	std::wstring texturewstr =
		StringToWCHART(newDisplayObject->texture_path); //convect string to Wchar
	HRESULT rs;
	rs = CreateDDSTextureFromFile(
		device,
		texturewstr.c_str(),
		nullptr,
		&newDisplayObject->m_texture_diffuse);
	//load tex into Shader resource

	//if texture fails.  load error default
	if (rs)
	{
		CreateDDSTextureFromFile(device,
		                         L"database/data/Error.dds",
		                         nullptr,
		                         &newDisplayObject->m_texture_diffuse);
		//load tex into Shader resource
	}


	newDisplayObject->m_model->UpdateEffects(
		[&](IEffect* effect) //This uses a Lambda function,  if you dont understand it: Look it up.
		{
			auto lights = dynamic_cast<BasicEffect*>(effect);
			if (lights)
			{
				lights->SetTexture(newDisplayObject->m_texture_diffuse);
				XMVECTOR diffuse{
					newDisplayObject->m_light_diffuse_r,
					newDisplayObject->m_light_diffuse_g,
					newDisplayObject->m_light_diffuse_b,
					1
				};
				lights->SetDiffuseColor(diffuse);
			}
		});


	sceneObject->model_path = newDisplayObject->model_path;
	sceneObject->tex_diffuse_path = newDisplayObject->texture_path;
	return true;
}

void Game::CreateHandleObject(DisplayObject* newDisplayObject, std::string model_path,
                              DirectX::SimpleMath::Color color) const
{
	//create a temp display object that we will populate then append to the display list.
	auto device = m_deviceResources->GetD3DDevice();

	//load model
	std::wstring modelwstr = StringToWCHART(model_path); //convect string to Wchar
	newDisplayObject->m_model = Model::CreateFromCMO(device, modelwstr.c_str(), *m_fxFactory, true);
	//get DXSDK to load model "False" for LH coordinate system (maya)

	//set scale
	newDisplayObject->m_scale.x = 1;
	newDisplayObject->m_scale.y = 1;
	newDisplayObject->m_scale.z = 1;

	//set wireframe / render flags
	newDisplayObject->m_render = true;
	newDisplayObject->m_wireframe = false;

	newDisplayObject->m_light_diffuse_r = color.R();
	newDisplayObject->m_light_diffuse_g = color.G();
	newDisplayObject->m_light_diffuse_b = color.B();

	newDisplayObject->m_model->UpdateEffects(
		[&](IEffect* effect) //This uses a Lambda function,  if you dont understand it: Look it up.
		{
			auto lights = dynamic_cast<BasicEffect*>(effect);
			if (lights)
			{
				//lights->SetTexture(newDisplayObject->m_texture_diffuse);
				XMVECTOR diffuse{
					newDisplayObject->m_light_diffuse_r,
					newDisplayObject->m_light_diffuse_g,
					newDisplayObject->m_light_diffuse_b,
					1
				};
				lights->SetDiffuseColor(diffuse);
			}
		});
}

DisplayObject* Game::CreateDisplayObject(const SceneObject* object) const
{
	//create a temp display object that we will populate then append to the display list.
	auto newDisplayObject = new DisplayObject();
	auto device = m_deviceResources->GetD3DDevice();

	//load model
	std::wstring modelwstr = StringToWCHART(object->model_path); //convect string to Wchar
	newDisplayObject->m_model = Model::CreateFromCMO(device, modelwstr.c_str(), *m_fxFactory, true);
	//get DXSDK to load model "False" for LH coordinate system (maya)

	//Load Texture
	std::wstring texturewstr = StringToWCHART(object->tex_diffuse_path); //convect string to Wchar
	HRESULT rs;
	rs = CreateDDSTextureFromFile(device, texturewstr.c_str(), nullptr, &newDisplayObject->m_texture_diffuse);
	//load tex into Shader resource

	//if texture fails.  load error default
	if (rs)
	{
		CreateDDSTextureFromFile(device, L"database/data/Error.dds", nullptr, &newDisplayObject->m_texture_diffuse);
		//load tex into Shader resource
	}

	//apply new texture to models effect

	newDisplayObject->m_ID = object->ID;

	//set position
	newDisplayObject->m_position.x = object->posX;
	newDisplayObject->m_position.y = object->posY;
	newDisplayObject->m_position.z = object->posZ;

	//setorientation
	newDisplayObject->m_orientation.x = object->rotX;
	newDisplayObject->m_orientation.y = object->rotY;
	newDisplayObject->m_orientation.z = object->rotZ;

	//set scale
	newDisplayObject->m_scale.x = object->scaX;
	newDisplayObject->m_scale.y = object->scaY;
	newDisplayObject->m_scale.z = object->scaZ;

	//set wireframe / render flags
	newDisplayObject->m_render = object->editor_render;
	newDisplayObject->m_wireframe = object->editor_wireframe;

	newDisplayObject->m_light_type = object->light_type;
	newDisplayObject->m_light_diffuse_r = object->light_diffuse_r;
	newDisplayObject->m_light_diffuse_g = object->light_diffuse_g;
	newDisplayObject->m_light_diffuse_b = object->light_diffuse_b;
	newDisplayObject->m_light_specular_r = object->light_specular_r;
	newDisplayObject->m_light_specular_g = object->light_specular_g;
	newDisplayObject->m_light_specular_b = object->light_specular_b;
	newDisplayObject->m_light_spot_cutoff = object->light_spot_cutoff;
	newDisplayObject->m_light_constant = object->light_constant;
	newDisplayObject->m_light_linear = object->light_linear;
	newDisplayObject->m_light_quadratic = object->light_quadratic;
	newDisplayObject->model_path = object->model_path;
	newDisplayObject->texture_path = object->tex_diffuse_path;
	newDisplayObject->m_model->UpdateEffects(
		[&](IEffect* effect) //This uses a Lambda function,  if you dont understand it: Look it up.
		{
			auto lights = dynamic_cast<BasicEffect*>(effect);
			if (lights)
			{
				lights->SetTexture(newDisplayObject->m_texture_diffuse);
				XMVECTOR diffuse{
					newDisplayObject->m_light_diffuse_r,
					newDisplayObject->m_light_diffuse_g,
					newDisplayObject->m_light_diffuse_b,
					1
				};
				lights->SetDiffuseColor(diffuse);
			}
		});




	return newDisplayObject;
}

void Game::RenderDisplayObject(const DisplayObject& obj) const
{
	if (obj.m_render == false) return;
	auto context = m_deviceResources->GetD3DDeviceContext();
	m_deviceResources->PIXBeginEvent(L"Draw model");
	XMMATRIX world =
		m_world * obj.GetWorldMatrix();

	obj.m_model->Draw(context, *m_states, world, m_view, m_projection, obj.m_wireframe);
	//last variable in draw,  make TRUE for wireframe

	m_deviceResources->PIXEndEvent();
}

void Game::RenderDisplayObjectOnTop(const DisplayObject& obj) const
{
	if (obj.m_render == false) return;
	auto context = m_deviceResources->GetD3DDeviceContext();
	m_deviceResources->PIXBeginEvent(L"Draw model");
	XMMATRIX world =
		m_world * obj.GetWorldMatrix();

	context->OMSetDepthStencilState(m_states->DepthNone(), 0);
	auto model = obj.m_model.get();
	assert(model != nullptr);

	for (const auto& it : model->meshes)
	{
		auto mesh = it.get();
		assert(mesh != nullptr);

		// Do model-level setCustomState work here
		mesh->Draw(context, world, m_view, m_projection, false);
	}
	m_deviceResources->PIXEndEvent();
}

void Game::BuildDisplayList(std::vector<SceneObject>* SceneGraph)
{
	auto device = m_deviceResources->GetD3DDevice();
	auto devicecontext = m_deviceResources->GetD3DDeviceContext();

	if (!m_displayList.empty()) //is the vector empty
	{
		m_displayList.clear(); //if not, empty it
	}

	//for every item in the scenegraph
	int numObjects = SceneGraph->size();
	for (int i = 0; i < numObjects; i++)
	{
		m_displayList.push_back(CreateDisplayObject(&SceneGraph->at(i)));
	}
}

void Game::BuildDisplayHierarchy(std::vector<SceneObject>* SceneGraph)
{
	std::map<int, DisplayObject*> idToTreeItems;
	std::vector<SceneObject> sceneCopy = *SceneGraph;
	while (sceneCopy.size() != 0)
	{
		//Remove items which id's are already presented in object to tree map
		auto removed = std::remove_if(sceneCopy.begin(), sceneCopy.end(),
		                              [idToTreeItems](SceneObject v)
		                              {
			                              return idToTreeItems.find(v.ID) != idToTreeItems.end();
		                              });
		sceneCopy.erase(removed, sceneCopy.end());
		for (const SceneObject& element : sceneCopy)
		{
			if (element.parent_id == 0)
			{
				DisplayObject* object = CreateDisplayObject(&element);
				m_displayList.push_back(object);
				idToTreeItems.insert({element.ID, object});
			}
			else
			{
				if (idToTreeItems.find(element.parent_id) != idToTreeItems.end())
				{
					DisplayObject* parent = idToTreeItems.at(element.parent_id);
					DisplayObject* object = CreateDisplayObject(&element);
					m_displayList.push_back(object);
					if (element.parent_id == 9)
					{
						int c = 3;
					}
					object->parentObject = parent;
					idToTreeItems.insert({element.ID, object});
				}
			}
		}
	}
	int a = 3;
}

void Game::BuildDisplayChunk(ChunkObject* SceneChunk)
{
	//populate our local DISPLAYCHUNK with all the chunk info we need from the object stored in toolmain
	//which, to be honest, is almost all of it. Its mostly rendering related info so...
	m_displayChunk.PopulateChunkData(SceneChunk); //migrate chunk data
	m_displayChunk.LoadHeightMap(m_deviceResources);
	m_displayChunk.m_terrainEffect->SetProjection(m_projection);
	m_displayChunk.InitialiseBatch();
}

void Game::SaveDisplayChunk(ChunkObject* SceneChunk)
{
	m_displayChunk.SaveHeightMap(); //save heightmap to file.
}

int Game::AddVisualHandle(ControlHandle* display_object)
{
	this->m_displayHandlesList.push_back(display_object);
	//Returns local object
	return m_displayHandlesList.size() - 1;
	//	return &this->m_displayList[m_displayList.size() - 1];
	//	return &this->m_displayList.at(this->m_displayList.size() - 1);
}

//std::vector<DisplayObject*> Game::GetHandles()
//{
//	return std::vector<DisplayObject*>(m_displayHandlesList);
//}

DisplayObject* Game::GetDisplayObject(int index)
{

	for (int i = 0; i < m_displayList.size(); ++i)
	{
		if(m_displayList[i]->m_ID == index)
			return m_displayList[i];
	}
	return nullptr;
}

void Game::RemoveDisplayObject(int objectId)
{
	int indexFound = -1;
	for (int i = 0; i < m_displayList.size(); ++i)
	{
		if(m_displayList[i]->m_ID == objectId)
		{
			indexFound = i;
			break;
		}
		
	}
	m_displayList.erase(this->m_displayList.begin() + indexFound);


}

void Game::AddRootDisplayObject(DisplayObject* object)
{
	this->m_displayList.push_back(object);
}

#ifdef DXTK_AUDIO
void Game::NewAudioDevice()
{
	if (m_audEngine && !m_audEngine->IsAudioDevicePresent())
	{
		// Setup a retry in 1 second
		m_audioTimerAcc = 1.f;
		m_retryDefault = true;
	}
}
#endif

#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto device = m_deviceResources->GetD3DDevice();

	m_states = std::make_unique<CommonStates>(device);

	m_fxFactory = std::make_unique<EffectFactory>(device);
	m_fxFactory->SetDirectory(L"database/data/"); //fx Factory will look in the database directory
	m_fxFactory->SetSharing(false);
	//we must set this to false otherwise it will share effects based on the initial tex loaded (When the model loads) rather than what we will change them to.

	m_sprites = std::make_unique<SpriteBatch>(context);

	m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

	m_batchEffect = std::make_unique<BasicEffect>(device);
	m_batchEffect->SetVertexColorEnabled(true);

	//Create effect for handles
	m_handlesEffect = std::make_unique<BasicEffect>(m_deviceResources->GetD3DDevice());
	//m_handlesEffect->EnableDefaultLighting();
	m_handlesEffect->SetLightingEnabled(false);
	m_handlesEffect->SetTextureEnabled(false);
	{
		const void* shaderByteCode;
		size_t byteCodeLength;

		m_handlesEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
	}

	{
		const void* shaderByteCode;
		size_t byteCodeLength;

		m_batchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

		DX::ThrowIfFailed(
			device->CreateInputLayout(VertexPositionColor::InputElements,
			                          VertexPositionColor::InputElementCount,
			                          shaderByteCode, byteCodeLength,
			                          m_batchInputLayout.ReleaseAndGetAddressOf())
		);
	}

	m_font = std::make_unique<SpriteFont>(device, L"SegoeUI_18.spritefont");

	//    m_shape = GeometricPrimitive::CreateTeapot(context, 4.f, 8);

	// SDKMESH has to use clockwise winding with right-handed coordinates, so textures are flipped in U
	m_model = Model::CreateFromSDKMESH(device, L"tiny.sdkmesh", *m_fxFactory);

	std::function<void(IEffect*)> setEffectFun(
		[](IEffect* e)-> void
		{
			auto lights = dynamic_cast<IEffectLights*>(e);
			auto matrices = dynamic_cast<IEffectMatrices*>(e);

			if (lights)
			{
				lights->SetLightingEnabled(false);
				lights->SetLightDiffuseColor(0, Colors::Gold);
			}
			if (matrices)
			{
				matrices->SetProjection(XMMatrixIdentity());
			}
		});

	m_model->UpdateEffects(setEffectFun);
	// Load textures
	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"seafloor.dds", nullptr, m_texture1.ReleaseAndGetAddressOf())
	);

	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(device, L"windowslogo.dds", nullptr, m_texture2.ReleaseAndGetAddressOf())
	);
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
	auto size = m_deviceResources->GetOutputSize();
	float aspectRatio = float(size.right) / float(size.bottom);
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	// This sample makes use of a right-handed coordinate system using row-major matrices.
	m_projection = Matrix::CreatePerspectiveFieldOfView(
		fovAngleY,
		aspectRatio,
		0.01f,
		1000.0f
	);

	m_batchEffect->SetProjection(m_projection);
}

XMMATRIX Game::GetObjectLocalMatrix(int i) const
{
	//	const XMVECTORF32 scale = { m_displayList[i]->m_scale.x, m_displayList[i]->m_scale.y, m_displayList[i]->m_scale.z };
	//	const XMVECTORF32 translate = { m_displayList[i]->m_position.x, m_displayList[i]->m_position.y, m_displayList[i]->m_position.z };
	//	//convert degrees into radians for rotation matrix
	//	XMVECTOR rotate = Quaternion::CreateFromYawPitchRoll(m_displayList[i]->m_orientation.y * 3.1415 / 180,
	//		m_displayList[i]->m_orientation.x * 3.1415 / 180,
	//		m_displayList[i]->m_orientation.z * 3.1415 / 180);
	//	XMMATRIX local = m_world * XMMatrixTransformation(g_XMZero, Quaternion::Identity, scale, g_XMZero, rotate, translate);
	return m_world * m_displayList[i]->GetWorldMatrix();
}

void Game::OnDeviceLost()
{
	m_states.reset();
	m_fxFactory.reset();
	m_sprites.reset();
	m_batch.reset();
	m_batchEffect.reset();
	m_font.reset();
	m_shape.reset();
	m_model.reset();
	m_texture1.Reset();
	m_texture2.Reset();
	m_batchInputLayout.Reset();
}

void Game::OnDeviceRestored()
{
	CreateDeviceDependentResources();

	CreateWindowSizeDependentResources();
}
#pragma endregion

std::wstring StringToWCHART(std::string s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	auto buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

DisplayObject* Game::MousePicking() const
{
	return this->MousePicking(m_displayList);
}

ControlHandle* Game::ControlHandleHitTest() const
{
	std::vector<DisplayObject*> baseObjectVector;
	for (ControlHandle* handle : m_displayHandlesList)
	{
		baseObjectVector.push_back(handle);
	}
	return static_cast<ControlHandle*>(this->MousePicking(baseObjectVector));
}

//int Game::MousePicking(std::vector<int> handleList) const
//{
//	std::vector<DisplayObject> objectsFromHandles;
//	for (int i = 0; i < handleList.size(); ++i)
//	{
//		//For each index in the handle list get the corresponding object
//		objectsFromHandles.push_back(m_displayList[handleList[i]]);
//	}
//	//Loop through gathered object to check selection;
//	return this->MousePicking(objectsFromHandles);
//}

DisplayObject* Game::MousePicking(const std::vector<DisplayObject*>& objectList) const
{
	DisplayObject* selected =nullptr;
	float pickedDistance = 0;
	float closestPickedDistance = D3D11_FLOAT32_MAX;

	//setup near and far planes of frustum with mouse X and mouse y passed down from Toolmain.
	//they may look the same but note, the difference in Z
	const XMVECTOR nearSource = XMVectorSet(m_InputCommands.mouseX, m_InputCommands.mouseY, 0.0f, 1.0f);
	const XMVECTOR farSource = XMVectorSet(m_InputCommands.mouseX, m_InputCommands.mouseY, 1.0f, 1.0f);

	//Loop through entire display list of objects and pick with each in turn.
	for (int i = 0; i < objectList.size(); i++)
	{
		XMMATRIX local = m_world * objectList[i]->GetWorldMatrix();
		//Unproject the points on the near and far plane, with respect to the matrix we just created.
		XMVECTOR nearPoint = XMVector3Unproject(nearSource, 0.0f, 0.0f, m_ScreenDimensions.right,
		                                        m_ScreenDimensions.bottom,
		                                        m_deviceResources->GetScreenViewport().MinDepth,
		                                        m_deviceResources->GetScreenViewport().MaxDepth, m_projection, m_view,
		                                        local);

		XMVECTOR farPoint = XMVector3Unproject(farSource, 0.0f, 0.0f, m_ScreenDimensions.right,
		                                       m_ScreenDimensions.bottom,
		                                       m_deviceResources->GetScreenViewport().MinDepth,
		                                       m_deviceResources->GetScreenViewport().MaxDepth, m_projection, m_view,
		                                       local);

		//turn the transformed points into our picking vector.
		XMVECTOR pickingVector = farPoint - nearPoint;
		pickingVector = XMVector3Normalize(pickingVector);

		//loop through mesh list for object
		for (int y = 0; y < objectList[i]->m_model.get()->meshes.size(); y++)
		{
			//checking for ray intersection
			if (objectList[i]->m_model.get()->meshes[y]->boundingBox.Intersects(
				nearPoint, pickingVector, pickedDistance))
			{
				if (pickedDistance < closestPickedDistance)
				{
					closestPickedDistance = pickedDistance;
					selected = objectList[i];
				}
			}
		}
	}

	//if we got a hit.  return it.
	return selected;
}

DirectX::XMVECTOR Game::GetWorldRay(float mouseX, float mouseY, float distance)
{
	//Define screen dimensions
	float screenWidth = m_ScreenDimensions.right - m_ScreenDimensions.left;
	float screenHeight = m_ScreenDimensions.bottom - m_ScreenDimensions.top;

	//Inverse view projection to return from
	//clip space to world space
	DirectX::XMMATRIX invProjectionView =
		DirectX::XMMatrixInverse(
			&DirectX::XMMatrixDeterminant(
				m_view * m_projection),
			(m_view * m_projection));

	//get NDC
	float x = (((2.0f * mouseX) / screenWidth) - 1);
	float y = -(((2.0f * mouseY) / screenHeight) - 1);

	DirectX::XMVECTOR mousePosition =
		DirectX::XMVectorSet(x, y, 0.0f, 1.0f);

	XMVECTOR mouseInWorldSpace =
		DirectX::XMVector3Transform(mousePosition, invProjectionView);

	mouseInWorldSpace.m128_f32[0] /= mouseInWorldSpace.m128_f32[3];
	mouseInWorldSpace.m128_f32[1] /= mouseInWorldSpace.m128_f32[3];
	mouseInWorldSpace.m128_f32[2] /= mouseInWorldSpace.m128_f32[3];
	mouseInWorldSpace.m128_f32[3] = 0;;

	return mouseInWorldSpace;
}
