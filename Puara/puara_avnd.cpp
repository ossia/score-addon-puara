// clang-format off
#include <ossia/detail/config.hpp>

#include "Puara/GestureRecognizer.hpp"
#include "Puara/LeakyIntegratorAvnd.hpp"

/* #undef AVND_REFLECTION_HELPERS */
#if defined(AVND_REFLECTION_HELPERS)
#include <avnd/common/aggregates.hpp>
#include <avnd/concepts/port.hpp>
#include <avnd/concepts/message.hpp>
#include <avnd/common/index.hpp>

#include ""

#include <avnd/concepts/generic.hpp>
#include <avnd/introspection/input.hpp>
#include <avnd/introspection/output.hpp>
#include <avnd/introspection/messages.hpp>

#include ""
#endif

#include <Avnd/Factories.hpp>

namespace oscr
{
template <>
void custom_factories<puara_gestures::objects::LeakyIntegratorAvnd>(
    std::vector<score::InterfaceBase*>& fx,
    const score::ApplicationContext& ctx, const score::InterfaceKey& key)
{
  using namespace oscr;
  oscr::instantiate_fx<puara_gestures::objects::LeakyIntegratorAvnd>(fx, ctx, key);
}
template <>
void custom_factories<puara_gestures::objects::GestureRecognizer>(
    std::vector<score::InterfaceBase*>& fx,
    const score::ApplicationContext& ctx, const score::InterfaceKey& key)
{
  oscr::instantiate_fx<puara_gestures::objects::GestureRecognizer>(fx, ctx, key);
}

}

// clang-format on
