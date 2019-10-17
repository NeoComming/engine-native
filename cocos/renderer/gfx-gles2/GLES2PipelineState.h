#ifndef CC_GFXGLES2_GLES2_PIPELINE_STATE_H_
#define CC_GFXGLES2_GLES2_PIPELINE_STATE_H_

CC_NAMESPACE_BEGIN

class GLES2GPUPipelineState;

class CC_GLES2_API GLES2PipelineState : public GFXPipelineState {
 public:
  GLES2PipelineState(GFXDevice* device);
  ~GLES2PipelineState();
  
 public:
  bool Initialize(const GFXPipelineStateInfo& info);
  void Destroy();

  CC_INLINE GLES2GPUPipelineState* gpu_pso() const { return gpu_pso_; }
  
 private:
  GLES2GPUPipelineState* gpu_pso_;
};

CC_NAMESPACE_END

#endif
