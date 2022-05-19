# Copyright (c) 2015 Nordic Semiconductor. All Rights Reserved.
#
# The information contained herein is property of Nordic Semiconductor ASA.
# Terms and conditions of usage are described in detail in NORDIC
# SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
#
# Licensees are granted free, non-transferable use of the information. NO
# WARRANTY of ANY KIND is provided. This heading must NOT be removed from
# the file.
from nordicsemi.dfu.dfu import Dfu
from nordicsemi.dfu.dfu_transport_serial import DfuTransportSerial

from nrfsdk.calliope import base
from nrfsdk.core import log


class SerialDfu(base.Command):
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
        parser.add_argument('--package', required=True, type=str,
                            help='The package filename')
        parser.add_argument('--port', '-p', required=True, type=str,
                            help='COM Port to which the device is connected.')
        parser.add_argument('--flowcontrol', '-fc', action='store_true', required=False,
                            help='Enable flow control, default: disabled.')
        parser.add_argument('--baudrate', '-b', type=int, required=False, default=38400,
                            help='Desired baudrate 38400/96000/115200/230400/250000/460800/921600/1000000 (default: 38400).'
                                 ' Note: Baud rates >115200 are supported by nRF51822,'
                                 ' but may not be supported by all RS232 devices on Windows.')
        parser.add_argument('--debug', '-d', required=False, action='store_true',
                            help='Enable debug output, default: disabled')

    def Run(self, args):
        """Programs a target with a DFU package.
        """

        serial_backend = DfuTransportSerial(args.port, args.baudrate, args.flowcontrol)
        dfu = Dfu(args.package, dfu_transport=serial_backend)

        log.out.Print("Upgrading target on {1} with DFU package {0}. Flow control is {2}."
                      .format(args.package, args.port, "enabled" if args.flowcontrol else "disabled"))

        try:
            dfu.dfu_send_images()
        except Exception as e:
            log.out.Print("")
            log.out.Print("Failed to upgrade target. Error is: {0}".format(e.message))
            log.out.Print("")
            log.out.Print("Possible causes:")
            log.out.Print("- bootloader, SoftDevice or application on target "
                          "does not match the requirements in the DFU package.")
            log.out.Print("- baud rate or flow control is not the same as in the target bootloader.")
            log.out.Print("- target is not in DFU mode. If using the SDK examples, "
                          "press Button 4 and RESET and release both to enter DFU mode.")

            return False

        return True

    def Display(self, unused_args, result):
        """Display prints information about what just happened to stdout.

        Args:
            unused_args: The same as the args in Run.
            result: The results of the Run() method.
        """

        if result is False:
            return

        log_message = "Device programmed."
        log.out.Print(log_message)
