
#include "FlipBookComponent.h"

#include <AzCore/Debug/Trace.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>

namespace ProjectK {

	AZ_COMPONENT_IMPL(FlipBookComponent, "FlipBookComponent", "{0B964033-10C2-4E16-A6E2-1C7445765ABE}");


	void FlipBookComponent::Reflect(AZ::ReflectContext* context) {
		if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context)) {
			serializeContext->Class<FlipBookComponent, AZ::Component>()
				->Version(1)
				->Field("Sprite", &FlipBookComponent::m_sprite)
				->Field("TotalRows", &FlipBookComponent::m_totalRows)
				->Field("TotalColumns", &FlipBookComponent::m_totalColumns)
				->Field("UV Center", &FlipBookComponent::m_materialUVcenter)
				->Field("Sprite Rotation", &FlipBookComponent::m_materialUVrotate)
				->Field("Sprite Scale", &FlipBookComponent::m_materialUVscale)
				->Field("SpriteIndex", &FlipBookComponent::m_spriteIndex)
				->Field("Start Frame", &FlipBookComponent::m_startFrame)
				->Field("End Frame", &FlipBookComponent::m_endFrame)
				->Field("Loop Start Frame", &FlipBookComponent::m_loopStartFrame)
				->Field("Loop Type", &FlipBookComponent::m_loopType)
				->Field("Framerate Unit", &FlipBookComponent::m_framerateUnit)
				->Field("Framerate", &FlipBookComponent::m_framerate)
				->Field("Start Delay", &FlipBookComponent::m_startDelay)
				->Field("Loop Delay", &FlipBookComponent::m_loopDelay)
				->Field("Reverse Delay", &FlipBookComponent::m_reverseDelay)
				->Field("Auto Play", &FlipBookComponent::m_isAutoPlay)
				->Field("DebugEnabled", &FlipBookComponent::m_debugEnabled)
				->Field("Preview In Editor", &FlipBookComponent::m_previewInEditor);

			//if (AZ::EditContext* editContext = serializeContext->GetEditContext()) {
			//	editContext
			//		->Class<FlipBookComponent>(
			//			"FlipBookComponent",
			//			"This component Render a sprite on the mesh specified by the mesh component and plays a spritesheet animation")
			//		->ClassElement(AZ::Edit::ClassElements::EditorData, "")
			//		->Attribute(AZ::Edit::Attributes::Category, "CustomComponents")
			//		->Attribute(AZ::Edit::Attributes::Icon,
			//			"Icons/Components/Component_Placeholder.svg")
			//		->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu,
			//			AZ_CRC_CE("Game"))
			//		;
			//}
		}
	}
	
	void FlipBookComponent::Activate() {
		SetSprite(m_sprite);

		bool materialAssignemtnSucessReturnStatus{ false };
		UpdateMaterialAssignmentId(materialAssignemtnSucessReturnStatus);
		if (materialAssignemtnSucessReturnStatus == false) {
			AZ_Error("FlipBookComponent", true, "Failed to get the Material ID");
			return;
		}
	}
	void FlipBookComponent::Deactivate() {
		// FlipBookRequestBus::Handler::BusDisconnect(GetEntityId());
		AZ::Data::AssetBus::Handler::BusDisconnect();
		AZ::TickBus::Handler::BusDisconnect();
	}

	void FlipBookComponent::OnTick(float deltaTime, AZ::ScriptTimePoint /*time*/) {
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
	void FlipBookComponent::OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset) {
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
		}
	}
	void FlipBookComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided) {
		provided.push_back(AZ_CRC_CE("FlipBookComponentService"));
	}
	void FlipBookComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent) {
	}
	void FlipBookComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required) {
		required.push_back(AZ_CRC_CE("MaterialProviderService"));
	}
	void FlipBookComponent::GetIncompatibleServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& incompatible) {
		incompatible.push_back(AZ_CRC_CE("FlipBookComponentService"));
	}

	void FlipBookComponent::OnSpriteAssetChanged() {
		Stop();

		SetSprite(m_sprite);

		OnTotalRowsChanged();
		OnTotalColumnsChanged();
		PopulateIndexList();
		PopulateConstrainedIndexList();
	}
	void FlipBookComponent::OnSpriteIndexChanged() {
		SetImageIndex(m_spriteIndex);
	}
	void FlipBookComponent::OnPreviewInEditorChanged() {
		if (m_previewInEditor == true) {
			Start();
		}
		else {
			Stop();
			SetImageIndex(m_spriteIndex);
		}
	}
	void FlipBookComponent::OnMaterialUVRotateChanged() {
		UpdateMaterialUVRotate(m_materialUVrotate);
	}
	void FlipBookComponent::OnMaterialUVScaleChanged() {
		UpdateMaterialUVScale(m_materialUVscale);
	}
	void FlipBookComponent::OnTotalColumnsChanged() {
		CalculateSpriteTileSizeInPixels();
		CalculateTileUSize();
		UpdateMaterialUVTile(m_tileU, m_tileV);
		m_spriteIndex = 0;
		m_endFrame = GetMaxFrame() - 1;
	}
	void FlipBookComponent::OnTotalRowsChanged() {
		CalculateSpriteTileSizeInPixels();
		CalculateTileVSize();
		UpdateMaterialUVTile(m_tileU, m_tileV);
		m_spriteIndex = 0;
		m_endFrame = GetMaxFrame() - 1;
	}

	void FlipBookComponent::CalculateTileUSize() {
		if (m_totalColumns <= 0) {
			m_tileU = 1.f / 1;
		}
		else {
			m_tileU = 1.f / m_totalColumns;
		}
	}
	void FlipBookComponent::CalculateTileVSize() {
		if (m_totalRows <= 0) {
			m_tileV = 1.f / 1;
		}
		else {
			m_tileV = 1.f / m_totalRows;
		}
	}
	void FlipBookComponent::CalculateOffsetU() {
		m_offsetU = m_tileU * m_spriteIndex;
	}
	void FlipBookComponent::CalculateOffsetV() {
		m_offsetV = m_tileV * m_spriteIndex;
	}

	void FlipBookComponent::ObtainSpriteImageSizeInPixels() {
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
	void FlipBookComponent::CalculateSpriteTileSizeInPixels() {
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

	void FlipBookComponent::SetImageIndex(int imageIndex) {
		if (imageIndex < 0 || imageIndex >= m_totalColumns * m_totalRows) {
			AZ_Error("FlipBookComponent", false,
				"Image index is out of bounds of spritesheet");
			return;
		}

		if (m_spriteWidth <= 0 || m_spriteHeight <= 0) {
			AZ_Warning("FlipBookComponent", false,
				"Invalid Image dimension"
				"Ensure if the image was loaded correctly");
			return;
		}

		m_spriteIndex = imageIndex;

		CalculateOffsetU();

		UpdateMaterialUVOffset(m_offsetU, m_offsetV);

		if (m_debugEnabled) {
			AZ_TracePrintf("FlipBookComponent",
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

	void FlipBookComponent::UpdateMaterialBaseColorTexture() {
		AZ::Render::MaterialComponentRequestBus::Event(
			GetEntityId(),
			&AZ::Render::MaterialComponentRequestBus::Events::SetPropertyValue,
			m_materialAssignmentId, AZStd::string("baseColor.textureMap"),
			AZStd::any(m_sprite));
	}
	void FlipBookComponent::UpdateMaterialAssignmentId(bool& retFlag) {
		retFlag = true;
		// Obtém o MaterialAssignmentMap da entidade
		AZ::Render::MaterialAssignmentMap materialMap;
		AZ::Render::MaterialComponentRequestBus::EventResult(
			materialMap, GetEntityId(),
			&AZ::Render::MaterialComponentRequestBus::Events::GetMaterialMap);

		if (materialMap.empty()) {
			AZ_Warning("FlipBookComponent", false,
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
			AZ_TracePrintf("FlipBookComponent", "Current spritesheet material name: %s",
				materialLabel.c_str());
		}
	}
	void FlipBookComponent::UpdateMaterialUVProperties(float tileU, float tileV, float offsetU, float offsetV) {
		UpdateMaterialUVTile(tileU, tileV);

		UpdateMaterialUVOffset(offsetU, offsetV);
	}
	void FlipBookComponent::UpdateMaterialUVOffset(float& offsetU, float& offsetV) {
		AZ::Render::MaterialComponentRequestBus::Event(
			GetEntityId(),
			&AZ::Render::MaterialComponentRequestBus::Events::SetPropertyValue,
			m_materialAssignmentId, AZStd::string("uv.offsetU"), AZStd::any(offsetU));

		AZ::Render::MaterialComponentRequestBus::Event(
			GetEntityId(),
			&AZ::Render::MaterialComponentRequestBus::Events::SetPropertyValue,
			m_materialAssignmentId, AZStd::string("uv.offsetV"), AZStd::any(offsetV));
	}
	void FlipBookComponent::UpdateMaterialUVTile(float& tileU, float& tileV) {
		AZ::Render::MaterialComponentRequestBus::Event(
			GetEntityId(),
			&AZ::Render::MaterialComponentRequestBus::Events::SetPropertyValue,
			m_materialAssignmentId, AZStd::string("uv.tileU"), AZStd::any(tileU));

		AZ::Render::MaterialComponentRequestBus::Event(
			GetEntityId(),
			&AZ::Render::MaterialComponentRequestBus::Events::SetPropertyValue,
			m_materialAssignmentId, AZStd::string("uv.tileV"), AZStd::any(tileV));
	}
	void FlipBookComponent::UpdateMaterialUVCenter(const AZ::Vector2& uvCenter) {
		AZ::Render::MaterialComponentRequestBus::Event(
			GetEntityId(),
			&AZ::Render::MaterialComponentRequestBus::Events::SetPropertyValue,
			m_materialAssignmentId, AZStd::string("uv.center"), AZStd::any(uvCenter));
	}
	void FlipBookComponent::UpdateMaterialUVScale(float scaleFactor) {
		AZ::Render::MaterialComponentRequestBus::Event(
			GetEntityId(),
			&AZ::Render::MaterialComponentRequestBus::Events::SetPropertyValue,
			m_materialAssignmentId, AZStd::string("uv.scale"),
			AZStd::any(scaleFactor));
	}
	void FlipBookComponent::UpdateMaterialUVRotate(float rotationAmount) {
		AZ::Render::MaterialComponentRequestBus::Event(
			GetEntityId(),
			&AZ::Render::MaterialComponentRequestBus::Events::SetPropertyValue,
			m_materialAssignmentId, AZStd::string("uv.rotateDegrees"),
			AZStd::any(rotationAmount));
	}

	int FlipBookComponent::GetMaxFrame() const {
		return m_totalColumns * m_totalRows;
	}

	bool FlipBookComponent::FrameWithinRange(int frameValue) {
		int maxFrame = GetMaxFrame();
		return maxFrame > 0 && frameValue < maxFrame;
	}

	AZStd::vector<AZ::Edit::EnumConstant<int>> FlipBookComponent::PopulateIndexList() const {
		AZStd::vector<AZ::Edit::EnumConstant<int>> indexList;
		int maxFrame = GetMaxFrame();

		for (int i = 0; i < maxFrame; ++i) {
			indexList.emplace_back(i, AZStd::to_string(i).c_str());
		}

		return indexList;
	}

	AZStd::vector<AZ::Edit::EnumConstant<int>> FlipBookComponent::PopulateConstrainedIndexList() const {
		AZStd::vector<AZ::Edit::EnumConstant<int>> indexList;

		int startFrame = AZStd::min(m_startFrame, m_endFrame);
		int endFrame = AZStd::min(m_endFrame, GetMaxFrame() - 1);

		for (int i = startFrame; i <= endFrame; ++i) {
			indexList.emplace_back(i, AZStd::to_string(i).c_str());
		}

		return indexList;
	}


	void FlipBookComponent::OnStartFrameChange() {
		m_endFrame = AZ::GetMax<int>(m_startFrame, m_endFrame);
		m_currentFrame = AZ::GetClamp<int>(m_currentFrame, m_startFrame, m_endFrame);
		m_loopStartFrame =
			AZ::GetClamp<int>(m_loopStartFrame, m_startFrame, m_endFrame);
	}

	void FlipBookComponent::OnEndFrameChange() {
		m_startFrame = AZ::GetMin<int>(m_startFrame, m_endFrame);
		m_currentFrame = AZ::GetClamp<int>(m_currentFrame, m_startFrame, m_endFrame);
		m_loopStartFrame =
			AZ::GetClamp<int>(m_loopStartFrame, m_startFrame, m_endFrame);
	}

	void FlipBookComponent::OnFramerateUnitChange() {
		AZ_Assert(
			m_framerateUnit == FlipBookComponent::FramerateUnits::FPS ||
			m_framerateUnit == FlipBookComponent::FramerateUnits::SecondsPerFrame,
			"New framerate unit added for flipbooks - please update this "
			"function accordingly!");
		m_framerate = m_framerate != 0.0f ? 1.0f / m_framerate : 0.0f;
	}

	bool FlipBookComponent::IsPingPongLoopType() const {
		return m_loopType == FlipBookComponent::LoopType::PingPong;
	}

	bool FlipBookComponent::IsLoopingType() const {
		return m_loopType != FlipBookComponent::LoopType::None;
	}

	float FlipBookComponent::CalculateLoopDelay() const {
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

	void FlipBookComponent::Start() {
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

	void FlipBookComponent::Stop() {
		m_isPlaying = false;
		AZ::TickBus::Handler::BusDisconnect();
	}

	void FlipBookComponent::SetSprite([[maybe_unused]] AZ::Data::Asset<AZ::RPI::StreamingImageAsset>& sprite) {
		if (!m_sprite.GetId().IsValid()) {
			AZ_Warning("FlipBookComponent", true,
				"A Sprite must be specified");
			return;
		}

		AZ::Data::AssetBus::Handler::BusDisconnect(m_previous_sprite_id);

		if (!AZ::Data::AssetBus::Handler::BusIsConnectedId(m_sprite.GetId())) {
			AZ::Data::AssetBus::Handler::BusConnect(m_sprite.GetId());
		}
		m_previous_sprite_id = m_sprite.GetId();
	}

	void FlipBookComponent::SetStartFrame(int startFrame) {
		if (!FrameWithinRange(startFrame)) {
			AZ_Warning("UI", false, "Invalid frame value given: %u", startFrame);
			return;
		}

		m_startFrame = startFrame;
		OnStartFrameChange();
	}

	void FlipBookComponent::SetEndFrame(int endFrame) {
		if (!FrameWithinRange(endFrame)) {
			AZ_Warning("UI", false, "Invalid frame value given: %u", endFrame);
			return;
		}

		m_endFrame = endFrame;
		OnEndFrameChange();
	}

	void FlipBookComponent::SetCurrentFrame(int currentFrame) {
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

	void FlipBookComponent::SetLoopStartFrame(int loopStartFrame) {
		// Ensure that loop start frame exists within start and end frame range
		const bool validFrameValue =
			loopStartFrame >= m_startFrame && loopStartFrame <= m_endFrame;
		if (!validFrameValue) {
			AZ_Warning("UI", false, "Invalid frame value given: %u", loopStartFrame);
			return;
		}

		m_loopStartFrame = loopStartFrame;
	}

	void FlipBookComponent::SetLoopType(FlipBookComponent::FlipBookComponent::LoopType loopType) {
		m_loopType = loopType;

		// PingPong is currently the only loop type that supports a negative loop
		// direction.
		if (m_loopType != FlipBookComponent::FlipBookComponent::LoopType::PingPong) {
			m_currentLoopDirection = 1;
		}
	}

}  // namespace ProjectK
