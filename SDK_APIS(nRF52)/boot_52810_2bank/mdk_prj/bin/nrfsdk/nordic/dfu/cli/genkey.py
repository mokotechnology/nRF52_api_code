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
from nrfsdk.exceptions import nRFNotImplementedException


class GenKey(base.Command):
    """Generate key and store it in a keystore"""

    @staticmethod
    def Args(parser):
        """Args is called by calliope to gather arguments for this command.

        Args:
          parser: An argparse parser that you can use to add arguments that go
              on the command line after this command. Positional arguments are
              allowed.
        """
        parser.add_argument('--name', required=True, type=str,
                            help='Name of key to generate')

    def Run(self, args):
        """Generate assymetric key.

        Args:
          args: argparse.Namespace, The arguments that this command was invoked
              with.

        Returns:
          The result of the edit operation, or None if it is cancelled.
        """
        raise nRFNotImplementedException()

    def Display(self, unused_args, result):
        """Display prints information about what just happened to stdout.

        Args:
          unused_args: The same as the args in Run.
          result: The results of the Run() method.
        """
        if result is None:
            return

