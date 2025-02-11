
#include <AzCore/Serialization/SerializeContext.h>

#include "ProjectKSystemComponent.h"

#include <ProjectK/ProjectKTypeIds.h>

namespace ProjectK
{
    AZ_COMPONENT_IMPL(ProjectKSystemComponent, "ProjectKSystemComponent",
        ProjectKSystemComponentTypeId);

    void ProjectKSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<ProjectKSystemComponent, AZ::Component>()
                ->Version(0)
                ;
        }
    }

    void ProjectKSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("ProjectKService"));
    }

    void ProjectKSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("ProjectKService"));
    }

    void ProjectKSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void ProjectKSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    ProjectKSystemComponent::ProjectKSystemComponent()
    {
        if (ProjectKInterface::Get() == nullptr)
        {
            ProjectKInterface::Register(this);
        }
    }

    ProjectKSystemComponent::~ProjectKSystemComponent()
    {
        if (ProjectKInterface::Get() == this)
        {
            ProjectKInterface::Unregister(this);
        }
    }

    void ProjectKSystemComponent::Init()
    {
    }

    void ProjectKSystemComponent::Activate()
    {
        ProjectKRequestBus::Handler::BusConnect();
    }

    void ProjectKSystemComponent::Deactivate()
    {
        ProjectKRequestBus::Handler::BusDisconnect();
    }
}
