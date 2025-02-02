/****************************************************************************
 Copyright (c) 2019-2021 Xiamen Yaji Software Co., Ltd.

 http://www.cocos.com

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
****************************************************************************/

#include "base/CoreStd.h"

#include "GFXSwapchain.h"
#include "GFXTexture.h"

namespace cc {
namespace gfx {

Texture::Texture()
: GFXObject(ObjectType::TEXTURE) {
}

Texture::~Texture() = default;

uint32_t Texture::computeHash(const TextureInfo &info) {
    uint32_t seed = 10;
    seed ^= toNumber(info.type) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= toNumber(info.usage) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= toNumber(info.format) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= toNumber(info.flags) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= toNumber(info.samples) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= (info.width) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= (info.height) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= (info.depth) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= (info.layerCount) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= (info.levelCount) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    return seed;
}

uint32_t Texture::computeHash(const TextureViewInfo &info) {
    uint32_t seed = 7;
    seed ^= info.texture->getHash() + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= toNumber(info.type) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= toNumber(info.format) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= (info.baseLevel) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= (info.levelCount) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= (info.baseLayer) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= (info.layerCount) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    return seed;
}

uint32_t Texture::computeHash(const Texture *texture) {
    uint32_t hash = texture->isTextureView() ? computeHash(texture->getViewInfo()) : computeHash(texture->getInfo());
    if (texture->_swapchain) hash ^= (texture->_swapchain->getObjectID()) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    return hash;
}

void Texture::initialize(const TextureInfo &info) {
    _info = info;
    _size = formatSize(_info.format, _info.width, _info.height, _info.depth);
    _hash = computeHash(info);

    _viewInfo.texture    = this;
    _viewInfo.format     = _info.format;
    _viewInfo.type       = _info.type;
    _viewInfo.baseLayer  = 0;
    _viewInfo.layerCount = _info.layerCount;
    _viewInfo.baseLevel  = 0;
    _viewInfo.levelCount = _info.levelCount;

    doInit(info);
}

void Texture::initialize(const TextureViewInfo &info) {
    _info     = info.texture->getInfo();
    _viewInfo = info;

    _isTextureView = true;
    _size          = formatSize(_info.format, _info.width, _info.height, _info.depth);
    _hash          = computeHash(info);

    doInit(info);
}

void Texture::resize(uint32_t width, uint32_t height) {
    if (_info.width != width || _info.height != height) {
        uint32_t size = formatSize(_info.format, width, height, _info.depth);
        doResize(width, height, size);

        _info.width  = width;
        _info.height = height;
        _size        = size;
        _hash        = computeHash(this);
    }
}

void Texture::destroy() {
    doDestroy();

    _info     = TextureInfo();
    _viewInfo = TextureViewInfo();

    _isTextureView = false;
    _size = _hash = 0;
}

///////////////////////////// Swapchain Specific /////////////////////////////

void Texture::initialize(const SwapchainTextureInfo &info, Texture *out) {
    out->_info.type       = TextureType::TEX2D;
    out->_info.format     = info.format;
    out->_info.width      = info.width;
    out->_info.height     = info.height;
    out->_info.layerCount = 1;
    out->_info.levelCount = 1;
    out->_info.depth      = 1;
    out->_info.samples    = SampleCount::ONE;
    out->_info.flags      = TextureFlagBit::NONE;
    out->_info.usage      = GFX_FORMAT_INFOS[toNumber(info.format)].hasDepth
                                ? TextureUsageBit::DEPTH_STENCIL_ATTACHMENT
                                : TextureUsageBit::COLOR_ATTACHMENT;

    out->_swapchain = info.swapchain;
    out->_size      = formatSize(info.format, info.width, info.height, 1);
    out->_hash      = computeHash(out);

    out->_viewInfo.texture    = out;
    out->_viewInfo.format     = out->_info.format;
    out->_viewInfo.type       = out->_info.type;
    out->_viewInfo.baseLayer  = 0;
    out->_viewInfo.layerCount = out->_info.layerCount;
    out->_viewInfo.baseLevel  = 0;
    out->_viewInfo.levelCount = out->_info.levelCount;

    out->doInit(info);
}

} // namespace gfx
} // namespace cc
