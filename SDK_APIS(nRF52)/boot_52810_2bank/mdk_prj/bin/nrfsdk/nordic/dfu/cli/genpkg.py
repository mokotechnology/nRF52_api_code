# Copyright (c) 2015 Nordic Semiconductor. All Rights Reserved.
#
# The information contained herein is property of Nordic Semiconductor ASA.
# Terms and conditions of usage are described in detail in NORDIC
# SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
#
# Licensees are granted free, non-transferable use of the information. NO
# WARRANTY of ANY KIND is provided. This heading must NOT be removed from
# the file.

from nrfsdk.calliope import base
from nrfsdk.core import log
from nrfsdk.exceptions import nRFException
from nordicsemi.dfu.package import Package


class GenPkg(base.Command):
    """Generate a package for distribution to Apps supporting Nordic DFU OTA"""

    @staticmethod
    def Args(parser):
        """Args is called by calliope to gather arguments for this command.

        Args:
            parser: An argparse parser that you can use to add arguments that go
                on the command line after this command. Positional arguments are
                allowed.
        """

        # This will parse any string starting with 0x as base 16.
        auto_int = lambda x: int(x, 0)
        auto_float = lambda x: float(x)

        parser.add_argument('--dfu-ver', required=False, type=auto_float,
                            help='DFU packet version to use')
        parser.add_argument('--dev-type', required=False, type=auto_int,
                            help='Device type')
        parser.add_argument('--dev-revision', required=False, type=auto_int, metavar='DEV_REV',
                            help='Device revision')
        parser.add_argument('--application-version', required=False, type=auto_int, metavar='APP_VER',
                            help='Application version')
        parser.add_argument('--key', required=False, type=str,
                            help='Name of key to use for encrypting the package')
        parser.add_argument('--sd-req', required=False, type=str,
                            help="SoftDevice requirement. What SoftDevice "
                                 "is required to already be present on the target device. "
                                 "Should be a list of firmware IDs. Example: --sd-req 0x4F,0x5A. "
                                 "For an empty list use 'none'. "
                                 "See: http://developer.nordicsemi.com/nRF51_SDK/doc/7.2.0/s110/html/a00065.html")
        parser.add_argument('--application', required=False, type=str,
                            help='The application firmware file')
        parser.add_argument('--bootloader', required=False, type=str, metavar='BL',
                            help='The bootloader firmware file')
        parser.add_argument('--softdevice', required=False, type=str, metavar='SD',
                            help='The SoftDevice firmware file')
        parser.add_argument('zipfile', nargs=1, type=str,
                            help='The package filename')

    def Run(self, args):
        """Calculates a hash for the provided file. The file is converted to .bin if it is a .hex file.

        Args:
            args: argparse.Namespace, The arguments that this command was invoked with.

        Returns:
            The result of the edit operation, or None if it is cancelled.
        """

        zipfile_path = args.zipfile[0]
        sd_req_list = None

        if args.sd_req == "none":
            sd_req_list = []

        elif args.sd_req:
            try:
                sd_req_list = []
                #This will parse any string starting with 0x as base 16.
                auto_int = lambda x: int(x, 0)
                sd_req_list = args.sd_req.split(',')
                sd_req_list = map(auto_int, sd_req_list)
            except ValueError, e:
                raise nRFException("Could not parse value for --sd-req. "
                                   "Hex values should be prefixed with 0x.")

        package = Package(args.dev_type,
                          args.dev_revision,
                          args.application_version,
                          args.key,
                          sd_req_list,
                          args.application,
                          args.bootloader,
                          args.softdevice,
                          args.dfu_ver)

        package.generate_package(zipfile_path)

        return zipfile_path

    def Display(self, unused_args, result):
        """Display prints information about what just happened to stdout.

        Args:
            unused_args: The same as the args in Run.
            result: The results of the Run() method.
        """

        if result is None:
            return

        log_message = "Zip created at {0}".format(result)
        log.out.Print(log_message)
