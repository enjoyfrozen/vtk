"""Test getting __version__ for VTK package
"""

import sys
import vtkmodules
from vtkmodules.vtkCommonCore import VTK_MAJOR_VERSION, VTK_MINOR_VERSION, VTK_BUILD_VERSION
from vtkmodules.test import Testing

class TestVersion(Testing.vtkTest):
    def testVersionAttribute(self):
        """Test VTK_VERSION macro and __version__ attribute
        """
        x,y,z = vtkmodules.__version__.split('.')
        self.assertEqual(x, str(VTK_MAJOR_VERSION))
        self.assertEqual(y, str(VTK_MINOR_VERSION))
        if z[0:3] == 'dev':
            self.assertEqual(z[3:], str(VTK_BUILD_VERSION))
        else:
            self.assertEqual(z, str(VTK_BUILD_VERSION))

if __name__ == "__main__":
    Testing.main([(TestVersion, 'test')])
