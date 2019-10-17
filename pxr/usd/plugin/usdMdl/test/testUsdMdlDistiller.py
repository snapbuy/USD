#!/pxrpythonsubst
#
# Copyright 2018 Pixar
#
# Licensed under the Apache License, Version 2.0 (the "Apache License")
# with the following modification; you may not use this file except in
# compliance with the Apache License and the following modification to it:
# Section 6. Trademarks. is deleted and replaced with:
#
# 6. Trademarks. This License does not grant permission to use the trade
#    names, trademarks, service marks, or product names of the Licensor
#    and its affiliates, except as required to comply with Section 4(c) of
#    the License and to reproduce the content of the NOTICE file.
#
# You may obtain a copy of the Apache License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the Apache License with the above modification is
# distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied. See the Apache License for the specific
# language governing permissions and limitations under the Apache License.

import os
os.environ['PXR_USDMDL_PLUGIN_SEARCH_PATHS'] = os.path.join(os.getcwd(), 'mdl')

from pxr import UsdMdl
import unittest

class TestDistiller(unittest.TestCase):
    def test_Distiller(self):
        """
        Test MDL node discovery.
        """
        UsdMdl.TestDistill("::nvidia::vMaterials::Design::Wood::burlwood::burlwood");
        self.assertTrue(os.path.isfile("preview.usda"));
        self.assertTrue(os.path.isfile("burlwood-base_color.png"));
        self.assertTrue(os.path.isfile("burlwood-metallic.png"));
        self.assertTrue(os.path.isfile("burlwood-normal.png"));
        self.assertTrue(os.path.isfile("burlwood-roughness.png"));
        self.assertTrue(os.path.isfile("burlwood-specular.png"));

if __name__ == '__main__':
    unittest.main()
