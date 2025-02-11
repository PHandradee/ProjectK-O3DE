
#include "EditorFlipBookComponent.h"

#include <AzCore/Debug/Trace.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>

#include <AzToolsFramework/UI/PropertyEditor/PropertyEditorAPI.h>

namespace ProjectK {
	void EditorFlipBookComponent::Reflect(AZ::ReflectContext* context) {
		if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context)) {
			serializeContext->Class<EditorFlipBookComponent, AZ::Component>()
				->Version(1)
				->Field("Sprite", &EditorFlipBookComponent::m_sprite)
				->Field("TotalRows", &EditorFlipBookComponent::m_totalRows)
				->Field("TotalColumns", &EditorFlipBookComponent::m_totalColumns)
				->Field("UV Center", &EditorFlipBookComponent::m_materialUVcenter)
				->Field("Sprite Rotation", &EditorFlipBookComponent::m_materialUVrotate)
				->Field("Sprite Scale", &EditorFlipBookComponent::m_materialUVscale)
				->Field("SpriteIndex", &EditorFlipBookComponent::m_spriteIndex)
				->Field("Start Frame", &EditorFlipBookComponent::m_startFrame)
				->Field("End Frame", &EditorFlipBookComponent::m_endFrame)
				->Field("Loop Start Frame", &EditorFlipBookComponent::m_loopStartFrame)
				->Field("Loop Type", &EditorFlipBookComponent::m_loopType)
				->Field("Framerate Unit", &EditorFlipBookComponent::m_framerateUnit)
				->Field("Framerate", &EditorFlipBookComponent::m_framerate)
				->Field("Start Delay", &EditorFlipBookComponent::m_startDelay)
				->Field("Loop Delay", &EditorFlipBookComponent::m_loopDelay)
				->Field("Reverse Delay", &EditorFlipBookComponent::m_reverseDelay)
				->Field("Auto Play", &EditorFlipBookComponent::m_isAutoPlay)
				->Field("DebugEnabled", &EditorFlipBookComponent::m_debugEnabled)
				->Field("Preview In Editor", &EditorFlipBookComponent::m_previewInEditor);

			if (AZ::EditContext* editContext = serializeContext->GetEditContext()) {
				editContext
					->Class<EditorFlipBookComponent>(
						"FlipBookComponent",
						"This component Render a sprite on the mesh specified by the mesh component and plays a spritesheet animation")
					->ClassElement(AZ::Edit::ClassElements::EditorData, "")
					->Attribute(AZ::Edit::Attributes::Category, "CustomComponents")
					->Attribute(AZ::Edit::Attributes::Icon,
						"Icons/Components/Component_Placeholder.svg")
					->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu,
						AZ_CRC_CE("Game"))
					->Attribute(AZ::Edit::Attributes::AutoExpand, true)
					->DataElement(AZ::Edit::UIHandlers::Default,
						&EditorFlipBookComponent::m_sprite, "Sprite",
						"The sprite asset to render")
					->Attribute(AZ::Edit::Attributes::ChangeNotify,
						&EditorFlipBookComponent::OnSpriteAssetChanged)
					->DataElement(AZ::Edit::UIHandlers::Default,
						&EditorFlipBookComponent::m_totalRows, "Total Rows",
						"Total of spritesheet rows")
					->Attribute(AZ::Edit::Attributes::ChangeNotify,
						&EditorFlipBookComponent::OnTotalRowsChanged)
					->DataElement(AZ::Edit::UIHandlers::Default,
						&EditorFlipBookComponent::m_totalColumns,
						"Total Columns",
						"Total of spritesheet columns")
					->Attribute(AZ::Edit::Attributes::ChangeNotify,
						&EditorFlipBookComponent::OnTotalColumnsChanged)
					->DataElement(AZ::Edit::UIHandlers::Default,
						&EditorFlipBookComponent::m_materialUVrotate,
						"Sprite Rotation",
						"Sprite rotation angle on degrees")
					->Attribute(AZ::Edit::Attributes::ChangeNotify,
						&EditorFlipBookComponent::OnMaterialUVRotateChanged)
					->DataElement(AZ::Edit::UIHandlers::Default,
						&EditorFlipBookComponent::m_materialUVscale,
						"Sprite Scale",
						"Sprite scale factor")
					->Attribute(AZ::Edit::Attributes::ChangeNotify,
						&EditorFlipBookComponent::OnMaterialUVScaleChanged)
					->DataElement(AZ::Edit::UIHandlers::ComboBox,
						&EditorFlipBookComponent::m_spriteIndex, "Sprite Index",
						"Actual sprite index on spritesheet")
					->Attribute(AZ::Edit::Attributes::EnumValues,
						&EditorFlipBookComponent::PopulateIndexList)
					->Attribute(AZ::Edit::Attributes::ChangeNotify,
						&EditorFlipBookComponent::OnSpriteIndexChanged)
					->DataElement(AZ::Edit::UIHandlers::ComboBox,
						&EditorFlipBookComponent::m_startFrame, "Start Frame",
						"Start frame of animation")
					->Attribute(AZ::Edit::Attributes::EnumValues,
						&EditorFlipBookComponent::PopulateIndexList)
					->DataElement(AZ::Edit::UIHandlers::ComboBox,
						&EditorFlipBookComponent::m_endFrame, "End Frame",
						"End frame of animation")
					->Attribute(AZ::Edit::Attributes::EnumValues,
						&EditorFlipBookComponent::PopulateIndexList)
					->DataElement(AZ::Edit::UIHandlers::ComboBox,
						&EditorFlipBookComponent::m_loopStartFrame,
						"Loop start frame", "Frame to start looping from")
					->Attribute("EnumValues",
						&EditorFlipBookComponent::PopulateConstrainedIndexList)
					->DataElement(AZ::Edit::UIHandlers::ComboBox,
						&EditorFlipBookComponent::m_loopType, "Loop type",
						"Go from start to end continuously or start to end and "
						"back to start")
					->EnumAttribute(FlipBookComponent::LoopType::None, "None")
					->EnumAttribute(FlipBookComponent::LoopType::Linear, "Linear")
					->EnumAttribute(FlipBookComponent::LoopType::PingPong, "PingPong")
					->Attribute(AZ::Edit::Attributes::ChangeNotify,
						AZ_CRC_CE("RefreshEntireTree"))
					->DataElement(AZ::Edit::UIHandlers::ComboBox,
						&EditorFlipBookComponent::m_framerateUnit,
						"Framerate unit", "Unit of measurement for framerate")
					->EnumAttribute(FlipBookComponent::FramerateUnits::FPS, "FPS")
					->EnumAttribute(FlipBookComponent::FramerateUnits::SecondsPerFrame,
						"Seconds Per Frame")
					->Attribute(AZ::Edit::Attributes::ChangeNotify,
						&EditorFlipBookComponent::OnFramerateUnitChange)
					->Attribute(AZ::Edit::Attributes::ChangeNotify,
						AZ_CRC_CE("RefreshEntireTree"))
					->DataElement(0, &EditorFlipBookComponent::m_framerate, "Framerate",
						"Determines transition speed between frames")
					->Attribute(AZ::Edit::Attributes::Min, 0.0f)
					->Attribute(AZ::Edit::Attributes::Max, AZ::Constants::FloatMax)
					->DataElement(0, &EditorFlipBookComponent::m_startDelay,
						"Start delay",
						"Number of seconds to wait before playing the flipbook "
						"(applied only once).")
					->Attribute(AZ::Edit::Attributes::Min, 0.0f)
					->Attribute(AZ::Edit::Attributes::Max, AZ::Constants::FloatMax)
					->DataElement(
						0, &EditorFlipBookComponent::m_loopDelay, "Loop delay",
						"Number of seconds to delay until the loop sequence plays")
					->Attribute(AZ::Edit::Attributes::Visibility,
						&EditorFlipBookComponent::IsLoopingType)
					->Attribute(AZ::Edit::Attributes::Min, 0.0f)
					->Attribute(AZ::Edit::Attributes::Max, AZ::Constants::FloatMax)
					->DataElement(0, &EditorFlipBookComponent::m_reverseDelay,
						"Reverse delay",
						"Number of seconds to delay until the reverse sequence "
						"plays (PingPong loop types only)")
					->Attribute(AZ::Edit::Attributes::Visibility,
						&EditorFlipBookComponent::IsPingPongLoopType)
					->Attribute(AZ::Edit::Attributes::Min, 0.0f)
					->Attribute(AZ::Edit::Attributes::Max, AZ::Constants::FloatMax)
					->DataElement(0, &EditorFlipBookComponent::m_isAutoPlay, "Auto Play",
						"Automatically starts playing the animation")
					->DataElement(AZ::Edit::UIHandlers::Default,
						&EditorFlipBookComponent::m_debugEnabled,
						"Enable Debug", "Debug infos")
					->DataElement(AZ::Edit::UIHandlers::Default,
						&EditorFlipBookComponent::m_previewInEditor,
						"Preview In Editor", "Play the Animation on editor")
					->Attribute(AZ::Edit::Attributes::ChangeNotify,
						&EditorFlipBookComponent::OnPreviewInEditorChanged);
			}
		}
	}
	void EditorFlipBookComponent::Init() {}
	void EditorFlipBookComponent::Activate() {
		SetSprite(m_sprite);

		bool materialAssignemtnSucessReturnStatus{ false };
		UpdateMaterialAssignmentId(materialAssignemtnSucessReturnStatus);
		if (materialAssignemtnSucessReturnStatus == false) {
			AZ_Error("EditorFlipBookComponent", true, "Failed to get the Material ID");
			return;
		}
	}
	void EditorFlipBookComponent::Deactivate() {
		// FlipBookRequestBus::Handler::BusDisconnect(GetEntityId());
		AZ::Data::AssetBus::Handler::BusDisconnect();
		AZ::TickBus::Handler::BusDisconnect();
	}
	void EditorFlipBookComponent::BuildGameEntity(AZ::Entity* gameEntity) {
		
		gameEntity->CreateComponent<FlipBookComponent>();
		
		FlipBookComponent* const flipBookComponent{ gameEntity->FindComponent<FlipBookComponent>() };

		if (flipBookComponent == nullptr) {
			AZ_Warning("EditorFlipBookComponent", false, "Failed to Build the game entity");
			return;
		}

		// Serialized members
		flipBookComponent->m_materialUVcenter = m_materialUVcenter;
		flipBookComponent->m_materialUVrotate = m_materialUVrotate;
		flipBookComponent->m_materialUVscale = m_materialUVscale;
		flipBookComponent->m_previewInEditor = m_previewInEditor;
		flipBookComponent->m_sprite = m_sprite;
		flipBookComponent->m_debugEnabled = m_debugEnabled;
		flipBookComponent->m_spriteIndex = m_spriteIndex;
		flipBookComponent->m_totalColumns = m_totalColumns;
		flipBookComponent->m_totalRows = m_totalRows;
		flipBookComponent->m_startFrame = m_startFrame;
		flipBookComponent->m_endFrame = m_endFrame;
		flipBookComponent->m_loopStartFrame = m_loopStartFrame;
		flipBookComponent->m_loopType = m_loopType;
		flipBookComponent->m_framerate = m_framerate;
		flipBookComponent->m_startDelay = m_startDelay;
		flipBookComponent->m_loopDelay = m_loopDelay;
		flipBookComponent->m_reverseDelay = m_reverseDelay;
		flipBookComponent->m_isAutoPlay = m_isAutoPlay;
		flipBookComponent->m_framerateUnit = m_framerateUnit;

		// Non-serialized members
		flipBookComponent->m_materialAssignmentId = m_materialAssignmentId;
		flipBookComponent->m_previous_sprite_id = m_previous_sprite_id;
		flipBookComponent->m_offsetU = m_offsetU;
		flipBookComponent->m_offsetV = m_offsetV;
		flipBookComponent->m_spriteWidth = m_spriteWidth;
		flipBookComponent->m_spriteHeight = m_spriteHeight;
		flipBookComponent->m_tileWidth = m_tileWidth;
		flipBookComponent->m_tileHeight = m_tileHeight;
		flipBookComponent->m_tileU = m_tileU;
		flipBookComponent->m_tileV = m_tileV;
		flipBookComponent->m_currentFrame = m_currentFrame;
		flipBookComponent->m_prevFrame = m_prevFrame;
		flipBookComponent->m_elapsedTime = m_elapsedTime;
		flipBookComponent->m_isPlaying = m_isPlaying;
		flipBookComponent->m_currentLoopDirection = m_currentLoopDirection;
		flipBookComponent->m_useStartDelay = m_useStartDelay;
	}
	void EditorFlipBookComponent::DisplayEntityViewport([[maybe_unused]] const AzFramework::ViewportInfo& viewportInfo, [[maybe_unused]] AzFramework::DebugDisplayRequests& debugDisplay) {
	}
	void EditorFlipBookComponent::OnTick(float deltaTime,AZ::ScriptTimePoint /*time*/) {
		if (m_isPlaying) {
			m_elapsedTime += deltaTime;

			if (m_useStartDelay) {
				if (m_elapsedTime >= m_startDelay) {
					m_useStartDelay = false;
					m_elapsedTime = 0.0f;
					SetImageIndex(m_currentFrame);
				}
				return;
			}

			const float loopDelay = CalculateLoopDelay();

			// Calculate the frame delay (time to transition to next frame) based on
			// framerate. If framerate is in FPS we convert to seconds-per-frame to test
			// against elapsedTime.
			const float frameDelay = CalculateFramerateAsSecondsPerFrame();

			if (m_elapsedTime >= (frameDelay + loopDelay)) {
				// Determine the number of frames that has elapsed and adjust
				// "elapsed time" to account for any additional time that has
				// passed given the current delta.
				const float elapsedTimeAfterDelayFrame =
					m_elapsedTime - (frameDelay + loopDelay);
				const AZ::s32 numFramesElapsed =
					static_cast<AZ::s32>(1 + (elapsedTimeAfterDelayFrame / frameDelay));
				m_elapsedTime =
					m_elapsedTime - ((numFramesElapsed * frameDelay) + loopDelay);

				// In case the loop direction is negative, we don't want to
				// subtract from the current frame if its zero.
				m_prevFrame = m_currentFrame;
				const AZ::s32 nextFrameNum =
					AZ::GetMax<AZ::s32>(0, static_cast<AZ::s32>(m_currentFrame) +
						numFramesElapsed * m_currentLoopDirection);
				m_currentFrame = static_cast<int>(nextFrameNum);

				switch (m_loopType) {
				case FlipBookComponent::LoopType::None:
					if (m_currentFrame > m_endFrame) {
						m_currentFrame = m_endFrame;
						Stop();
					}
					break;
				case FlipBookComponent::LoopType::Linear:
					if (m_currentFrame > m_endFrame) {
						m_currentFrame = m_loopStartFrame;
					}
					break;
				case FlipBookComponent::LoopType::PingPong:
					if (m_currentLoopDirection > 0 && m_currentFrame >= m_endFrame) {
						m_currentLoopDirection = -1;
						m_currentFrame = m_endFrame;

					}
					else if (m_currentLoopDirection < 0 &&
						m_currentFrame <= m_loopStartFrame) {
						m_currentLoopDirection = 1;
						m_currentFrame = m_loopStartFrame;
					}
					break;
				default:
					break;
				}

				// Show current frame
				SetImageIndex(m_currentFrame);
			}
		}
	}
	void EditorFlipBookComponent::OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset) {
		if (asset == m_sprite) {

			if (m_debugEnabled)
			{
				AZ_Printf("Debug", "The image asset is ready");
			}

			ObtainSpriteImageSizeInPixels();

			CalculateSpriteTileSizeInPixels();

			UpdateMaterialBaseColorTexture();

			CalculateTileUSize();
			CalculateTileVSize();
			UpdateMaterialUVTile(m_tileU, m_tileV);

			m_spriteIndex = 0;
			m_startFrame = 0;
			m_endFrame = GetMaxFrame() - 1;

			if (m_previewInEditor) {
				Start();
			}
			RefreshProperties();
		}
	}
	void EditorFlipBookComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided) {
		provided.push_back(AZ_CRC_CE("FlipBookComponentService"));
	}
	void EditorFlipBookComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent) {
	}
	void EditorFlipBookComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required) {
		required.push_back(AZ_CRC_CE("MaterialProviderService"));
	}
	void EditorFlipBookComponent::GetIncompatibleServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType&incompatible) {
		incompatible.push_back(AZ_CRC_CE("FlipBookComponentService"));
	}

	void EditorFlipBookComponent::OnSpriteAssetChanged() {
		Stop();

		SetSprite(m_sprite);

		OnTotalRowsChanged();
		OnTotalColumnsChanged();
		PopulateIndexList();
		PopulateConstrainedIndexList();
	}
	void EditorFlipBookComponent::OnSpriteIndexChanged() {
		SetImageIndex(m_spriteIndex);
	}
	void EditorFlipBookComponent::OnPreviewInEditorChanged() {
		if (m_previewInEditor == true) {
			Start();
		}
		else {
			Stop();
			SetImageIndex(m_spriteIndex);
		}
	}
	void EditorFlipBookComponent::OnMaterialUVRotateChanged() {
		UpdateMaterialUVRotate(m_materialUVrotate);
	}
	void EditorFlipBookComponent::OnMaterialUVScaleChanged() {
		UpdateMaterialUVScale(m_materialUVscale);
	}
	void EditorFlipBookComponent::OnTotalColumnsChanged() {
		CalculateSpriteTileSizeInPixels();
		CalculateTileUSize();
		UpdateMaterialUVTile(m_tileU, m_tileV);
		m_spriteIndex = 0;
		m_endFrame = GetMaxFrame() - 1;
		RefreshProperties();
	}
	void EditorFlipBookComponent::OnTotalRowsChanged() {
		CalculateSpriteTileSizeInPixels();
		CalculateTileVSize();
		UpdateMaterialUVTile(m_tileU, m_tileV);
		m_spriteIndex = 0;
		m_endFrame = GetMaxFrame() - 1;
		RefreshProperties();
	}

	void EditorFlipBookComponent::CalculateTileUSize() {
		if (m_totalColumns <= 0) {
			m_tileU = 1.f / 1;
		}
		else {
			m_tileU = 1.f / m_totalColumns;
		}
	}
	void EditorFlipBookComponent::CalculateTileVSize() {
		if (m_totalRows <= 0) {
			m_tileV = 1.f / 1;
		}
		else {
			m_tileV = 1.f / m_totalRows;
		}
	}
	void EditorFlipBookComponent::CalculateOffsetU() {
		m_offsetU = m_tileU * m_spriteIndex;
	}
	void EditorFlipBookComponent::CalculateOffsetV() {
		m_offsetV = m_tileV * m_spriteIndex;
	}

	void EditorFlipBookComponent::ObtainSpriteImageSizeInPixels() {
		if (m_sprite.IsReady())  
		{
			const AZ::RPI::StreamingImageAsset* imageAsset = m_sprite.Get();
			if (imageAsset) {
				const AZ::RHI::ImageDescriptor& imageDesc =
					imageAsset->GetImageDescriptor();
				m_spriteWidth = imageDesc.m_size.m_width;
				m_spriteHeight = imageDesc.m_size.m_height;

				if (m_debugEnabled)
				{
					AZ_Printf("Debug", "Size of the Image: %u x %u pixels", m_spriteWidth,
						m_spriteHeight);
				}
			}
			else {
				AZ_Warning("Debug", false, "Invalid image asset.");
			}
		}
		else {
			AZ_Warning("Debug", false, "The image asset is not ready yet");
		}
	}
	void EditorFlipBookComponent::CalculateSpriteTileSizeInPixels() {
		if (m_totalColumns > 0 && m_totalRows > 0)
		{
			m_tileWidth = m_spriteWidth / m_totalColumns;
			m_tileHeight = m_spriteHeight / m_totalRows;

			if (m_debugEnabled)
			{
				AZ_Printf("Debug", "Tile size: %u x %u pixels", m_tileWidth,
					m_tileHeight);
			}

		}
		else {
			if (m_debugEnabled)
			{
				AZ_Warning("Debug", false, "Invalid number of rows or columns!");
			}
		}
	}

	void EditorFlipBookComponent::RefreshProperties() {
		AzToolsFramework::PropertyEditorGUIMessages::Bus::Broadcast(
			&AzToolsFramework::PropertyEditorGUIMessages::RequestRefresh,
			AzToolsFramework::PropertyModificationRefreshLevel::Refresh_Values);
	}
	
	void EditorFlipBookComponent::SetImageIndex(int imageIndex) {
		if (imageIndex < 0 || imageIndex >= m_totalColumns * m_totalRows) {
			AZ_Error("EditorFlipBookComponent", false,
				"Image index is out of bounds of spritesheet");
			return;
		}

		if (m_spriteWidth <= 0 || m_spriteHeight <= 0) {
			AZ_Warning("EditorFlipBookComponent", false,
				"Invalid Image dimension"
				"Ensure if the image was loaded correctly");
			return;
		}

		m_spriteIndex = imageIndex;

		CalculateOffsetU();

		UpdateMaterialUVOffset(m_offsetU, m_offsetV);

		if (m_debugEnabled) {
			AZ_TracePrintf("EditorFlipBookComponent",
				"Sprite Index: %d | Offset U: %f | Offset V: %f | Tile U: "
				"%f | Tile V: %f | Scale: %f | Center U: %f | Center V: %f",
				m_spriteIndex, m_offsetU, m_offsetV, m_tileU, m_tileV,
				m_materialUVscale, m_materialUVcenter.GetX(),
				m_materialUVcenter.GetY());
		}

		if (m_isPlaying) {
			m_elapsedTime = 0.0f;
		}
	}
	
	void EditorFlipBookComponent::UpdateMaterialBaseColorTexture() {
		AZ::Render::MaterialComponentRequestBus::Event(
			GetEntityId(),
			&AZ::Render::MaterialComponentRequestBus::Events::SetPropertyValue,
			m_materialAssignmentId, AZStd::string("baseColor.textureMap"),
			AZStd::any(m_sprite));
	}
	void EditorFlipBookComponent::UpdateMaterialAssignmentId(bool& retFlag) {
		retFlag = true;
		// Obtém o MaterialAssignmentMap da entidade
		AZ::Render::MaterialAssignmentMap materialMap;
		AZ::Render::MaterialComponentRequestBus::EventResult(
			materialMap, GetEntityId(),
			&AZ::Render::MaterialComponentRequestBus::Events::GetMaterialMap);

		if (materialMap.empty()) {
			AZ_Warning("EditorFlipBookComponent", false,
				"No material found on this entity");
			retFlag = false;
			return;
		}

		// Pega o primeiro material válido (ou o material específico que você deseja
		// modificar)
		m_materialAssignmentId = materialMap.begin()->first;

		// Obtém o nome do material
		AZStd::string materialLabel;
		AZ::Render::MaterialComponentRequestBus::EventResult(
			materialLabel, GetEntityId(),
			&AZ::Render::MaterialComponentRequestBus::Events::GetMaterialLabel,
			m_materialAssignmentId);

		if (m_debugEnabled) {
			AZ_TracePrintf("EditorFlipBookComponent", "Current spritesheet material name: %s",
				materialLabel.c_str());
		}
	}
	void EditorFlipBookComponent::UpdateMaterialUVProperties(float tileU,float tileV,float offsetU,float offsetV) {
		UpdateMaterialUVTile(tileU, tileV);

		UpdateMaterialUVOffset(offsetU, offsetV);
	}
	void EditorFlipBookComponent::UpdateMaterialUVOffset(float& offsetU,float& offsetV) {
		AZ::Render::MaterialComponentRequestBus::Event(
			GetEntityId(),
			&AZ::Render::MaterialComponentRequestBus::Events::SetPropertyValue,
			m_materialAssignmentId, AZStd::string("uv.offsetU"), AZStd::any(offsetU));

		AZ::Render::MaterialComponentRequestBus::Event(
			GetEntityId(),
			&AZ::Render::MaterialComponentRequestBus::Events::SetPropertyValue,
			m_materialAssignmentId, AZStd::string("uv.offsetV"), AZStd::any(offsetV));
	}
	void EditorFlipBookComponent::UpdateMaterialUVTile(float& tileU, float& tileV) {
		AZ::Render::MaterialComponentRequestBus::Event(
			GetEntityId(),
			&AZ::Render::MaterialComponentRequestBus::Events::SetPropertyValue,
			m_materialAssignmentId, AZStd::string("uv.tileU"), AZStd::any(tileU));

		AZ::Render::MaterialComponentRequestBus::Event(
			GetEntityId(),
			&AZ::Render::MaterialComponentRequestBus::Events::SetPropertyValue,
			m_materialAssignmentId, AZStd::string("uv.tileV"), AZStd::any(tileV));
	}
	void EditorFlipBookComponent::UpdateMaterialUVCenter(const AZ::Vector2& uvCenter) {
		AZ::Render::MaterialComponentRequestBus::Event(
			GetEntityId(),
			&AZ::Render::MaterialComponentRequestBus::Events::SetPropertyValue,
			m_materialAssignmentId, AZStd::string("uv.center"), AZStd::any(uvCenter));
	}
	void EditorFlipBookComponent::UpdateMaterialUVScale(float scaleFactor) {
		AZ::Render::MaterialComponentRequestBus::Event(
			GetEntityId(),
			&AZ::Render::MaterialComponentRequestBus::Events::SetPropertyValue,
			m_materialAssignmentId, AZStd::string("uv.scale"),
			AZStd::any(scaleFactor));
	}
	void EditorFlipBookComponent::UpdateMaterialUVRotate(float rotationAmount) {
		AZ::Render::MaterialComponentRequestBus::Event(
			GetEntityId(),
			&AZ::Render::MaterialComponentRequestBus::Events::SetPropertyValue,
			m_materialAssignmentId, AZStd::string("uv.rotateDegrees"),
			AZStd::any(rotationAmount));
	}

	int EditorFlipBookComponent::GetMaxFrame() const {
		return m_totalColumns * m_totalRows;
	}

	bool EditorFlipBookComponent::FrameWithinRange(int frameValue) {
		int maxFrame = GetMaxFrame();
		return maxFrame > 0 && frameValue < maxFrame;
	}

	AZStd::vector<AZ::Edit::EnumConstant<int>> EditorFlipBookComponent::PopulateIndexList() const {
		AZStd::vector<AZ::Edit::EnumConstant<int>> indexList;
		int maxFrame = GetMaxFrame();

		for (int i = 0; i < maxFrame; ++i) {
			indexList.emplace_back(i, AZStd::to_string(i).c_str());
		}

		return indexList;
	}

	AZStd::vector<AZ::Edit::EnumConstant<int>> EditorFlipBookComponent::PopulateConstrainedIndexList() const {
		AZStd::vector<AZ::Edit::EnumConstant<int>> indexList;

		int startFrame = AZStd::min(m_startFrame, m_endFrame);
		int endFrame = AZStd::min(m_endFrame, GetMaxFrame() - 1);

		for (int i = startFrame; i <= endFrame; ++i) {
			indexList.emplace_back(i, AZStd::to_string(i).c_str());
		}

		return indexList;
	}


	void EditorFlipBookComponent::OnStartFrameChange() {
		m_endFrame = AZ::GetMax<int>(m_startFrame, m_endFrame);
		m_currentFrame = AZ::GetClamp<int>(m_currentFrame, m_startFrame, m_endFrame);
		m_loopStartFrame =
			AZ::GetClamp<int>(m_loopStartFrame, m_startFrame, m_endFrame);
	}

	void EditorFlipBookComponent::OnEndFrameChange() {
		m_startFrame = AZ::GetMin<int>(m_startFrame, m_endFrame);
		m_currentFrame = AZ::GetClamp<int>(m_currentFrame, m_startFrame, m_endFrame);
		m_loopStartFrame =
			AZ::GetClamp<int>(m_loopStartFrame, m_startFrame, m_endFrame);
	}

	void EditorFlipBookComponent::OnFramerateUnitChange() {
		AZ_Assert(
			m_framerateUnit == FlipBookComponent::FramerateUnits::FPS ||
			m_framerateUnit == FlipBookComponent::FramerateUnits::SecondsPerFrame,
			"New framerate unit added for flipbooks - please update this "
			"function accordingly!");
		m_framerate = m_framerate != 0.0f ? 1.0f / m_framerate : 0.0f;
	}

	bool EditorFlipBookComponent::IsPingPongLoopType() const {
		return m_loopType == FlipBookComponent::LoopType::PingPong;
	}

	bool EditorFlipBookComponent::IsLoopingType() const {
		return m_loopType != FlipBookComponent::LoopType::None;
	}

	float EditorFlipBookComponent::CalculateLoopDelay() const {
		float loopDelay = 0.0f;
		if (IsLoopingType()) {
			const bool isStartFrame = m_currentFrame == m_loopStartFrame;
			const bool playingIntro =
				m_prevFrame < m_currentFrame && m_startFrame != m_loopStartFrame;
			const bool shouldApplyStartLoopDelay = isStartFrame && !playingIntro;

			if (shouldApplyStartLoopDelay) {
				loopDelay = m_loopDelay;
			}
			else if (m_loopType == FlipBookComponent::LoopType::PingPong) {
				const bool isEndFrame = m_currentFrame == m_endFrame;
				const bool isPlayingReverse = m_currentLoopDirection < 0;
				const bool shouldApplyReverseDelay = isEndFrame && isPlayingReverse;

				if (shouldApplyReverseDelay) {
					loopDelay = m_reverseDelay;
				}
			}
		}

		return loopDelay;
	}

	void EditorFlipBookComponent::Start() {
		m_currentFrame = m_startFrame;
		m_currentLoopDirection = 1;
		m_isPlaying = true;
		m_elapsedTime = 0.0f;
		m_useStartDelay = m_startDelay > 0.0f ? true : false;

		if (!m_useStartDelay) {
			SetImageIndex(m_currentFrame);
		}

		if (!AZ::TickBus::Handler::BusIsConnected()) {
			AZ::TickBus::Handler::BusConnect();
		}
	}

	void EditorFlipBookComponent::Stop() {
		m_isPlaying = false;
		AZ::TickBus::Handler::BusDisconnect();
	}

	void EditorFlipBookComponent::SetSprite([[maybe_unused]] AZ::Data::Asset<AZ::RPI::StreamingImageAsset>& sprite) {
		if (!m_sprite.GetId().IsValid()) {
			AZ_Warning("EditorFlipBookComponent", true,
				"A Sprite must be specified");
			return;
		}

		AZ::Data::AssetBus::Handler::BusDisconnect(m_previous_sprite_id);

		if (!AZ::Data::AssetBus::Handler::BusIsConnectedId(m_sprite.GetId())) {
			AZ::Data::AssetBus::Handler::BusConnect(m_sprite.GetId());
		}
		m_previous_sprite_id = m_sprite.GetId();
	}

	void EditorFlipBookComponent::SetStartFrame(int startFrame) {
		if (!FrameWithinRange(startFrame)) {
			AZ_Warning("UI", false, "Invalid frame value given: %u", startFrame);
			return;
		}

		m_startFrame = startFrame;
		OnStartFrameChange();
	}

	void EditorFlipBookComponent::SetEndFrame(int endFrame) {
		if (!FrameWithinRange(endFrame)) {
			AZ_Warning("UI", false, "Invalid frame value given: %u", endFrame);
			return;
		}

		m_endFrame = endFrame;
		OnEndFrameChange();
	}

	void EditorFlipBookComponent::SetCurrentFrame(int currentFrame) {
		// The current frame needs to stay between the start and end frames
		const bool validFrameValue =
			currentFrame >= m_startFrame && currentFrame <= m_endFrame;
		if (!validFrameValue) {
			AZ_Warning("UI", false, "Invalid frame value given: %u", currentFrame);
			return;
		}

		m_currentFrame = currentFrame;
		SetImageIndex(m_currentFrame);
	}

	void EditorFlipBookComponent::SetLoopStartFrame(int loopStartFrame) {
		// Ensure that loop start frame exists within start and end frame range
		const bool validFrameValue =
			loopStartFrame >= m_startFrame && loopStartFrame <= m_endFrame;
		if (!validFrameValue) {
			AZ_Warning("UI", false, "Invalid frame value given: %u", loopStartFrame);
			return;
		}

		m_loopStartFrame = loopStartFrame;
	}

	void EditorFlipBookComponent::SetLoopType(FlipBookComponent::FlipBookComponent::LoopType loopType) {
		m_loopType = loopType;

		// PingPong is currently the only loop type that supports a negative loop
		// direction.
		if (m_loopType != FlipBookComponent::FlipBookComponent::LoopType::PingPong) {
			m_currentLoopDirection = 1;
		}
	}

}  // namespace ProjectK
