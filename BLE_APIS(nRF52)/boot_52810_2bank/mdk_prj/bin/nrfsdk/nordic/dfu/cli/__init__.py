# Copyright (c) 2015 Nordic Semiconductor. All Rights Reserved.
#
# The information contained herein is property of Nordic Semiconductor ASA.
# Terms and conditions of usage are described in detail in NORDIC
# SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
#
# Licensees are granted free, non-transferable use of the information. NO
# WARRANTY of ANY KIND is provided. This heading must NOT be removed from
# the file.

"""The DFU group."""

from nrfsdk.calliope import base


class DFU(base.Group):
    """Manage the DFU features in Nordic SDK.

    This set of commands support, public/private key generation, public key export to C header file, .dat file generation, hash generation of firmware files, automatic conversion of .hex files to .bin files, Nordic DFU OTA package generation for distribution to applications.
  """

    detailed_help = {
        'DESCRIPTION': '{description}',
        'EXAMPLES': """\
          TODO

            $ {command} genkey
          """,
    }

