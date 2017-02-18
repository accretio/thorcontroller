# coding=utf-8
from __future__ import absolute_import

import octoprint.plugin

class WorkshopPlugin(octoprint.plugin.TemplatePlugin,
                     octoprint.plugin.AssetPlugin):

    def get_template_configs(self):
		return [
         	dict(type="tab", template="workshop_tab.jinja2", custom_bindings=True)
		]

    def get_assets(self):
        return dict(
            js=['js/workshop.js' ],
            css=[ 'css/style.css' ]
        )

    
__plugin_name__ = "Assembly Station"
__plugin_version__ = "1.0.0"
__plugin_description__ = "A plugin to control the tools"
__plugin_implementation__ = WorkshopPlugin()
