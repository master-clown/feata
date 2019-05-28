#pragma once

#include "service.h"

#if defined(_WIN32)
    #if defined(FEATA_APP)
        #define FEATA_PLUGIN_API _declspec(dllimport)
    #else
        #define FEATA_PLUGIN_API _declspec(dllexport)
    #endif
#else
    #define FEATA_PLUGIN_API
#endif

/**
  * Every plugin must contain these two routines, either in
  * default definition, or in custom.
  */
#define DECL_CREATE_FREE_PLUG_ROUTINES          \
    extern "C" FEATA_PLUGIN_API IPlugin* CreatePlugin();         \
    extern "C" FEATA_PLUGIN_API void     FreePlugin(IPlugin* p);
#define DEF_CREATE_FREE_PLUG_ROUTINES(PlugT)    \
    extern "C" FEATA_PLUGIN_API IPlugin* CreatePlugin() { return new PlugT(); }  \
    extern "C" FEATA_PLUGIN_API void FreePlugin(IPlugin* p) { if(p) delete p; }

class IPlugin;

using PluginCreateCb = IPlugin*(*)();
using PluginFreeCb = void(*)(IPlugin*);

class IPlugin
{
public:
    IPlugin() = default;
    virtual ~IPlugin() = default;

    virtual int Init(const ServiceInfo service_lst[],
                     const unsigned int service_num) = 0;

    /**
     * \details
     *     Returns a list of settings which are required for
     *     the plugin and formatted as JSON array. Every entry
     *     denotes a setting and may consist of following fields:
     *
     *      "codename":
     *          short name for the setting (by C++ variable naming rules)
     *      "fullname":
     *          full name of the setting, which would be displayed to user
     *      "desc": (OPT)
     *          setting description for user
     *      "type":
     *          type of the setting's value. See list of available types below
     *      "value":
     *          for "geom_sel"-like types: ignored;
     *          for "strchoice" type:      zero-based index of default selection;
     *          for other types:           default value of the setting
     *      "strchoice_lst": (FOR "strchoice" TYPE ONLY)
     *          array of strings, between which user will make a choice
     *      "geom_select_type": (FOR TYPES ASSOCIATED WITH "geom_select" ONLY)
     *          geometric entity type, which is allowed for selection.
     *          See list of available types below
     *
     *
     *     Input of the keywords described below is case insensitive.
     *     Whitespaces are not forbidden.
     *
     *     Available types:
     *      "bool", "int", "real", "string":
     *          no comment
     *      "strchoice":
     *          list of strings, represented in GUI by ComboBoxes.
     *          STRING: zero-based index of selected choice (it's
     *          assumed the plugin knows available choices per se)
     *      "geom_select"
     *          selection of geometric entities of type specified
     *          in the "geom_select_type" field. An entry of a
     *          selection is represented by an integer ID; a selection
     *          is represented as $-separated list of integers-IDs.
     *          STRING: <geom_select_type>~<geom_select list>
     *
     ***********WARNING! ID of geometry entity is one-based!
     *      "geom_select_string":
     *          the same as previous, but every entity is mapped to a
     *          string. Described as comma-separated list:
     *              "<ID1>@<str1>$<ID2>@<real2>$<...>"
     *          STRING: <geom_select_type>~<geom_select_string list>
     *
     *     Available types for "geom_select_types" field:
     *      "vertex", "edge", "face", "solid"
     */
    virtual const char* ListRequiredSettings() const = 0;

protected:

    /**
     * \details
     *     The main program provides the plugin with some services,
     *     which may be invoked via this routine. All possible input
     *     or output must be transfered through strings.
     *
     ******All functions return zero on success and a POSITIVE non-zero otherwise,
     *     so one can use negative codes for own purposes.
     *
     ******WARNING! All non-null output data must be freed by 'free_output_data()' call!
     *
     *     List of available services. 'i' - expects input, 'o' - output;
     *     key '[OPT]' stands for 'OPTIONAL' and may be omitted.
     *
     *      log*(i):
     *          family of logging functions. '*' might be:
     *              'i'       - low-priority misc information;
     *              <nothing> - simple message;
     *              'w'       - warning message;
     *              'e'       - error message
     *
     *      init_progress_bar(i):
     *          requests to set progress bar to min-max-current values.
     *
     *          Input: string in format
     *              'min:<uint>, max:<uint>, [OPT]cur:<uint>',
     *              where
     *                  'min' & 'max':
     *                      if equal, sets the progress bar to indefinite await type.
     *                  'cur':
     *                      initial progress value. Must be between min and max.
     *
     *      set_progress_value(i):
     *          sets progress bar value to 'arg_size' (yes, pass the progress value
     *          through this argument. 'service_args' is arbitrary). Must be between
     *          previously passed 'min' and 'max', ignored otherwise.
     *
     *      hide_progress_bar():
     *          requests to hide the progress bar. Has no effect when the latter
     *          never been shown. No arguments required.
     *
     *      get_setting_value(io):
     *          fetch value of setting.
     *
     *          Input: codename of the setting.
     *          Output: its value in the format described
     *                  in 'ListRequiredSettings()' for all
     *                  possible types of settings
     *
     *      free_output_data(i):
     *          called to release output data, which has been
     *          allocated by the main program. Input: pointer
     *          of output data placed in the argument 'service_args',
     *          and any value for other arguments (zeros, if
     *          you'd like)
     */
    virtual int CallService(const char* service_name,
                            const char* service_args,
                            const unsigned int arg_size,
                            char** result = nullptr,
                            unsigned int* res_size = nullptr) = 0;
};
