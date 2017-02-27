# coding=utf-8
from __future__ import absolute_import

# inspired from https://github.com/imrahil/OctoPrint-NavbarTemp/blob/master/octoprint_navbartemp/__init__.py

import octoprint.plugin
import octoprint.printer
from octoprint.util import RepeatedTimer
import sys
import re
from datadog import statsd

class StatsdPlugin(octoprint.plugin.StartupPlugin):

    def __init__(self):
        self.interval = 30.0
        
        # send a first event
        statsd.event('Station start', 'Octoprint started')
       
    def on_after_startup(self):
        self._logger.info("Starting the stats plugin") 
        self.startTimer()
                
    def startTimer(self):
        self._checkTempTimer = RepeatedTimer(self.interval, self.collectMetrics, None, None, True)
        self._checkTempTimer.start()

    def collectPrinterTemperatures(self):
        try:
            temps = self._printer.get_current_temperatures()
            if len(temps) > 0:
                self._logger.info("temps are %s" % temps)
                statsd.gauge('printer.temperatures.bed.actual', temps['bed']['actual'])
                statsd.gauge('printer.temperatures.bed.target', temps['bed']['target'])
                statsd.gauge('printer.temperatures.extruder.actual', temps['tool0']['actual'])
                statsd.gauge('printer.temperatures.extruder.target', temps['tool0']['target'])
        except:
            self._logger.error("Unexpected error: %s" % (sys.exc_info()[0]))

    def collectMetrics(self):
        self._logger.info("Collecting metrics")
        self.collectPrinterTemperatures()
        if sys.platform == "linux2":
            with file("/sys/class/thermal/thermal_zone0/temp") as f:
                s = f.read()
                try:
                    statsd.gauge('system.cpu.temperature', ((float(s) / 1000.0 - 32) * 5 / 9))
                except:
                    self._logger.error("Unexpected error: %s" % (sys.exc_info()[0]))
                                    
    
__plugin_name__ = "Statsd plugin"
__plugin_version__ = "1.0.0"
__plugin_description__ = "A plugin to send all kind of metrics to statsd"
__plugin_implementation__ = StatsdPlugin() 
