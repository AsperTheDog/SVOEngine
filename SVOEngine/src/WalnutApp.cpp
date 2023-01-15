#include <iostream>
#include <stack>

#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "../SVO.hpp"
#include "../ColorFunctions.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/intersect.hpp>

#include <fstream>

#define SUBSAMPLE 0

glm::vec3 homogenize(const glm::vec4& p)
{
	return {p * (1.f / p.w)};
}

class ExampleLayer : public Walnut::Layer
{
public:
	virtual void OnAttach() override
	{
		ColorFunctions::setRandSeed(0x1234142);
		this->svo.populate(1);

		camPos = glm::vec3(1600, 1500, 1600);
		camDir = glm::normalize(glm::vec3(-1, -1, -1));
		
		assert(NodeIndexer::test());
	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("Raytracing info");
		if (ImGui::Button("render"))
		{
			Render();
		}
		ImGui::DragFloat3("Camera position", &camPos.x, 1.f);
		ImGui::DragFloat3("Camera direction", &camDir.x, 0.001f);
		ImGui::DragInt("LOD level", &svo.LODLevel, 10, 0, 5000);
		ImGui::End();

		ImGui::Begin("Viewport");

		//width = (uint32_t)ImGui::GetContentRegionAvail().x >> SUBSAMPLE;
		//height = (uint32_t)ImGui::GetContentRegionAvail().y >> SUBSAMPLE;
		width = 2560;
		height = 1440;
		if (image)
			ImGui::Image(image->GetDescriptorSet(), { 
				(float)(image->GetWidth() << SUBSAMPLE),
				(float)(image->GetHeight() << SUBSAMPLE)
			});

		ImGui::End();

		V = glm::lookAt(camPos, camPos + camDir, glm::vec3(0, 1, 0));
		P = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.01f, 100.0f);
	}

private:
	glm::u8vec4* imageData = nullptr;
	std::shared_ptr<Walnut::Image> image;
	uint32_t width = 0, height = 0;
	glm::mat4 V{}, P{};
	glm::vec3 camPos{}, camDir{};

	SVO svo{10, 9000000000, ColorFunctions::ambientOcclusionEffect};

	inline uint32_t getIndexFromCoord(uint32_t x, uint32_t y)
	{
		return x + (y * width);
	}

	void Render()
	{
		if (height == 0 || width == 0) return;

		if (!image || width != image->GetWidth() || height != image->GetHeight())
		{
			image = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
			delete[] imageData;
			imageData = new glm::u8vec4[width * height];
		}

		glm::vec3 camera_pos = glm::vec3(glm::inverse(V) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		auto dirMat = glm::inverse(P * V);

#ifndef _DEBUG
#pragma omp parallel for
#endif
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				glm::u8vec3 newColor = glm::u8vec3(0);

				glm::vec2 screenCoord = glm::vec2((float)x / (float)width, (float)y / (float)height);
				glm::vec4 viewCoord = glm::vec4(screenCoord.x * 2.0f - 1.0f, screenCoord.y * 2.0f - 1.0f, 1.0f, 1.0f);
				glm::vec3 p = homogenize(dirMat * viewCoord);

				Ray newRay = {};
				newRay.origin = camera_pos;
				newRay.direction = glm::normalize(p - camera_pos);
				newRay.t = FLT_MAX;

				//std::cout << "Testing ray with X: " << x << ", Y:" << y << "\n";
				if (svo.intersectRay(&newRay, 1))
				//if(testSphere(&newRay, {0, 0, 0}, 2))
				{
					newColor = newRay.color;
				}
				imageData[getIndexFromCoord(x, y)] = glm::u8vec4(newColor, 255);
			}
		}

		image->SetData(imageData);
		
		//std::fstream imgout("MyImage.bin", std::fstream::out | std::fstream::binary);
		//imgout.write(reinterpret_cast<char*>(imageData), width * height * 4);
		//imgout.close();
	}
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Voxel engine";
	
	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});

	return app;
}