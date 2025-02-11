
#pragma once

#include <AzCore/Component/Component.h>

#include <ProjectK/ProjectKBus.h>

namespace ProjectK
{
    class ProjectKSystemComponent
        : public AZ::Component
        , protected ProjectKRequestBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(ProjectKSystemComponent);

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        ProjectKSystemComponent();
        ~ProjectKSystemComponent();

    protected:
        ////////////////////////////////////////////////////////////////////////
        // ProjectKRequestBus interface implementation

        ////////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////
    };
}
