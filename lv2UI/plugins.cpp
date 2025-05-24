#include "plugins.h"
#include <QDebug>
#include <lv2/atom/atom.h>
#include <lv2/urid/urid.h>
#include <lv2/ui/ui.h>
#include <suil/suil.h>

#define LV2_PATH "/opt/homebrew/lib/lv2/"

LV2::Plugin::Manager::Manager(){
    qDebug("Init Plugin manager");
    _world = lilv_world_new();

    LilvNode *lv2Path = lilv_new_string(_world, LV2_PATH);
    lilv_world_set_option(_world, LILV_OPTION_LV2_PATH, lv2Path);
    lilv_node_free(lv2Path);
    lilv_world_load_all(_world);

    _portConnectionOptionalURI = lilv_new_uri(_world, LV2_CORE__connectionOptional);
    _hostType = lilv_new_uri(_world, LV2_UI__GtkUI);
}

void LV2::Plugin::Manager::refreshPlugins(){
    const LilvPlugins *plugins = lilv_world_get_all_plugins(_world);

    QList<Plugin::Description> list;
    LILV_FOREACH(plugins, i, plugins) {
        const LilvPlugin *p = lilv_plugins_get(plugins, i);
        list.append(createFromPlugin(p));
    }
    _plugins = list;
}

QList<LV2::Plugin::Description> LV2::Plugin::Manager::getPlugins(){
    return _plugins;
}


extern "C"{
static unsigned test(const char* container_type_uri, const char* ui_type_uri){
    return 0;
}
}

LV2::Plugin::Description LV2::Plugin::Manager::createFromPlugin(const LilvPlugin *p){
    Plugin::Description desc;
    LilvNode *nodeName = lilv_plugin_get_name(p);

    desc.name = lilv_node_as_string(nodeName);
    desc.uri = lilv_node_as_string(lilv_plugin_get_uri(p));

    lilv_node_free(nodeName);

    LilvNode *projectNode = lilv_plugin_get_project(p);
    desc.project = lilv_node_as_string(projectNode);
    lilv_node_free(projectNode);

    // ports
    for (uint32_t i = 0; i < lilv_plugin_get_num_ports(p); i++) {
        LV2::Plugin::Description::Port portDesc;
        const LilvPort *port = lilv_plugin_get_port_by_index(p, i);
        portDesc.optional = lilv_port_has_property(p, port, _portConnectionOptionalURI);

        LilvNode *portName = lilv_port_get_name(p, port);
        portDesc.name = lilv_node_as_string(portName);
        lilv_node_free(portName);


        bool isInput = false;
        bool isAudio = false;
        bool isAtom = false;

        const LilvNodes *portClasses = lilv_port_get_classes(p, port);
        LILV_FOREACH(nodes, pC, portClasses) {
            const LilvNode *portClass = lilv_nodes_get(portClasses, pC);
            const char *portClassName = lilv_node_as_string(portClass);

            bool handled = false;
            if (strcmp(portClassName, LV2_ATOM__AtomPort) == 0) {
                isAtom = true;
                handled = true;
            }
            if (strcmp(portClassName, LV2_CORE__ControlPort) == 0) {
                isAudio = false;
                handled = true;
            } else if (strcmp(portClassName, LV2_CORE__AudioPort) == 0) {
                isAudio = true;
                handled = true;
            }
            if (strcmp(portClassName, LV2_CORE__InputPort) == 0) {
                isInput = true;
                handled = true;
            } else if (strcmp(portClassName, LV2_CORE__OutputPort) == 0) {
                isInput = false;
                handled = true;
            }
            if (!handled) {
                printf("\t\t '%s'\n", portClassName);
                assert(false);
            }
        }
        portDesc.flow = isInput? LV2::Plugin::Description::Port::INPUT: LV2::Plugin::Description::Port::OUTPUT;
        portDesc.type = isAudio? LV2::Plugin::Description::Port::AUDIO: LV2::Plugin::Description::Port::CONTROL;

        // scale points
        LilvScalePoints* scalePoints = lilv_port_get_scale_points(p, port);

        LILV_FOREACH(scale_points, pS, scalePoints) {
            const LilvScalePoint* scalePoint = lilv_scale_points_get(scalePoints, pS);
            LV2::Plugin::Description::Port::ScalePoint scalePointDesc;

            scalePointDesc.label = lilv_node_as_string(lilv_scale_point_get_label(scalePoint));
            const LilvNode* nodeVal = lilv_scale_point_get_value(scalePoint);
            scalePointDesc.value = lilv_node_as_float(nodeVal);
            portDesc.scalePoints.append(scalePointDesc);
        }

        lilv_scale_points_free(scalePoints);
        desc.ports.append(portDesc);
    }
    // features
    LilvNodes *features = lilv_plugin_get_required_features(p);
    LILV_FOREACH(nodes, fI, features) {
        const LilvNode *nodeFeat = lilv_nodes_get(features, fI);
        const char *featName = lilv_node_as_string(nodeFeat);

        LV2::Plugin::Description::Feature featDesc;
        featDesc.uri = featName;
        featDesc.optional = false;
        desc.features.append(featDesc);
    }
    lilv_nodes_free(features);

    features = lilv_plugin_get_optional_features(p);
    LILV_FOREACH(nodes, fI, features) {
        const LilvNode *nodeFeat = lilv_nodes_get(features, fI);
        const char *featName = lilv_node_as_string(nodeFeat);

        LV2::Plugin::Description::Feature featDesc;
        featDesc.uri = featName;
        featDesc.optional = true;
        desc.features.append(featDesc);
    }
    lilv_nodes_free(features);

    LilvUIs* uis = lilv_plugin_get_uis(p);
    LILV_FOREACH(uis, fu, uis){
        const LilvUI* ui = lilv_uis_get(uis, fu);
        const LilvNode* uriNode = lilv_ui_get_uri(ui);

        LV2::Plugin::Description::UI uiDesc;
        uiDesc.uri = lilv_node_as_string(uriNode);

        const LilvNode* uiType   = NULL;
        uiDesc.supported = lilv_ui_is_supported(ui, suil_ui_supported,_hostType, &uiType);
        if (uiDesc.supported  && uiType != NULL){
            qDebug("supported ui: '%s'", lilv_node_as_string(uiType));
        }
        desc.uis.append(uiDesc);
    }
    lilv_uis_free(uis);
    return desc;
}


LV2::Plugin::Manager::~Manager(){
    lilv_world_free(_world);
}

LV2::Plugin::Plugin() {

}


LV2::Plugin::~Plugin(){

}
