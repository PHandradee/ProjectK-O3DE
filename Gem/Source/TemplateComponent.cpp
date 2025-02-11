
#include "TemplateComponent.h"

#include <AzCore/Debug/Trace.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>


namespace ProjectK
{
    AZ_COMPONENT_IMPL(TemplateComponent, "TemplateComponent", "{FB5BF3AE-2FE3-4751-90CB-19DEA6D15E00}");

    void TemplateComponent::Activate()
    {
        TemplateRequestBus::Handler::BusConnect(GetEntityId());
    }

    void TemplateComponent::Deactivate()
    {
        TemplateRequestBus::Handler::BusDisconnect(GetEntityId());
    }


    void TemplateComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<TemplateComponent, AZ::Component>()
                ->Version(1)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<TemplateComponent>("TemplateComponent", "[Description of functionality provided by this component]")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "CustomComponents")
                    ->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                    ;
            }
        }

        if (AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->Class<TemplateComponent>("Template Component Group")
                ->Attribute(AZ::Script::Attributes::Category, "ProjectK Gem Group")
                ;
        }
    }


    void TemplateComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("TemplateComponentService"));
    }

    void TemplateComponent::GetIncompatibleServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
    }

    void TemplateComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void TemplateComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }


} // namespace ProjectK
