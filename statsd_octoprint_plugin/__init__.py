# coding=utf-8
from __future__ import absolute_import

# inspired from https://github.com/imrahil/OctoPrint-NavbarTemp/blob/master/octoprint_navbartemp/__init__.py

import octoprint.plugin
from octoprint.util import RepeatedTimer
import sys
import re
from datadog import initialize
from datadog import statsd

class StatsdPlugin(octoprint.plugin.StartupPlugin):

    def __init__(self):
        self.isRaspi = False
        self.debugMode = True      # to simulate temp on Win/Mac
        self.interval = 30.0
        
        # send a first event
        statsd.increment("station.restarts")
       
        
    def on_after_startup(self):
        self._logger.info("Hello World!") 
        if sys.platform == "linux2":
            with open('/proc/cpuinfo', 'r') as infile:
                cpuinfo = infile.read()
                # Match a line like 'Hardware   : BCM2709'
            match = re.search('^Hardware\s+:\s+(\w+)$', cpuinfo, flags=re.MULTILINE | re.IGNORECASE)

            if match is None:
                # Couldn't find the hardware, assume it isn't a pi.
                self.isRaspi = False
            elif match.group(1) == 'BCM2708':
                self._logger.debug("Pi 1")
                self.isRaspi = True
            elif match.group(1) == 'BCM2709':
                self._logger.debug("Pi 2")
                self.isRaspi = True

            if self.isRaspi and self.displayRaspiTemp:
                self._logger.info("Let's start RepeatedTimer!")
                self.startTimer()
                
        elif self.debugMode:
            self._logger.info("Let's start RepeatedTimer!")
            self.isRaspi = True
            self.startTimer()

    def startTimer(self):
        self._checkTempTimer = RepeatedTimer(self.interval, self.collectMetrics, None, None, True)
        self._checkTempTimer.start()

    def collectMetrics(self):
        self._logger.info("Collecting metrics")

        if sys.platform == "linux2":
            p = run("/opt/vc/bin/vcgencmd measure_temp", stdout=Capture())
            p = p.stdout.text

        elif self.debugMode:
            import random
            def randrange_float(start, stop, step):
                return random.randint(0, int((stop - start) / step)) * step + start
            p = "temp=%s'C" % randrange_float(5, 60, 0.1)
            self._logger.debug("response from sarge: %s" % p)

            match = re.search('=(.*)\'', p)
   
            if not match:
                self.isRaspi = False
            else:
                temp = match.group(1)
                self._logger.info("match: %s" % temp)
                statsd.gauge('temperature.cpu', float(temp))
                

__plugin_name__ = "Statsd plugin"
__plugin_version__ = "1.0.0"
__plugin_description__ = "A plugin to send all kind of metrics to statsd"
__plugin_implementation__ = StatsdPlugin() 
