#ifndef CC_GFXGLES2_GLES2_TEXTURE_VIEW_H_
#define CC_GFXGLES2_GLES2_TEXTURE_VIEW_H_

CC_NAMESPACE_BEGIN

class GLES2GPUTextureView;

class CC_GLES2_API GLES2TextureView : public GFXTextureView {
public:
  GLES2TextureView(GFXDevice* device);
  ~GLES2TextureView();
  
public:
  bool Initialize(const GFXTextureViewInfo& info);
  void Destroy();
  
  CC_INLINE GLES2GPUTextureView* gpu_tex_view() const { return gpu_tex_view_; }
  
private:
  GLES2GPUTextureView* gpu_tex_view_;
};

CC_NAMESPACE_END

#endif
