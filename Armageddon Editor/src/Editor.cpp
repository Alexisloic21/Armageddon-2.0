#include <stdlib.h>  
#include <crtdbg.h>
#include "Application.h"
#include "Main.h"
#include "Log.h"

#include "Renderer\Camera\Camera.h"
#include "Renderer\Primitives.h"
#include "Renderer\Interface.h"
#include "Panel/ContentBrowser.h"
#include "Panel/EntityList.h"
#include "Panel/EntityProperties.h"
#include "Panel/MaterialEditor.h"
#include "Panel/BoneDebug.h"
#include "Panel/RessourceManager.h"
#include "Panel/OuputLog.h"

#include "Scene/Scene.h"
#include "Renderer/EnvMap.h"
#include "Renderer/ConstantBuffer.h"
#include "Renderer/ConstantBufferTypes.h"
#include "Renderer/Renderer2D.h"
#include "Scene/Serializer.h"
#include "Renderer/CascadeShadow.h"
#include "Renderer/PostProcessing/Bloom.h"

#include "Scripting/ScriptEngine.h"
#include "Utils/Timer.h"
#include "imgui.h"
#include "ImGuizmo.h"

class Editor : public Armageddon::Application
{
public:
	
	void OnUpdate() override;
	void OnRender() override;
	void ImGuiRender() override;
	void OnInit() override;
    void onMouseEvent(MouseEvent::MEventType e, float x, float y);
    void onKeyBoardEvent(const unsigned char keyCode);
    void RenderScene(bool BindMat);

    Armageddon::PixelShaders  FinalPassPixel;
    Armageddon::VertexShaders FinalPassVertex;

    Armageddon::PixelShaders  GbufferPixel;
    Armageddon::PixelShaders  GbufferCombine;

    Armageddon::VertexShaders defaultVertexShader;

	Editor()
	{
        m_Scene.m_SceneState = SceneState::Editor;
        FinalPassVertex = AssetManager::GetOrCreateVertexShader(L"..\\bin\\Debug-x64\\Armageddon 2.0\\BloomThresholdVertex.cso");
        FinalPassPixel = AssetManager::GetOrCreatePixelShader(L"..\\bin\\Debug-x64\\Armageddon 2.0\\CombinePixel.cso");

        defaultVertexShader = AssetManager::GetOrCreateVertexShader(L"..\\bin\\Debug-x64\\Armageddon 2.0\\DefaultVertexShader.cso");
       //TODO REORGANISER TOUT LE PROJET

        GbufferPixel = AssetManager::GetOrCreatePixelShader(L"..\\bin\\Debug-x64\\Armageddon 2.0\\Gbuffer.cso");
        GbufferCombine = AssetManager::GetOrCreatePixelShader(L"..\\bin\\Debug-x64\\Armageddon 2.0\\GbufferCombine.cso");

        m_Envmap = EnvMap(L"..\\Armageddon Editor\\Assets\\Texture\\Skybox\\HDR\\sunset_jhbcentral_1k.hdr");
        m_PlayButton.Create(L"Ressources//Icones//Editor//PlayButton.png");

        auto materialRef = AssetManager::GetOrCreateMaterial("LightMaterial");
        AssetManager::m_MaterialMap[materialRef].SetVertexShader(L"..\\bin\\Debug-x64\\Armageddon 2.0\\BillBoardVertex.cso");
        AssetManager::m_MaterialMap[materialRef].SetPixelShader(L"..\\bin\\Debug-x64\\Armageddon 2.0\\BillBoardPixel.cso");
        AssetManager::m_MaterialMap[materialRef].SetAlbedoMap(L"Ressources//Icones//Editor//icone_point_light.png");
        AssetManager::m_MaterialMap[materialRef].RenderMode = 1;
	}

	~Editor()
	{
        for (auto& mat : AssetManager::v_material)
        {
            //m_serializer.SerializeMaterial("Assets/Materials/" + mat.m_AssetName + ".mat", mat);
        }
	}
    
private:
    ImVec2 CurrentWindowSize;
    
    Scene m_Scene;
    Serializer m_serializer = {&m_Scene};
    //Editor Panel
    ContentBrowser                  m_ContentBrowser;
    EntityList                      m_EntityList = { m_Scene };
    EntityProperties                m_EntityProperties = { m_Scene };
    MaterialEditor                  m_MaterialEditor = { m_Scene };
    BoneDebug                       m_BoneDebug = { m_Scene };
    RessourceManager                m_RessourceManager = { m_Scene };
    OutputWindow                    m_OutputWindow;

    
    bool cameraControlsActive = false;
    
    void CreateDockSpace();
    void DrawImGuiScene();
	void DrawGuizmos();

	EnvMap m_Envmap;
    Armageddon::CascadeShadow m_Cascade = { &Armageddon::Application::GetWindow()->GetRenderer().m_camera,m_Scene };
    Armageddon::Bloom m_bloom;

    Mesh m_quad = Armageddon::Renderer2D::GeneratePlane();

    Texture m_PlayButton;



};

Armageddon::Application* Armageddon::CreateApplication()
{
	return new Editor();
}
static float cameraSpeed = 0.3f;
void Editor::OnUpdate()
{
    if (cameraControlsActive)
    {
        if (Armageddon::Application::GetWindow()->GetKeyBoard().KeyIsPressed(AG_KEY_S))
        {
            //Armageddon::Application::GetWindow()->GetRenderer().m_camera.AdjustPosition(Armageddon::Application::GetWindow()->GetRenderer().m_camera.GetUpVec() * cameraSpeed);
            Armageddon::Application::GetWindow()->GetRenderer().m_camera.AdjustPosition(Armageddon::Application::GetWindow()->GetRenderer().m_camera.GetBackwardVec() * cameraSpeed);

        }
        if (Armageddon::Application::GetWindow()->GetKeyBoard().KeyIsPressed(AG_KEY_Space))
        {
            //Armageddon::Application::GetWindow()->GetRenderer().m_camera.AdjustPosition(Armageddon::Application::GetWindow()->GetRenderer().m_camera.GetUpVec() * cameraSpeed);
            Armageddon::Application::GetWindow()->GetRenderer().m_camera.AdjustPosition(Armageddon::Application::GetWindow()->GetRenderer().m_camera.GetUpVec() * cameraSpeed);

        }

        if (Armageddon::Application::GetWindow()->GetKeyBoard().KeyIsPressed(AG_KEY_Q))
        {
            Armageddon::Application::GetWindow()->GetRenderer().m_camera.AdjustPosition(Armageddon::Application::GetWindow()->GetRenderer().m_camera.GetLeftVec() * cameraSpeed);

        }
        if (Armageddon::Application::GetWindow()->GetKeyBoard().KeyIsPressed(AG_KEY_Z))
        {
            Armageddon::Application::GetWindow()->GetRenderer().m_camera.AdjustPosition(Armageddon::Application::GetWindow()->GetRenderer().m_camera.GetForwardVec() * cameraSpeed);
        }
        if (Armageddon::Application::GetWindow()->GetKeyBoard().KeyIsPressed(AG_KEY_D))
        {
            Armageddon::Application::GetWindow()->GetRenderer().m_camera.AdjustPosition(Armageddon::Application::GetWindow()->GetRenderer().m_camera.GetRightVec() * cameraSpeed);
        }
        if (Armageddon::Application::GetWindow()->GetKeyBoard().KeyIsPressed(AG_KEY_A))
        {
            Armageddon::Application::GetWindow()->GetRenderer().m_camera.AdjustRotation(0.0,0.1* cameraSpeed,0);
        }
       
    }
	if (Armageddon::Application::GetWindow()->GetKeyBoard().KeyIsPressed(AG_KEY_escape))
		EntityList::Seleceted = entt::null;
   
}


/*
* TODO am?liorer tout ?a pour l'instant c'est tr?s chaotique car si je veut rajouter des choses je suis obliger 
* de modifier le dll donc pas tr?s bon ! 
*/
void Editor::OnRender()
{
   // m_Scene.UpdateScene();


    m_Scene.m_SceneBuffer.Time = m_Scene.Scenetime;
    Armageddon::Renderer::g_WorldCBuffer.SetDynamicData(&m_Scene.m_SceneBuffer);
    Armageddon::Renderer::g_WorldCBuffer.BindVS();
    Armageddon::Renderer::g_WorldCBuffer.BindPS();

    Profiler m_CascadeTimer("GlobalPass");
    auto group = m_Scene.g_registry.group<LightComponent>(entt::get<TagComponent>);
    for (int i = 0 ; i < group.size();i++)
    {
        auto ent = group[i];
        auto entity = m_Scene.GetEntityByID(ent);
        auto& component = entity.GetComponent<LightComponent>();
        switch (component.type)
        {
        case 0:
        {
			Armageddon::Renderer::g_LightBufferData.PointLights[i].Color = component.m_pointLight.Color;
			Armageddon::Renderer::g_LightBufferData.PointLights[i].Position = component.m_pointLight.Position;
			Armageddon::Renderer::g_LightBufferData.PointLights[i].Intensity = component.m_pointLight.Intensity;
			Armageddon::Renderer::g_LightBufferData.PointLights[i].Radius = component.m_pointLight.Radius;
            break;
        }
        case 1:
        {
			Armageddon::Renderer::g_LightBufferData.DirectionalLights[i].Color = component.m_directionalLight.Color;
			Armageddon::Renderer::g_LightBufferData.DirectionalLights[i].Direction = component.m_directionalLight.Direction;
			Armageddon::Renderer::g_LightBufferData.DirectionalLights[i].Intensity = component.m_directionalLight.Intensity;
            break;
        }
        default:
            break;
        }


    }

	Armageddon::Renderer::g_LightBufferData.PointLightCount = Armageddon::Renderer::g_PointLightsVector.size();
    Armageddon::Renderer::g_LightBufferData.DirectionalLightCount = Armageddon::Renderer::g_DirectLightsVector.size();
 
    m_Cascade.CalculateMatrices();

	//Armageddon::Interface::GetDeviceContext()->OMSetRenderTargets(1, m_Cascade.m_CascadeLightTex.pTargetView.GetAddressOf(), nullptr);
  
    


    //Depth pass
    ID3D11ShaderResourceView* null[] = { nullptr, nullptr , nullptr , nullptr , nullptr , nullptr , nullptr , nullptr , nullptr , nullptr };
    Armageddon::Interface::GetDeviceContext()->PSSetShaderResources(0, 10, null);
    Armageddon::Interface::GetDeviceContext()->OMSetRenderTargets(1, &Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().m_DepthPass.RenderTargetView,nullptr);
    float color[] = { 0.1f,0.1f,0.1f,1.0f };
    Armageddon::Interface::GetDeviceContext()->ClearRenderTargetView(Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().m_DepthPass.RenderTargetView, color);

    
    for (auto iterator = m_Scene.EntityMap.begin(); iterator != m_Scene.EntityMap.cend(); iterator++)
    {
        if (iterator->second.HasComponent<MeshComponent>() && !iterator->second.HasComponent<LightComponent>())
        {

            auto& comp = iterator->second.GetComponent<MeshComponent>();
            if (iterator->second.HasComponent<TransformComponent>())
            {
                auto& transform = iterator->second.GetComponent<TransformComponent>();
                // comp.m_mesh.GetTransform()->WorldMat *= transform.GetTransformMatrix();

            }


            Armageddon::Renderer::g_WorldBufferData.Time += 0.01f;
            Armageddon::Renderer::g_WorldCBuffer.SetDynamicData(&Armageddon::Renderer::g_WorldBufferData);
            Armageddon::Renderer::g_WorldCBuffer.BindPS();
            Armageddon::Renderer::g_WorldCBuffer.BindVS();

            Armageddon::Renderer::g_TransformCBuffer.SetDynamicData(comp.m_mesh.GetTransform());
            Armageddon::Renderer::g_TransformCBuffer.BindPS();
            Armageddon::Renderer::g_TransformCBuffer.BindVS();

            m_Cascade.Update();

            Armageddon::Renderer::g_LightBufferData.LightViewProjection = m_Cascade.LightView * m_Cascade.LightProjection;
            Armageddon::Renderer::g_LightBufferData.CameraPos = Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().m_camera.GetPos();


            Armageddon::Renderer::g_LightCBuffer.SetDynamicData(&Armageddon::Renderer::g_LightBufferData);
            Armageddon::Renderer::g_LightCBuffer.BindPS();
            Armageddon::Renderer::g_LightCBuffer.BindVS();


            for (auto& submesh : comp.m_mesh.v_SubMeshes)
            {
                Armageddon::Interface::GetDeviceContext()->IASetInputLayout(m_Cascade.vx.GetInputLayout());
                Armageddon::Interface::GetDeviceContext()->PSSetShader(m_Cascade.px.GetShader(), nullptr, 0);
                Armageddon::Interface::GetDeviceContext()->VSSetShader(defaultVertexShader.GetShader(), nullptr, 0);

                submesh.BindIndexBuffer();
                submesh.BindVertexBuffer();
                submesh.DrawIndexed();
            }
        }
    }

    
    
    
    //HERE IS WHERE I Bind the RenderTargetView 




    m_Cascade.m_CascadeLightTex.Bind(Armageddon::Interface::GetDeviceContext().Get());
    m_Cascade.m_CascadeLightTex.Clear(Armageddon::Interface::GetDeviceContext().Get());
    Armageddon::Renderer::g_WorldCBuffer.BindVS();
    Armageddon::Renderer::g_WorldCBuffer.BindPS();
    Profiler Shadow("ShadowPass");
   
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = 1920;
    viewport.Height = 1080;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    Armageddon::Interface::GetDeviceContext()->RSSetViewports(1, &viewport);
    //I DRAW MY SCENE
    for (auto iterator = m_Scene.EntityMap.begin(); iterator != m_Scene.EntityMap.cend(); iterator++)
    {
		if (iterator->second.HasComponent<MeshComponent>() && !iterator->second.HasComponent<LightComponent>())
		{
            
			auto& comp = iterator->second.GetComponent<MeshComponent>();
			if (iterator->second.HasComponent<TransformComponent>())
			{
				auto& transform = iterator->second.GetComponent<TransformComponent>();
               // comp.m_mesh.GetTransform()->WorldMat *= transform.GetTransformMatrix();

			}
            

            Armageddon::Renderer::g_WorldBufferData.Time += 0.01f;
            Armageddon::Renderer::g_WorldCBuffer.SetDynamicData(&Armageddon::Renderer::g_WorldBufferData);
            Armageddon::Renderer::g_WorldCBuffer.BindPS();
            Armageddon::Renderer::g_WorldCBuffer.BindVS();

			Armageddon::Renderer::g_TransformCBuffer.SetDynamicData(comp.m_mesh.GetTransform());
			Armageddon::Renderer::g_TransformCBuffer.BindPS();
			Armageddon::Renderer::g_TransformCBuffer.BindVS();

			m_Cascade.Update();

			Armageddon::Renderer::g_LightBufferData.LightViewProjection = m_Cascade.LightView * m_Cascade.LightProjection;
			Armageddon::Renderer::g_LightBufferData.CameraPos = Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().m_camera.GetPos();


			Armageddon::Renderer::g_LightCBuffer.SetDynamicData(&Armageddon::Renderer::g_LightBufferData);
			Armageddon::Renderer::g_LightCBuffer.BindPS();
			Armageddon::Renderer::g_LightCBuffer.BindVS();


			for (auto& submesh : comp.m_mesh.v_SubMeshes)
			{

                Armageddon::Interface::GetDeviceContext()->IASetInputLayout(m_Cascade.vx.GetInputLayout());
                Armageddon::Interface::GetDeviceContext()->PSSetShader(m_Cascade.px.GetShader(),nullptr,0);
                Armageddon::Interface::GetDeviceContext()->VSSetShader(m_Cascade.vx.GetShader(),nullptr,0);
				submesh.BindIndexBuffer();
				submesh.BindVertexBuffer();
				submesh.DrawIndexed();
			}
		}
	}

     Shadow.~Profiler();


     Profiler FramePass("FrameBufferPass");

     Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().m_FrameBuffer.Bind(Armageddon::Interface::GetDeviceContext().Get());
     Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().m_FrameBuffer.Clear(Armageddon::Interface::GetDeviceContext().Get());
     Armageddon::Renderer::g_WorldCBuffer.BindVS();
     Armageddon::Renderer::g_WorldCBuffer.BindPS();
     for (auto iterator = m_Scene.EntityMap.begin(); iterator != m_Scene.EntityMap.cend(); iterator++)
     {
         if (iterator->second.HasComponent<MeshComponent>() && !iterator->second.HasComponent<LightComponent>())
         {

             auto& comp = iterator->second.GetComponent<MeshComponent>();
             if (iterator->second.HasComponent<TransformComponent>())
             {
                 auto& transform = iterator->second.GetComponent<TransformComponent>();
                 // comp.m_mesh.GetTransform()->WorldMat *= transform.GetTransformMatrix();

             }


             Armageddon::Renderer::g_TransformCBuffer.SetDynamicData(comp.m_mesh.GetTransform());
             Armageddon::Renderer::g_TransformCBuffer.BindPS();
             Armageddon::Renderer::g_TransformCBuffer.BindVS();



             Armageddon::Renderer::g_LightCBuffer.SetDynamicData(&Armageddon::Renderer::g_LightBufferData);
             Armageddon::Renderer::g_LightCBuffer.BindPS();
             Armageddon::Renderer::g_LightCBuffer.BindVS();

             Armageddon::Interface::GetDeviceContext()->PSSetShaderResources(50, 1, m_Envmap.m_convEnvMapTexture.GetRessourceViewPtr());
             Armageddon::Interface::GetDeviceContext()->PSSetShaderResources(51, 1, m_Envmap.m_PreFilteredEnvMap.GetRessourceViewPtr());
             Armageddon::Interface::GetDeviceContext()->PSSetShaderResources(52, 1, m_Envmap.m_BRFLutTexture.GetRessourceViewPtr());
             Armageddon::Interface::GetDeviceContext()->PSSetShaderResources(53, 1, m_Cascade.m_CascadeLightTex.DephtResourceView.GetAddressOf());

             for (auto& submesh : comp.m_mesh.v_SubMeshes)
             {

                 submesh.BindVertexBuffer();
                 submesh.BindIndexBuffer();
                 comp.m_mesh.BindMaterial(submesh.m_materialIndex);
                 submesh.DrawIndexed();
             }
         }
     }
     m_Envmap.Render(&Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().m_camera);
     FramePass.~Profiler();







     Profiler BloomTimer("BloomPass");
     Armageddon::Renderer::g_WorldCBuffer.BindVS();
     Armageddon::Renderer::g_WorldCBuffer.BindPS();
     m_bloom.m_BloomTexture.Bind(Armageddon::Interface::GetDeviceContext().Get());
     m_bloom.m_BloomTexture.Clear(Armageddon::Interface::GetDeviceContext().Get());

     D3D11_VIEWPORT aviewport;
     ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

     aviewport.TopLeftX = 0;
     aviewport.TopLeftY = 0;
     aviewport.Width = 1920;
     aviewport.Height = 1080;
     aviewport.MinDepth = 0.0f;
     aviewport.MaxDepth = 1.0f;

     Armageddon::Interface::GetDeviceContext()->RSSetViewports(1, &aviewport);
     Armageddon::Interface::GetDeviceContext()->PSSetSamplers(0, 1, Armageddon::Interface::GetSamplerState().GetAddressOf());
     Armageddon::Interface::GetDeviceContext()->PSSetShaderResources(0, 1, Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().m_FrameBuffer.GetAddressOfShaderRessource());

     for (auto& submesh : m_quad.v_SubMeshes)
     {
         Armageddon::Interface::GetDeviceContext()->IASetInputLayout(m_bloom.vx.GetInputLayout());
         Armageddon::Interface::GetDeviceContext()->PSSetShader(m_bloom.px.GetShader(), nullptr, 0);
         Armageddon::Interface::GetDeviceContext()->VSSetShader(m_bloom.vx.GetShader(), nullptr, 0);
        
         submesh.BindVertexBuffer();
         submesh.BindIndexBuffer();
         submesh.DrawIndexed();
     }
 
    // HERE I CHANGE THE RenderTargetView



	Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().GetOffScreenRenderTarget().Bind(Armageddon::Interface::GetDeviceContext().Get());
	Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().GetOffScreenRenderTarget().Clear(Armageddon::Interface::GetDeviceContext().Get());
    //I REDRAW MY SCENE WHERE I'M BINDING THE RESSOURCE
    Armageddon::Renderer::g_WorldCBuffer.BindVS();
    Armageddon::Renderer::g_WorldCBuffer.BindPS();
    m_bloom.Render();

    BloomTimer.~Profiler();


    Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().GetOffScreenRenderTarget().Bind(Armageddon::Interface::GetDeviceContext().Get());
    Armageddon::Renderer::g_WorldCBuffer.BindVS();
    Armageddon::Renderer::g_WorldCBuffer.BindPS();
    Profiler RenderPass("RenderPass");

    RenderScene(true);
    RenderPass.~Profiler();
    Profiler CombinePass("CombinePass");

    Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().FinalPass.Bind(Armageddon::Interface::GetDeviceContext().Get());
    Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().FinalPass.Clear(Armageddon::Interface::GetDeviceContext().Get());
    Armageddon::Renderer::g_WorldCBuffer.BindVS();
    Armageddon::Renderer::g_WorldCBuffer.BindPS();
    Armageddon::Interface::GetDeviceContext()->PSSetSamplers(0, 1, Armageddon::Interface::GetClampSampler().GetAddressOf());
    Armageddon::Interface::GetDeviceContext()->PSSetShaderResources(0, 1, Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().m_FrameBuffer.GetAddressOfShaderRessource());
    Armageddon::Interface::GetDeviceContext()->PSSetShaderResources(1, 1, m_bloom.m_bloomUpSample[0].GetRessourceViewPtr());
     
    m_bloom.BloomPropety.TexelSize = DirectX::XMFLOAT2(float(1.0F / Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().FinalPass.GetImageX()), float(1.0F / Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().FinalPass.GetImageY()));
    m_bloom.m_BloomConstant.SetDynamicData(&m_bloom.BloomPropety);
    m_bloom.m_BloomConstant.BindPS();
    for (auto& submesh : m_quad.v_SubMeshes)
    {
        Armageddon::Interface::GetDeviceContext()->IASetInputLayout(FinalPassVertex.GetInputLayout());
        Armageddon::Interface::GetDeviceContext()->PSSetShader(FinalPassPixel.GetShader(), nullptr, 0);
        Armageddon::Interface::GetDeviceContext()->VSSetShader(FinalPassVertex.GetShader(), nullptr, 0);
        submesh.BindVertexBuffer();
        submesh.BindIndexBuffer();
        submesh.DrawIndexed();
    }

	m_Envmap.Render(&Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().m_camera);
    
}

static float i; 
void Editor::ImGuiRender()
{
    
     CreateDockSpace();
     DrawImGuiScene();

    m_ContentBrowser.ImGuiDraw();
    m_EntityList.ImGuiDraw();
    m_EntityProperties.ImGuiDraw();
    m_MaterialEditor.ImGuiDraw();
    m_BoneDebug.ImGuiDraw();
    m_RessourceManager.ImGuiDraw();
    m_OutputWindow.ImGuiDraw();

    ImGui::Begin("Debug shadow");


    ImGui::Image(
        Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().FinalPass.GetShaderRessource()
		, { Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().FinalPass.GetImageX(),
      Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().FinalPass.GetImageY() });
      

   /** ImGui::Image(
        Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().gBuffer[0].GetRessourceView()
        , { Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().gBuffer[3].GetImageX(),Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().gBuffer[3].GetImageY() });
   
    ImGui::Image(
        Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().gBuffer[1].GetRessourceView()
        , { Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().gBuffer[3].GetImageX(),Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().gBuffer[3].GetImageY() });
    ImGui::Image(
        Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().gBuffer[2].GetRessourceView()
        , { Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().gBuffer[3].GetImageX(),Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().gBuffer[3].GetImageY() });
    /*
    ImGui::Image(
        Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().gBuffer[3].GetRessourceView()
        , { Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().gBuffer[3].GetImageX(),Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().gBuffer[3].GetImageY() });
   */

   /*ImGui::Image(
       m_Cascade.m_CascadeLightTex.DephtResourceView.Get()
		, { m_Cascade.m_CascadeLightTex.GetImageX(),
       m_Cascade.m_CascadeLightTex.GetImageY() });*/
   
    ImGui::End();
    
}

void Editor::OnInit()
{
    ImGui::SetCurrentContext(Armageddon::Application::GetWindow()->GetRenderer().GetImGuiContext());
    ImGuizmo::SetImGuiContext(Armageddon::Application::GetWindow()->GetRenderer().GetImGuiContext());
	Armageddon::Log::GetLogger()->trace("OnInit Event Reached");
    Armageddon::Application::GetWindow()->SetMouseCallBack(std::bind(&Editor::onMouseEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    Armageddon::Application::GetWindow()->setKeyBoardCallBack(std::bind(&Editor::onKeyBoardEvent, this, std::placeholders::_1));
  
    Armageddon::Application::GetWindow()->GetRenderer().m_camera.SetPosition(0.0f,0.0f,-1.0f);
    Armageddon::Application::GetWindow()->GetRenderer().m_camera.SetProjectionValues(90.0f,Armageddon::Application::GetWindow()->GetAspectRatio(),0.1f,10000.0f);

   
    Armageddon::Log::GetLogger()->trace("OnInit Event Reached");

    //m_Scene.LoadScene("Assets/Scenes/TestScene.mat");
    //m_serializer.ParseExperimentalMaterial("Assets/Materials/MaterialDeTeste.mat");
    m_serializer.DeserializeScene("Assets/Scenes/TestScene.mat");

   // Armageddon::Renderer*/

   // Mesh model = Mesh("..\\Armageddon Editor\\Assets\\Models\\suzanne.obj");

   /* Mesh model = Mesh("..\\Armageddon Editor\\Assets\\Models\\Nature Kit\\Models\\OBJ format\\canoe.obj");
    model.SetTransform(&Armageddon::Application::GetWindow()->GetRenderer().m_camera);
   Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().SubmitMesh(model);*/

   /*auto entity = m_Scene.CreateEntity();
   entity.AddComponent<TagComponent>("test");
   entity.AddComponent<MeshComponent>("..\\Armageddon Editor\\Assets\\Models\\Nature Kit\\Models\\OBJ format\\canoe.obj");*/

    
}
static float CameraZoom = 1.0f;

void Editor::onMouseEvent(MouseEvent::MEventType e, float x, float y)
{
    if (cameraControlsActive)
    {
        switch (e)
        {
        case  MouseEvent::MEventType::Wheeldown:
            CameraZoom -= 0.1f;
            Armageddon::Application::GetWindow()->GetRenderer().m_camera.AdjustPosition(Armageddon::Application::GetWindow()->GetRenderer().m_camera.GetBackwardVec() * cameraSpeed);

            break;
        case  MouseEvent::MEventType::Wheelup:
            CameraZoom += 0.1f;
            Armageddon::Application::GetWindow()->GetRenderer().m_camera.AdjustPosition(Armageddon::Application::GetWindow()->GetRenderer().m_camera.GetForwardVec() * cameraSpeed);


            break;

        case MouseEvent::MEventType::Move:

            Armageddon::Application::GetWindow()->GetRenderer().m_camera.AdjustRotation(y * -0.01f, x * 0.01f, 0.0f);
            break;
        }
    }
}

void Editor::onKeyBoardEvent(const unsigned char keyCode)
{

    if (Armageddon::Application::GetWindow()->GetKeyBoard().KeyIsPressed(AG_KEY_ctrl))
    {
        if (!Armageddon::Application::GetWindow()->GetKeyBoard().KeyIsPressed(AG_KEY_D) && keyCode == AG_KEY_D)
        {
            if (EntityList::Seleceted != entt::null)
            {
               // m_Scene.DuplicateEntity(m_Scene.GetEntityByID(EntityList::Seleceted));
            }
        }

    }
    if ( keyCode == AG_KEY_delete) 
    {
        if (EntityList::Seleceted != entt::null) {
            m_Scene.DeleteEntity(m_Scene.GetEntityByID(EntityList::Seleceted));
            EntityList::Seleceted == entt::null;
        }
    }
    if (keyCode == AG_KEY_O)
    {
        
    }
}

void Editor::RenderScene(bool BindMat)
{
    Armageddon::Interface::GetDeviceContext()->RSSetViewports(1, &Armageddon::Renderer::ViewPort);

    for (auto iterator = m_Scene.EntityMap.begin(); iterator != m_Scene.EntityMap.cend(); iterator++)
    {

        //Armageddon::Interface::GetDeviceContext()->CSSetShader()
       /*//* if (ent.HasComponent<RigidBodyComponent>())
        {
           auto& component = ent.GetComponent<RigidBodyComponent>();
           component.update();
        }*/
		if (iterator->second.HasComponent<MeshComponent>())
		{
			auto& component = iterator->second.GetComponent<MeshComponent>();
			if (!component.m_mesh.IsEmpty()) {
				component.m_mesh.UpdtateTransform(&Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().m_camera);
				if (iterator->second.HasComponent<LightComponent>())
				{
					auto& Lightcomponent = iterator->second.GetComponent<LightComponent>();

					component.m_mesh.GetTransform()->WorldMat *= DirectX::XMMatrixTranslation(Lightcomponent.m_pointLight.Position.x, Lightcomponent.m_pointLight.Position.y, Lightcomponent.m_pointLight.Position.z);

				}
				if (iterator->second.HasComponent<TransformComponent>())
				{
					auto& transform = iterator->second.GetComponent<TransformComponent>();
					component.m_mesh.GetTransform()->WorldMat *= transform.GetTransformMatrix();
				}
				if (component.m_mesh.m_skeleton.m_JointsCount > 0)
				{
					for (UINT i = 0; i < component.m_mesh.m_skeleton.m_JointsCount; i++)
					{
						m_quad.UpdtateTransform(&Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().m_camera);

                        m_quad.GetTransform()->WorldMat *= component.m_mesh.m_skeleton.m_aJoints[i].m_inverseBlindPose;

						Armageddon::Renderer::g_TransformCBuffer.SetDynamicData(m_quad.GetTransform());
						Armageddon::Renderer::g_TransformCBuffer.BindPS();
						Armageddon::Renderer::g_TransformCBuffer.BindVS();

                        m_quad.v_SubMeshes[0].BindVertexBuffer();
                        m_quad.v_SubMeshes[0].BindIndexBuffer();
                        m_quad.v_SubMeshes[0].DrawIndexed();
					}
				}


				Armageddon::Renderer::g_TransformCBuffer.SetDynamicData(component.m_mesh.GetTransform());
				Armageddon::Renderer::g_TransformCBuffer.BindPS();
				Armageddon::Renderer::g_TransformCBuffer.BindVS();

				Armageddon::Renderer::g_LightBufferData.CameraPos = Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().m_camera.GetPos();
				Armageddon::Renderer::g_LightBufferData.LightViewProjection = m_Cascade.LightView * m_Cascade.LightProjection;
	


				Armageddon::Renderer::g_LightCBuffer.SetDynamicData(&Armageddon::Renderer::g_LightBufferData);
				Armageddon::Renderer::g_LightCBuffer.BindPS();
				Armageddon::Renderer::g_LightCBuffer.BindVS();



				Armageddon::Interface::GetDeviceContext()->PSSetShaderResources(6, 1, m_Envmap.m_convEnvMapTexture.GetRessourceViewPtr());
				Armageddon::Interface::GetDeviceContext()->PSSetShaderResources(7, 1, m_Envmap.m_PreFilteredEnvMap.GetRessourceViewPtr());
				Armageddon::Interface::GetDeviceContext()->PSSetShaderResources(8, 1, m_Envmap.m_BRFLutTexture.GetRessourceViewPtr());
				Armageddon::Interface::GetDeviceContext()->PSSetShaderResources(9, 1, m_Cascade.m_CascadeLightTex.DephtResourceView.GetAddressOf());
                

                
				//TEST DEBUG 
                if (iterator->second.HasComponent<TagComponent>())
                {
                    auto& tag = iterator->second.GetComponent<TagComponent>();
                }
				// component.m_mesh.BindShaders();
                
                for (auto& Submesh : component.m_mesh.v_SubMeshes)
                {

                    Submesh.BindVertexBuffer();
                    Submesh.BindIndexBuffer();
                    component.m_mesh.BindMaterial(Submesh.m_materialIndex);
                    Submesh.DrawIndexed();
                }


			}
		}
	}
  //  Armageddon::PhysicsInterface::g_PhysxScene->simulate(0.01);
  //  Armageddon::PhysicsInterface::g_PhysxScene->fetchResults(true);
    m_Envmap.Render(&Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().m_camera);
}


void Editor::CreateDockSpace()
{
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", 0, window_flags);
	ImGui::PopStyleVar();


	ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
	}


	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{

			if (ImGui::MenuItem("Save Scene", ""))
			{
                
                for (auto iterator = m_Scene.EntityMap.begin(); iterator != m_Scene.EntityMap.cend(); iterator++)
                {
                    if (iterator->second.HasComponent<TagComponent>())
                    {
                        Armageddon::Log::GetLogger()->trace("TAG COMOPONENENENEN");

                    }
                    if (iterator->second.HasComponent<MeshComponent>())
                    {
                    }
                    if (iterator->second.HasComponent<TransformComponent>())
                    {
                        Armageddon::Log::GetLogger()->trace("qsdqsdqsdqsdqsdqsdqsd");
                    }

                    m_serializer.SerializeScene("Assets/Scenes/TestScene.mat", iterator->second,m_Scene.g_registry);
                }
               // m_Scene.g_registry.data
                Armageddon::Log::GetLogger()->trace(m_Scene.g_registry.capacity<TransformComponent>());
                Armageddon::Log::GetLogger()->trace(m_Scene.g_registry.capacity<TagComponent>());
            }

			ImGui::Separator();

			ImGui::EndMenu();
		}


		ImGui::EndMenuBar();
	}

	ImGui::End();



	ImGui::Begin("Debug");
   // ImGui::DragFloat("Rougness", &m_PBRBUFFER.Roughness, 0.01, 0, 1);
   // ImGui::DragFloat("Metalic", &m_PBRBUFFER.Metalic, 0.01, 0, 1);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    for (auto iterator = ProfilerData::m_profilerData.begin(); iterator != ProfilerData::m_profilerData.cend(); iterator++)
    {
        ImGui::Text("%s : %.3f ms", iterator->first.c_str(), iterator->second);

    }
    ProfilerData::Clear();
	ImGui::End();
}

void Editor::DrawImGuiScene()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f,0.0f});
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, { 0.0f,0.0f });
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0862745098f, 0.0862745098f, 0.0862745098f, 1.0f));

    

    ImGui::Begin("##SceneToolBar", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse);
    ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5) - (ImGui::GetWindowHeight() - 4.0f * 0.5f));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4( 35/255, 35/255, 35/255 ,1.0f));
    if (ImGui::ImageButton(m_PlayButton.GetRessourceView(), { 20,20 }))
    {
        if (m_Scene.m_SceneState == SceneState::Editor)
        {
            m_Scene.m_SceneState = SceneState::Runtime;
            ScriptEngine::Init(&m_Scene);

        }
        if (m_Scene.m_SceneState == SceneState::Runtime)
        {
        }
    }
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(2);

    ImGui::End();


    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f,0.0f });

    ImGui::Begin("Scene", 0);
    ImGui::PopStyleVar(3);
    DrawGuizmos();
    ImVec2 WindowSize = ImGui::GetWindowSize();
    ImVec2 vMin = ImGui::GetWindowContentRegionMin();
    ImVec2 vMax = ImGui::GetWindowContentRegionMax();

    vMin.x += ImGui::GetWindowPos().x;
    vMin.y += ImGui::GetWindowPos().y;
    vMax.x += ImGui::GetWindowPos().x;
    vMax.y += ImGui::GetWindowPos().y;
    if ((ImGui::IsWindowFocused() || ImGui::IsWindowHovered()) && GetAsyncKeyState(AG_KEY_shift) && GetAsyncKeyState(AG_KEY_ctrl))
    {

        cameraControlsActive = true;
    }
    else
    {

        cameraControlsActive = false;

    }
    if (CurrentWindowSize.x != WindowSize.x || CurrentWindowSize.y != WindowSize.y)
    {
		const ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
       if (viewportPanelSize.x / viewportPanelSize.y > 0.0f) {
            /* on resize que le offscreen render target view*/
           Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().GetOffScreenRenderTarget().ResizeRenderTargetView(
                vMax.x - vMin.x, vMax.y - vMin.y,
                nullptr);
          Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().CreateViewPort(vMax.x - vMin.x, vMax.y - vMin.y);
         // Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().ResizeBuffer(vMax.x - vMin.x, vMax.y - vMin.y);
		    Armageddon::Application::GetWindow()->GetRenderer().m_camera.SetProjectionValues(90.0f, viewportPanelSize.x / viewportPanelSize.y, 0.1f, 10000.0f);

            CurrentWindowSize = ImGui::GetWindowSize();
        }
    }



    ImGui::Image(
        Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().GetOffScreenRenderTarget().GetShaderRessource()
        , { Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().GetOffScreenRenderTarget().GetImageX(),
        Armageddon::Application::GetApplicationInsatnce()->GetWindow()->GetRenderer().GetOffScreenRenderTarget().GetImageY() });


    ImGui::End();
}

void Editor::DrawGuizmos()
{

   ImGuizmo::SetOrthographic(false);
    ImGuizmo::Enable(true);
	ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());

    if (EntityList::Seleceted != entt::null)
    {
        auto entity = m_Scene.GetEntityByID(EntityList::Seleceted);
        if (entity.HasComponent<TransformComponent>())
        {
            auto& component = entity.GetComponent<TransformComponent>();
            ImVec2 vMin = ImGui::GetWindowContentRegionMin();
            ImVec2 vMax = ImGui::GetWindowContentRegionMax();

			vMin.x += ImGui::GetWindowPos().x;
			vMin.y += ImGui::GetWindowPos().y;
			vMax.x += ImGui::GetWindowPos().x;
			vMax.y += ImGui::GetWindowPos().y;
            DirectX::XMFLOAT4X4 ViewMat;
            DirectX::XMFLOAT4X4 ProjMat;
            DirectX::XMFLOAT4X4 TransformMat;
            
            DirectX::XMStoreFloat4x4(&ViewMat, Armageddon::Application::GetWindow()->GetRenderer().m_camera.GetViewMatrix());
			DirectX::XMStoreFloat4x4(&ProjMat, Armageddon::Application::GetWindow()->GetRenderer().m_camera.GetProjectionMatrix());
			DirectX::XMStoreFloat4x4(&TransformMat, component.GetTransformMatrix());

            ImGuizmo::SetRect(vMin.x, vMin.y, vMax.x - vMin.x, vMax.y - vMin.y);
           // ImGui::GetForegroundDrawList()->AddRect(vMin, vMax, IM_COL32(255, 255, 0, 255));
            if (ImGuizmo::Manipulate(*ViewMat.m, *ProjMat.m, ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL, *TransformMat.m))
            {
				component.Translation.x = TransformMat.m[3][0];
				component.Translation.y = TransformMat.m[3][1];
				component.Translation.z = TransformMat.m[3][2];
            }
        }
    }
}
