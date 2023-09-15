# Copyright (c) 2015 Nordic Semiconductor. All Rights Reserved.
#
# The information contained herein is property of Nordic Semiconductor ASA.
# Terms and conditions of usage are described in detail in NORDIC
# SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
#
# Licensees are granted free, non-transferable use of the information. NO
# WARRANTY of ANY KIND is provided. This heading must NOT be removed from
# the file.

import binascii
import tempfile
import shutil
import os

from nrfsdk.calliope import base
from nrfsdk.core import log

from nordicsemi.dfu.package import Package


class Hash(base.Command):
    """Generate a hash for the firmware file provided"""

    @staticmethod
    def Args(parser):
        """Args is called by calliope to gather arguments for this command.

        Args:
          parser: An argparse parser that you can use to add arguments that go
              on the command line after this command. Positional arguments are
              allowed.
        """
        parser.add_argument('firmware', nargs=1, type=str,
                            help='The firmware file to run hash calculation on. Can be in .hex or .bin format.')

    def Run(self, args):
        """Calculates a hash for the provided file. The file is converted to .bin if it is a .hex file.

        Args:
          args: argparse.Namespace, The arguments that this command was invoked
              with.

        Returns:
          The result of the edit operation, or None if it is cancelled.
        """

        firmware_path = args.firmware[0]


        work_directory = tempfile.mkdtemp(prefix="nrf_", suffix="_dfu")
        Package.normalize_firmware_to_bin(work_directory, firmware_path)

        firmware_bin_filename = os.path.basename(firmware_path)
        firmware_bin_filename = firmware_bin_filename.replace(".hex", ".bin")
        firmware_bin_path = os.path.join(work_directory, firmware_bin_filename)

        firmware_hash = Package.calculate_sha256_hash(firmware_bin_path)
        shutil.rmtree(work_directory)
        return firmware_hash

    def Display(self, args, result):
        """Display prints information about what just happened to stdout.

        Args:
            unused_args: The same as the args in Run.
            result: The results of the Run() method.
        """
        if result is None:
            return

        firmware_path = args.firmware[0]

        result = binascii.hexlify(result)

        log_message = "Calculated hash for {0}: {1}".format(firmware_path, result)
        log.out.Print(log_message)
