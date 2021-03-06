#include "Material.h"
#include "../Renderer/Renderer.h"



Material::Material(const std::string& MaterialName) : m_usedMaterialName(m_usedMaterialName)
{
	 m_AssetName = MaterialName;
	 m_AssetType = AssetType::MaterialAsset;
	 Armageddon::Log::GetLogger()->error("POUF");
	 m_albedoMap					 = AssetManager::GetOrCreateTexture( "Ressources//Textures//DefaultAlbedo.png" );
	 m_normalMap					 = AssetManager::GetOrCreateTexture( "Ressources//Textures//DefaultNormal.tiff" );
	 m_specularMap					 = AssetManager::GetOrCreateTexture( "Ressources//Textures//DefaultAlbedo.png" );
	 m_ambiantOcclusionMap			 = AssetManager::GetOrCreateTexture( "Ressources//Textures//DefaultAlbedo.png" );

	 m_VertexShader					 = AssetManager::GetOrCreateVertexShader(L"..\\bin\\Debug-x64\\Armageddon 2.0\\DefaultVertexShader.cso");
	 m_PixelShader					 = AssetManager::GetOrCreatePixelShader(L"..\\bin\\Debug-x64\\Armageddon 2.0\\PBR.cso");
	 m_EmissiveMap					 = AssetManager::GetOrCreateTexture("Ressources//Textures//DefaultAlbedo.png");

	 
	 AssetManager::m_AssetMap[HashUtils::_64BitHash(MaterialName)] = *this;
	 AssetManager::m_MaterialMap[HashUtils::_64BitHash(MaterialName)] = *this;
	

}

Material::Material(Texture& m_albedoMap, Texture& m_normalMap, Texture& m_specularMap, Texture& m_ambiantOcclusionMap)
{
	this->m_albedoMap = m_albedoMap;
	this->m_normalMap = m_normalMap;
	this->m_specularMap = m_specularMap;
	this->m_ambiantOcclusionMap = m_ambiantOcclusionMap;
}
Material::~Material()
{
	free(TestConstBuffer);
}



void Material::SetAlbedoMap(const std::filesystem::path& m_albedoPath)
{
	m_albedoMap = AssetManager::GetOrCreateTexture(m_albedoPath);
	
}

void Material::SetNormalMap(const std::filesystem::path& m_NormalPath)
{
	m_normalMap = AssetManager::GetOrCreateTexture(m_NormalPath);

}

void Material::SetSpecularMap(const std::filesystem::path& m_SpecularPath)
{
	m_specularMap = AssetManager::GetOrCreateTexture(m_SpecularPath);
}

void Material::SetMetalicMap(const std::filesystem::path& m_MetalicPath)
{
	m_metalicMap = AssetManager::GetOrCreateTexture(m_MetalicPath);

}

void Material::SetEmisiveMap(const std::filesystem::path& emissivePath)
{
	m_EmissiveMap = AssetManager::GetOrCreateTexture(emissivePath);

}

void Material::SetAOMap(const std::filesystem::path& m_AOPath)
{
	m_ambiantOcclusionMap = AssetManager::GetOrCreateTexture(m_AOPath);

}

void Material::SetVertexShader(const std::filesystem::path& ShaderPath)
{
	m_VertexShader = AssetManager::GetOrCreateVertexShader(ShaderPath);
}

void Material::SetPixelShader(const std::filesystem::path& ShaderPath)
{
	m_PixelShader = AssetManager::GetOrCreatePixelShader(ShaderPath);

}

void Material::BindRessources()
{
	

	Armageddon::Renderer::g_PBRCBuffer.SetDynamicData(&m_PBRBUFFER);
	Armageddon::Renderer::g_PBRCBuffer.BindPS();
	Armageddon::Renderer::g_PBRCBuffer.BindVS();



	Armageddon::Interface::GetDeviceContext()->IASetInputLayout(m_VertexShader.GetInputLayout());
	Armageddon::Interface::GetDeviceContext()->PSSetSamplers(0, 1, Armageddon::Interface::GetSamplerState().GetAddressOf());
	Armageddon::Interface::GetDeviceContext()->PSSetSamplers(1, 1, Armageddon::Interface::GetClampSampler().GetAddressOf());

	for (size_t i = 0; i < m_Textures.size(); i++)
	{
		Armageddon::Interface::GetDeviceContext()->PSSetShaderResources(i, 1, m_Textures[i].GetRessourceViewPtr());
		Armageddon::Interface::GetDeviceContext()->VSSetShaderResources(i, 1, m_Textures[i].GetRessourceViewPtr());
	}
}

Material::Material(Texture* m_albedoMap, Texture* m_normalMap, Texture* m_specularMap, Texture* m_metalicMap, Texture* m_ambiantOcclusionMap)
{
	
}

void Material::BindShaders()
{
	/*
	*  TODO MIEUX OIT
	*/
	if (RenderMode == 1)
	{
		float blendFactor[] = { 0.0f,0.0f,0.0f,0.0f };
		Armageddon::Interface::GetDeviceContext()->OMSetBlendState(Armageddon::Renderer::AlphaBlendState.Get(), blendFactor, 0xffffffff);
	}
	else
	{
		Armageddon::Interface::GetDeviceContext()->OMSetBlendState(0, 0, 0xffffffff);

	}


	Armageddon::Interface::GetDeviceContext()->VSSetShader(m_VertexShader.GetShader(), NULL, 0);
	Armageddon::Interface::GetDeviceContext()->PSSetShader(m_PixelShader.GetShader(), NULL, 0);



	/*for (UINT i = 0; i < m_MaterialProperty.m_VTexure.size(); i++)
	{
		Armageddon::Interface::GetDeviceContext()->PSSetShaderResources(i, 1, m_MaterialProperty.m_VTexure[0].m_Texture.GetRessourceViewPtr());
	}*/
}
	