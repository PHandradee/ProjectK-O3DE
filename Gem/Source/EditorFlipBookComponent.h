#ifndef EDITOR_FLIPBOOK_COMPONENT_H
#define EDITOR_FLIPBOOK_COMPONENT_H

/* Include the following headers:
 * EditorComponentBase.h - the editor component base class. Derive from
 * this class to create a component to use in the editor that is the
 * counterpart of the version of the component that is used during runtime.
 * EntityDebugDisplayBus.h - provides a debug draw API to be used by components.
 * EditorVisibilityBus.h - controls whether an entity is shown or hidden in the
 * editor FlipBookComponent.h - header file for the runtime version of the
 * component.
 */
#include <AzCore/Asset/AssetCommon.h>
#include <AzFramework/Entity/EntityDebugDisplayBus.h>
#include <AzToolsFramework/ToolsComponents/EditorComponentBase.h>
#include <AzToolsFramework/ToolsComponents/EditorVisibilityBus.h>

#include "FlipBookComponent.h"

namespace ProjectK {

	class EditorFlipBookComponent
		: public AzToolsFramework::Components::EditorComponentBase,
		private AzFramework::EntityDebugDisplayEventBus::Handler,
		public AZ::TickBus::Handler,
		public AZ::Data::AssetBus::Handler {
	public:
		AZ_EDITOR_COMPONENT(EditorFlipBookComponent,
			"{5034A7F3-63DB-4298-83AA-915AB23EFEA0}");

		// Perform reflection for this component. The context parameter is the
		// reflection context.
		static void Reflect(AZ::ReflectContext* context);

		// AZ::Component interface implementation.
		void Init() override;
		void Activate() override;
		void Deactivate() override;

		// Faciliate the translation of an editor component into a runtime component.
		void BuildGameEntity(AZ::Entity* gameEntity) override;

		// AzFramework::EntityDebugDisplayEventBus implementation.
		void DisplayEntityViewport(
			const AzFramework::ViewportInfo& viewportInfo,
			AzFramework::DebugDisplayRequests& debugDisplay) override;

		// AZ::TickBus implementation
		void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

		// AZ::Data AssetBus Implementation
		void OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset) override;

		// Optional functions for defining provided and dependent services.
		static void GetProvidedServices(
			AZ::ComponentDescriptor::DependencyArrayType& provided);
		static void GetDependentServices(
			AZ::ComponentDescriptor::DependencyArrayType& dependent);
		static void GetRequiredServices(
			AZ::ComponentDescriptor::DependencyArrayType& required);
		static void GetIncompatibleServices(
			AZ::ComponentDescriptor::DependencyArrayType& incompatible);

	private:
		void OnSpriteAssetChanged();
		void OnSpriteIndexChanged();
		void OnPreviewInEditorChanged();
		void OnMaterialUVRotateChanged();
		void OnMaterialUVScaleChanged();
		void OnTotalColumnsChanged();
		void OnTotalRowsChanged();

		void CalculateTileUSize();
		void CalculateTileVSize();
		void CalculateOffsetU();
		void CalculateOffsetV();

		void ObtainSpriteImageSizeInPixels();
		void CalculateSpriteTileSizeInPixels();

		void RefreshProperties();

		void SetImageIndex(int imageIndex);

		void UpdateMaterialBaseColorTexture();
		void UpdateMaterialAssignmentId(bool& retFlag);
		void UpdateMaterialUVProperties(float tileU,
			float tileV,
			float offsetU,
			float offsetV);
		void UpdateMaterialUVOffset(float& offsetU, float& offsetV);
		void UpdateMaterialUVTile(float& tileU, float& tileV);
		void UpdateMaterialUVCenter(const AZ::Vector2& uvCenter);
		void UpdateMaterialUVScale(float scaleFactor);
		void UpdateMaterialUVRotate(float rotationAmount);

		//! Total number of cells within sprite-sheet
		int GetMaxFrame() const;

		//! Ensures that the given frame value is valid for the associated
		//! sprite-sheet
		bool FrameWithinRange(int frameValue);

		AZStd::vector<AZ::Edit::EnumConstant<int>> PopulateIndexList() const;

		AZStd::vector<AZ::Edit::EnumConstant<int>> PopulateConstrainedIndexList()
			const;

		//! Updates correlated frame values when the start frame value changes.
		void OnStartFrameChange();

		//! Updates correlated frame values when the end frame value changes.
		void OnEndFrameChange();

		//! Converts the frame-rate value based on the new framerate unit.
		void OnFramerateUnitChange();

		//! Returns true if this is a PingPong loop type, false otherwise.
		bool IsPingPongLoopType() const;

		//! Returns true if animation will loop, false otherwise.
		bool IsLoopingType() const;

		//! Returns the amount to delay the current (forward or reverse) loop sequence
		float CalculateLoopDelay() const;

		//! Returns the framerate in seconds-per-frame.
		float CalculateFramerateAsSecondsPerFrame() const
		{
			return m_framerateUnit == FlipBookComponent::FramerateUnits::FPS && m_framerate != 0.0f
				? 1.0f / m_framerate
				: m_framerate;
		}

		void Start();
		void Stop();
		bool IsPlaying() { return m_isPlaying; }
		void SetSprite(AZ::Data::Asset<AZ::RPI::StreamingImageAsset>& sprite);
		int GetStartFrame() { return m_startFrame; }
		void SetStartFrame(int startFrame);
		int GetEndFrame() { return m_endFrame; }
		void SetEndFrame(int endFrame);
		int GetCurrentFrame() { return m_currentFrame; }
		void SetCurrentFrame(int currentFrame);
		int GetLoopStartFrame() { return m_loopStartFrame; }
		void SetLoopStartFrame(int loopStartFrame);
		FlipBookComponent::LoopType GetLoopType() { return m_loopType; }
		void SetLoopType(FlipBookComponent::LoopType loopType);
		float GetFramerate() { return m_framerate; }
		void SetFramerate(float framerate)
		{
			m_framerate = AZ::GetMax<float>(0.0f, framerate);
		}
		FlipBookComponent::FramerateUnits GetFramerateUnit()
		{
			return m_framerateUnit;
		}
		void SetFramerateUnit(FlipBookComponent::FramerateUnits framerateUnit)
		{
			m_framerateUnit = framerateUnit;
		}
		float GetStartDelay() { return m_startDelay; }
		void SetStartDelay(float startDelay)
		{
			m_startDelay = AZ::GetMax<float>(0.0f, startDelay);
		}
		float GetLoopDelay() { return m_loopDelay; }
		void SetLoopDelay(float loopDelay)
		{
			m_loopDelay = AZ::GetMax<float>(0.0f, loopDelay);
		}
		float GetReverseDelay() { return m_reverseDelay; }
		void SetReverseDelay(float reverseDelay)
		{
			m_reverseDelay = AZ::GetMax<float>(0.0f, reverseDelay);
		}
		bool GetIsAutoPlay() { return m_isAutoPlay; }
		void SetIsAutoPlay(bool isAutoPlay) { m_isAutoPlay = isAutoPlay; }

	private:
		// Serialized members
		AZ::Vector2 m_materialUVcenter{ 0.f, 0.5f };
		float m_materialUVrotate{ 0.f };
		float m_materialUVscale{ 1.f };
		bool m_previewInEditor{ false };
		AZ::Data::Asset<AZ::RPI::StreamingImageAsset> m_sprite;
		bool m_debugEnabled{ false };
		int m_spriteIndex{ 0 };
		int m_totalColumns{ 1 };
		int m_totalRows{ 1 };
		int m_startFrame{ 0 }; //!< Start frame of animation. Can be different from
		//!< "loop start" frame
		//!< to allow animations to have an "intro" sequence.
		int m_endFrame{ 0 }; //!< Last frame of animation.
		int m_loopStartFrame{ 0 }; //!< Start frame for looped animations
		FlipBookComponent::LoopType m_loopType{ FlipBookComponent::LoopType::None };
		float m_framerate{ 0.0f }; //!< Transition speed between frames (either in FPS
		//!< or seconds to wait per frame).
		float m_startDelay{ 0.0f }; //!< Number of seconds to wait before playing the
		//!< flipbook (applied only once during playback).
		float m_loopDelay{
			0.0f
		}; //!< Number of seconds to wait before playing the loop sequence
		float m_reverseDelay{
			0.0f
		}; //!< Number of seconds to wait before playing the
		//!< reverse loop sequence (PingPong loop types)
		bool m_isAutoPlay{
			true
		}; //!< Whether the animation should automatically start playing.
		FlipBookComponent::FramerateUnits m_framerateUnit{
			FlipBookComponent::FramerateUnits::FPS
		}; //! Units to use when defining
		//! framerate

	 // Non-serialized members
		AZ::Render::MaterialAssignmentId m_materialAssignmentId;
		AZ::Data::AssetId m_previous_sprite_id;
		float m_offsetU{ 0.f };
		float m_offsetV{ 0.f };
		uint32_t m_spriteWidth{};
		uint32_t m_spriteHeight{};
		uint32_t m_tileWidth{};
		uint32_t m_tileHeight{};
		float m_tileU{};
		float m_tileV{};
		int m_currentFrame{ 0 }; //!< Current sprite-sheet frame/index displayed
		int m_prevFrame{ 0 }; //!< Previous frame displayed - used to detect intro
		//!< sequences in a loop
		float m_elapsedTime{
			0.0f
		}; //!< Used to determine passage of time for handling frame delay
		bool m_isPlaying{
			false
		}; //!< True if the animation is playing, false otherwise
		AZ::s32 m_currentLoopDirection{
			1
		}; //!< Used for PingPong loop direction (positive/negative)
		bool m_useStartDelay{
			false
		}; //!< Keeps track of whether start delay applies or not.
	};

} // namespace ProjectK

#endif // !EDITOR_FLIPBOOK_COMPONENT_H
