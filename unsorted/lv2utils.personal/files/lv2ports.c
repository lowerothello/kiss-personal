/*
  Copyright 2007-2019 David Robillard <d@drobilla.net>

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THIS SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

/* very heavily edited */

#include <lilv/lilv.h>
#include "lv2/core/lv2.h"
#include "lv2/event/event.h"
#include "lv2/port-groups/port-groups.h"
// #include "lv2/presets/presets.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static LilvNode* applies_to_pred     = NULL;
static LilvNode* control_class       = NULL;
static LilvNode* audio_class         = NULL;
static LilvNode* input_class         = NULL;
static LilvNode* output_class        = NULL;
static LilvNode* event_class         = NULL;
static LilvNode* group_pred          = NULL;
static LilvNode* label_pred          = NULL;
static LilvNode* designation_pred    = NULL;
static LilvNode* supports_event_pred = NULL;

static void
print_usage(void)
{
  printf("Usage: lv2ports [OPTION]... PLUGIN_URI\n"
         "Print information about an installed LV2 plugin.\n\n"
         "  --audioout   Print the plugin's audio output ports\n"
         "  --audioin    Print the plugin's audio input ports\n"
         "  --controlout Print the plugin's control output ports\n"
         "  --controlin  Print the plugin's control input ports\n"
         "  --help       Display this help and exit\n");
}

int
main(int argc, char** argv)
{
  if (argc == 1) {
    print_usage();
    return 1;
  }

  int show_audioout  = 0;
  int show_audioin   = 0;
  int show_ctlout    = 0;
  int show_ctlin     = 0;
  const char* plugin_uri    = NULL;
  for (int i = 1; i < argc; ++i) {

    if (!strcmp(argv[i], "--help")) {
      print_usage();
      return 0;
    }

    if (!strcmp(argv[i], "--audioout")) {
      show_audioout = 1;
    } else if (!strcmp(argv[i], "--audioin")) {
      show_audioin = 1;
    } else if (!strcmp(argv[i], "--controlout")) {
      show_ctlout = 1;
    } else if (!strcmp(argv[i], "--controlin")) {
      show_ctlin = 1;
    } else if (argv[i][0] == '-') {
      print_usage();
      return 1;
    } else if (i == argc - 1) {
      plugin_uri = argv[i];
    }
  }

  int ret = 0;

  LilvWorld* world = lilv_world_new();
  lilv_world_load_all(world);

  LilvNode* uri = lilv_new_uri(world, plugin_uri);
  if (!uri) {
    fprintf(stderr, "Invalid plugin URI\n");
    lilv_world_free(world);
    return 1;
  }

  applies_to_pred     = lilv_new_uri(world, LV2_CORE__appliesTo);
  control_class       = lilv_new_uri(world, LILV_URI_CONTROL_PORT);
  audio_class         = lilv_new_uri(world, LILV_URI_AUDIO_PORT);
  input_class         = lilv_new_uri(world, LILV_URI_INPUT_PORT);
  output_class        = lilv_new_uri(world, LILV_URI_OUTPUT_PORT);
  event_class         = lilv_new_uri(world, LILV_URI_EVENT_PORT);
  group_pred          = lilv_new_uri(world, LV2_PORT_GROUPS__group);
  label_pred          = lilv_new_uri(world, LILV_NS_RDFS "label");
  designation_pred    = lilv_new_uri(world, LV2_CORE__designation);
  supports_event_pred = lilv_new_uri(world, LV2_EVENT__supportsEvent);

  const LilvPlugins* plugins = lilv_world_get_all_plugins(world);
  const LilvPlugin*  p       = lilv_plugins_get_by_uri(plugins, uri);

  if (p) {
    /* Ports */

    const uint32_t num_ports = lilv_plugin_get_num_ports(p);
    float*         mins      = (float*)calloc(num_ports, sizeof(float));
    float*         maxes     = (float*)calloc(num_ports, sizeof(float));
    float*         defaults  = (float*)calloc(num_ports, sizeof(float));
    lilv_plugin_get_port_ranges_float(p, mins, maxes, defaults);

      for (uint32_t i = 0; i < num_ports; ++i) {
        const LilvPort* port = lilv_plugin_get_port_by_index(p, i);

        if (!port) {
          printf("\t\tERROR: Illegal/nonexistent port\n");
        } else {
          if (show_audioout == 1 && lilv_port_is_a(p, port, audio_class) && lilv_port_is_a(p, port, output_class)) {
            const LilvNode* sym = lilv_port_get_symbol(p, port);
            printf("%s\n", lilv_node_as_string(sym));
          }

          if (show_audioin == 1 && lilv_port_is_a(p, port, audio_class) && lilv_port_is_a(p, port, input_class)) {
            const LilvNode* sym = lilv_port_get_symbol(p, port);
            printf("%s\n", lilv_node_as_string(sym));
          }

          if (show_ctlout == 1 && lilv_port_is_a(p, port, control_class) && lilv_port_is_a(p, port, output_class)) {
            const LilvNode* sym = lilv_port_get_symbol(p, port);
            printf("%s", lilv_node_as_string(sym));
            if (!isnan(mins[i])) {
              printf("\t%f", mins[i]);
            }
            if (!isnan(maxes[i])) {
              printf("\t%f", maxes[i]);
            }
            printf("\n");
          }

          if (show_ctlin == 1 && lilv_port_is_a(p, port, control_class) && lilv_port_is_a(p, port, input_class)) {
            const LilvNode* sym = lilv_port_get_symbol(p, port);
            printf("%s", lilv_node_as_string(sym));
            if (!isnan(mins[i])) {
              printf("\t%f", mins[i]);
            }
            if (!isnan(maxes[i])) {
              printf("\t%f", maxes[i]);
            }
            printf("\n");
          }
        }
      }

    free(mins);
    free(maxes);
    free(defaults);

  } else {
    fprintf(stderr, "Plugin not found.\n");
  }

  ret = (p != NULL ? 0 : -1);

  lilv_node_free(uri);

  lilv_node_free(supports_event_pred);
  lilv_node_free(designation_pred);
  lilv_node_free(label_pred);
  lilv_node_free(group_pred);
  lilv_node_free(event_class);
  lilv_node_free(control_class);
  lilv_node_free(audio_class);
  lilv_node_free(input_class);
  lilv_node_free(output_class);
  lilv_node_free(applies_to_pred);

  lilv_world_free(world);
  return ret;
}
