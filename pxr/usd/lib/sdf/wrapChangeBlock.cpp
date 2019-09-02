//
// Copyright 2016 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
//
#include "pxr/pxr.h"
#include "pxr/usd/sdf/changeBlock.h"
#include "pxr/usd/sdf/changeManager.h"
#include "pxr/base/tf/diagnostic.h"
#include "pxr/base/tf/pyUtils.h"

#include <boost/python.hpp>

using namespace boost::python;

PXR_NAMESPACE_USING_DIRECTIVE

namespace {

class Sdf_PythonChangeBlock {
public:
    // #nv begin #fast-updates
    Sdf_PythonChangeBlock(bool fastUpdates=false) : _block(0), _fastUpdates(fastUpdates)
    // nv end
    {
        // Do nothing.
    }

    ~Sdf_PythonChangeBlock()
    {
        delete _block;
    }

    void Open()
    {
        if (!TF_VERIFY(_block == 0)) {
            return;
        }
        // #nv begin #fast-updates
        _block = new SdfChangeBlock(_fastUpdates);
        // nv end
    }

    void Close(object, object, object)
    {
        if (!TF_VERIFY(_block != 0)) {
            return;
        }
        delete _block;
        _block = 0;
    }

private:
    SdfChangeBlock* _block;

    // #nv begin #fast-updates
    bool _fastUpdates;
    // nv end
};

// #nv begin #fast-updates
static
void
_BeginBlock(bool fastUpdates)
{
    Sdf_ChangeManager::Get().OpenChangeBlock(fastUpdates);
}
// nv end

static
void
_EndBlock()
{
    Sdf_ChangeManager::Get().CloseChangeBlock();
}

} // anonymous namespace 

void
wrapChangeBlock()
{
    // This allows SdfChangeBlocks to be used in an RAII fashion in Python 
    // with the 'with' statement.
    typedef Sdf_PythonChangeBlock This;
    class_<This, boost::noncopyable>("ChangeBlock", init<>())
        // #nv begin #fast-updates
        .def(init<bool>(arg("fastUpdates")))
        // nv end
        .def("__enter__", &This::Open)
        .def("__exit__", &This::Close)
        ;

    // Helpers to open/close change blocks in a non-RAII fashion. Primarily
    // here for API compatibility, consumers should prefer the ChangeBlock
    // object above.
    // #nv begin #fast-updates
    def("BeginChangeBlock", &_BeginBlock, arg("fastUpdates")=false);
    // nv end
    def("EndChangeBlock", &_EndBlock);
}
