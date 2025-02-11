
#ifndef TEMPLATE_COMPONENT_H
#define TEMPLATE_COMPONENT_H


#include <AzCore/Component/Component.h>
#include <ProjectK/TemplateInterface.h>

namespace ProjectK
{
    /*
    * TODO: Register this component in your Gem's AZ::Module interface by inserting the following into the list of m_descriptors:
    *       TemplateComponent::CreateDescriptor(),
    */

    class TemplateComponent
        : public AZ::Component
        , public TemplateRequestBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(TemplateComponent);


        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);

        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);

        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);

        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

    protected:

        void Activate() override;


        void Deactivate() override;

    private:
    
    };
} // namespace ProjectK

#endif // !TEMPLATE_COMPONENT_H