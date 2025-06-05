// clang-format off
#include "score_addon_puara.hpp"

#include <score/application/ApplicationContext.hpp>
#include <score/plugins/Interface.hpp>
#include <score/plugins/qt_interfaces/FactoryInterface_QtInterface.hpp>
#include <score/plugins/qt_interfaces/PluginRequirements_QtInterface.hpp>

#include <score_plugin_engine.hpp>
#include <score/plugins/FactorySetup.hpp>

#include <Avnd/Factories.hpp>

namespace puara_gestures::objects { struct GestureRecognizer; } 


namespace puara_gestures::objects { struct LeakyIntegratorAvnd; } 


namespace puara_gestures::objects { struct Roll; } 


namespace puara_gestures::objects { struct Tilt; } 


namespace puara_gestures::objects { struct Jab; } 


namespace puara_gestures::objects { struct Shake; } 



score_addon_puara::score_addon_puara() = default;
score_addon_puara::~score_addon_puara() = default;

std::vector<score::InterfaceBase*> score_addon_puara::factories(
        const score::ApplicationContext& ctx,
        const score::InterfaceKey& key) const
{
  std::vector<score::InterfaceBase*> fx;

  
::oscr::custom_factories<puara_gestures::objects::GestureRecognizer>(fx, ctx, key); 


::oscr::custom_factories<puara_gestures::objects::LeakyIntegratorAvnd>(fx, ctx, key); 


::oscr::custom_factories<puara_gestures::objects::Roll>(fx, ctx, key); 


::oscr::custom_factories<puara_gestures::objects::Tilt>(fx, ctx, key); 


::oscr::custom_factories<puara_gestures::objects::Jab>(fx, ctx, key); 


::oscr::custom_factories<puara_gestures::objects::Shake>(fx, ctx, key); 



  return fx;
}

std::vector<score::PluginKey> score_addon_puara::required() const
{
  return {score_plugin_engine::static_key()};
}

#include <score/plugins/PluginInstances.hpp>
SCORE_EXPORT_PLUGIN(score_addon_puara)
