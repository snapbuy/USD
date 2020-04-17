//
// Copyright 2020 Pixar
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
#ifndef PXR_IMAGING_HD_ST_TEXTURE_HANDLE_REGISTRY_H
#define PXR_IMAGING_HD_ST_TEXTURE_HANDLE_REGISTRY_H

#include "pxr/pxr.h"
#include "pxr/imaging/hdSt/api.h"

#include "pxr/imaging/hdSt/textureObject.h"

#include "pxr/imaging/hd/enums.h"

#include <tbb/concurrent_vector.h>

#include <set>
#include <memory>

PXR_NAMESPACE_OPEN_SCOPE

class Hgi;

class HdStTextureIdentifier;
class HdStSamplerParameters;
class HdSt_SamplerObjectRegistry;

using HdStTextureHandlePtr =
    std::weak_ptr<class HdStTextureHandle>;
using HdStTextureHandleSharedPtr =
    std::shared_ptr<class HdStTextureHandle>;
using HdStTextureObjectPtr =
    std::weak_ptr<class HdStTextureObject>;
using HdStTextureObjectSharedPtr =
    std::shared_ptr<class HdStTextureObject>;
using HdStSamplerObjectSharedPtr =
    std::shared_ptr<class HdStSamplerObject>;
using HdStShaderCodePtr =
    std::weak_ptr<class HdStShaderCode>;
using HdStShaderCodeSharedPtr =
    std::shared_ptr<class HdStShaderCode>;

/// \class HdSt_TextureHandleRegistry
///
/// Keeps track of texture handles and allocates the textures and
/// samplers using the HdSt_TextureObjectRegistry, respectively,
/// HdSt_SamplerObjectRegistry.  Its responsibilities including
/// tracking what texture handles are associated to a texture,
/// computing the target memory of a texture from the memory requests
/// in the texture handles, triggering sampler and texture garbage
/// collection, and determining what HdStShaderCode instances are
/// affecting by (re-)committing a texture.
///
class HdSt_TextureHandleRegistry final
{
public:
    HdSt_TextureHandleRegistry();
    ~HdSt_TextureHandleRegistry();

    /// Set Hgi instance
    void SetHgi(Hgi* hgi);

    /// Allocate texture handle (thread-safe).
    ///
    /// See HdStResourceRegistry::AllocateTextureHandle for details.
    ///
    HDST_API
    HdStTextureHandleSharedPtr AllocateTextureHandle(
        const HdStTextureIdentifier &textureId,
        HdTextureType textureType,
        const HdStSamplerParameters &samplerParams,
        /// memoryRequest in bytes.
        size_t memoryRequest,
        bool createBindlessHandle,
        HdStShaderCodePtr const &shaderCode);

    /// Mark texture dirty (thread-safe).
    /// 
    /// If set, the target memory of the texture will be recomputed
    /// during commit and the data structure tracking the associated
    /// handles will be updated potentially triggering texture garbage
    /// collection.
    ///
    void MarkDirty(HdStTextureObjectPtr const &texture);

    /// Mark shader dirty (thread-safe).
    ///
    /// If set, the shader is scheduled to be updated (i.e., have its
    /// ComputeBufferSourcesFromTextures called) on the next commit.
    ///
    void MarkDirty(HdStShaderCodePtr const &shader);

    /// Mark that sampler garbage collection needs to happen during
    /// next commit (thead-safe).
    ///
    void MarkSamplerGarbageCollectionNeeded();

    /// Get sampler object registry.
    ///
    HdSt_SamplerObjectRegistry * GetSamplerObjectRegistry() const {
        return _samplerObjectRegistry.get();
    }

    /// Commit textures. Return shader code instances that
    /// depend on the (re-)loaded textures so that they can add
    /// buffer sources based on the texture meta-data.
    ///
    /// Also garbage collect textures and samplers if necessary.
    ///
    std::set<HdStShaderCodeSharedPtr> Commit();

private:
    void _ComputeMemoryRequest(HdStTextureObjectSharedPtr const &);
    void _ComputeMemoryRequests(const std::set<HdStTextureObjectSharedPtr> &);

    bool _GarbageCollectHandlesAndComputeTargetMemory();
    void _GarbageCollectAndComputeTargetMemory();
    std::set<HdStShaderCodeSharedPtr> _Commit();

    class _TextureToHandlesMap;

    bool _samplerGarbageCollectionNeeded;

    // Handles that are new or for which the underlying texture has
    // changed: samplers might need to be (re-)allocated and the
    // corresponding shader code might need to update the shader bar.
    tbb::concurrent_vector<HdStTextureHandlePtr> _dirtyHandles;

    // Textures whose set of associated handles and target memory
    // might have changed.
    tbb::concurrent_vector<HdStTextureObjectPtr> _dirtyTextures;

    // Shaders that dropped a texture handle also need to be notified
    // (for example because they re-allocated the shader bar after dropping
    // the texture).
    tbb::concurrent_vector<HdStShaderCodePtr> _dirtyShaders;

    std::unique_ptr<_TextureToHandlesMap> _textureToHandlesMap;
    std::unique_ptr<class HdSt_SamplerObjectRegistry> _samplerObjectRegistry;
    std::unique_ptr<class HdSt_TextureObjectRegistry> _textureObjectRegistry;

};

PXR_NAMESPACE_CLOSE_SCOPE

#endif