# Copyright 2013 Google Inc. All Rights Reserved.

"""The super-group for the nRF CLI."""

from nrfsdk.calliope import actions
from nrfsdk.calliope import base
from nrfsdk.core import properties


class nRF(base.Group):
  """Manage nRF resources and developer workflow.

  The *nrf* CLI provides an easy to use interface for developers using Nordic Semiconductor products.
  """

  @staticmethod
  def Args(parser):
    # Must have a None default so properties are not always overridden when the
    # arg is not provided.
    quiet_arg = parser.add_argument(
        '--quiet',
        '-q',
        default=None,
        help='Disable all interactive prompts.',
        action=actions.StoreConstProperty(
            properties.VALUES.core.disable_prompts, True))
    quiet_arg.detailed_help = """\
        Disable all interactive prompts when running nrf commands. If input
        is required, defaults will be used, or an error will be raised.
        """
