"""Test getting __version__ for VTK package
"""

import sys
import vtkmodules
from vtkmodules.vtkCommonCore import VTK_VERSION
from vtkmodules.test import Testing

class TestVersion(Testing.vtkTest):
    def testVersionAttribute(self):
        """Test VTK_VERSION macro and __version__ attribute
        """
        self.assertEqual(vtkmodules.__version__, VTK_VERSION)

if __name__ == "__main__":
    Testing.main([(TestVersion, 'test')])
