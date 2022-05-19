# Copyright (c) 2015 Nordic Semiconductor. All Rights Reserved.
#
# The information contained herein is property of Nordic Semiconductor ASA.
# Terms and conditions of usage are described in detail in NORDIC
# SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
#
# Licensees are granted free, non-transferable use of the information. NO
# WARRANTY of ANY KIND is provided. This heading must NOT be removed from
# the file.

"""Command to print version information.
"""

import textwrap

from nrfsdk.calliope import base
from nrfsdk.core import config
from nrfsdk.core import log


class Version(base.Command):
    """Print version information.
    """

    def Run(self, args):
        return None

    def Display(self, args, result):
        log.Print(textwrap.dedent("""\
nrf version {nrf_version}
""".format(nrf_version=config.NRF_SDK_VERSION)))
