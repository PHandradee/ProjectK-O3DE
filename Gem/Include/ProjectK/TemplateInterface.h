
#ifndef TEMPLATE_INTERFACE_H
#define TEMPLATE_INTERFACE_H


#include <AzCore/Component/ComponentBus.h>

namespace ProjectK
{
    class TemplateRequests
        : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(ProjectK::TemplateRequests, "{51347C6B-904B-4492-BE82-DF5B192CD4E7}");

        // Put your public request methods here.
        
        // Put notification events here. Examples:
        // void RegisterEvent(AZ::EventHandler<...> notifyHandler);
        // AZ::Event<...> m_notifyEvent1;
        
    };

    using TemplateRequestBus = AZ::EBus<TemplateRequests>;

} // namespace ProjectK

#endif // !TEMPLATE_INTERFACE_H