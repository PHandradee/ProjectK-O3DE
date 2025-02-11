
#pragma once

#include <ProjectK/ProjectKTypeIds.h>

#include <AzCore/EBus/EBus.h>
#include <AzCore/Interface/Interface.h>

namespace ProjectK
{
    class ProjectKRequests
    {
    public:
        AZ_RTTI(ProjectKRequests, ProjectKRequestsTypeId);
        virtual ~ProjectKRequests() = default;
        // Put your public methods here
    };

    class ProjectKBusTraits
        : public AZ::EBusTraits
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        //////////////////////////////////////////////////////////////////////////
    };

    using ProjectKRequestBus = AZ::EBus<ProjectKRequests, ProjectKBusTraits>;
    using ProjectKInterface = AZ::Interface<ProjectKRequests>;

} // namespace ProjectK
