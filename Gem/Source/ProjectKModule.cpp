
#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Module/Module.h>

#include "ProjectKSystemComponent.h"

#include <ProjectK/ProjectKTypeIds.h>

#include "FlipBookComponent.h"
#include "EditorFlipBookComponent.h"


namespace ProjectK
{
    class ProjectKModule
        : public AZ::Module
    {
    public:
        AZ_RTTI(ProjectKModule, ProjectKModuleTypeId, AZ::Module);
        AZ_CLASS_ALLOCATOR(ProjectKModule, AZ::SystemAllocator);

        ProjectKModule()
            : AZ::Module()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            m_descriptors.insert(m_descriptors.end(), {
                ProjectKSystemComponent::CreateDescriptor(),
                FlipBookComponent::CreateDescriptor(),
                EditorFlipBookComponent::CreateDescriptor(),
                });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<ProjectKSystemComponent>(),
            };

        }

    };
}// namespace ProjectK

#if defined(O3DE_GEM_NAME)
AZ_DECLARE_MODULE_CLASS(AZ_JOIN(Gem_, O3DE_GEM_NAME), ProjectK::ProjectKModule)
#else
AZ_DECLARE_MODULE_CLASS(Gem_ProjectK, ProjectK::ProjectKModule)
#endif
