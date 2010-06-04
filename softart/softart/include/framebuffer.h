#ifndef SOFTART_FRAMEBUFFER_H
#define SOFTART_FRAMEBUFFER_H

#include "enums.h"
#include "decl.h"
#include "colors.h"
#include "render_stage.h"
#include "shaderregs.h"

#include <vector>
#include "softart_fwd.h"
BEGIN_NS_SOFTART()

struct depth_stencil_op_desc {
    stencil_op stencil_fail_op;
    stencil_op stencil_depth_fail_op;
    stencil_op stencil_pass_op;
    compare_function stencil_func;

	depth_stencil_op_desc()
		: stencil_fail_op(stencil_op_keep),
			stencil_depth_fail_op(stencil_op_keep),
			stencil_pass_op(stencil_op_keep),
			stencil_func(compare_function_always){
	}
};

struct depth_stencil_desc {
    bool depth_enable;
    bool depth_write_mask;
    compare_function depth_func;
    bool stencil_enable;
    uint8_t stencil_read_mask;
    uint8_t stencil_write_mask;
    depth_stencil_op_desc front_face;
    depth_stencil_op_desc back_face;

	depth_stencil_desc()
		: depth_enable(true),
			depth_write_mask(true),
			depth_func(compare_function_less),
			stencil_enable(false),
			stencil_read_mask(0xFF), stencil_write_mask(0xFF){
	}
};

class depth_stencil_state {
	depth_stencil_desc desc_;

	typedef bool (*depth_test_func_type)(float ps_depth, float cur_depth);
	typedef bool (*stencil_test_func_type)(int32_t ref, int32_t cur_stencil);
	typedef int32_t (*stencil_op_func_type)(int32_t ref, int32_t cur_stencil);
	typedef void (*write_depth_func_type)(float depth, backbuffer_pixel_out& target_pixel);
	typedef void (*write_stencil_func_type)(int32_t stencil, backbuffer_pixel_out& target_pixel);

	depth_test_func_type depth_test_func_;
	stencil_test_func_type stencil_test_func_[2];
	stencil_op_func_type stencil_op_func_[6];
	write_depth_func_type write_depth_func_;
	write_stencil_func_type write_stencil_func_;

public:
	depth_stencil_state(const depth_stencil_desc& desc);
	const depth_stencil_desc& get_desc() const;

	bool depth_test(float ps_depth, float cur_depth) const;
	bool stencil_test(bool front_face, int32_t ref, int32_t cur_stencil) const;
	int32_t stencil_operation(bool front_face, bool depth_pass, bool stencil_pass, int32_t ref, int32_t cur_stencil) const;
	int32_t stencil_read(int32_t stencil) const;
	void write_depth(float depth, backbuffer_pixel_out& target_pixel) const;
	void write_stencil(int32_t stencil, backbuffer_pixel_out& target_pixel) const;
};

class framebuffer : public render_stage
{
private:
	std::vector<boost::shared_ptr<surface> > back_cbufs_;
	boost::shared_ptr<surface> dbuf_;
	boost::shared_ptr<surface> sbuf_;
	
	std::vector<bool> buf_valids;
	std::vector<surface* > cbufs_; //framebuffer没有释放surface的权力

	size_t width_, height_;
	pixel_format fmt_;

	bool check_buf(surface* psurf);

public:
	//inherited
	void initialize(renderer_impl* pparent);

	framebuffer(size_t width, size_t height, pixel_format fmt);
	~framebuffer(void);

	//重置
	void reset(size_t width, size_t height, pixel_format fmt);

	//渲染目标设置
	void set_render_target_disabled(render_target tar, size_t tar_id);
	void set_render_target_enabled(render_target tar, size_t tar_id);

	void set_render_target(render_target tar, size_t tar_id, surface* psurf);
	surface* get_render_target(render_target tar, size_t tar_id) const;

	//获得渲染状态	
	efl::rect<size_t> get_rect();
	size_t get_width() const;
	size_t get_height() const;
	pixel_format get_buffer_format() const;

	//渲染
	void render_pixel(const h_blend_shader& hbs, size_t x, size_t y, const ps_output& ps);

	//清理
	void clear_color(size_t tar_id, const color_rgba32f& c);
	void clear_depth(float d);
	void clear_stencil(int32_t s);
	void clear_color(size_t tar_id, const efl::rect<size_t>& rc, const color_rgba32f& c);
	void clear_depth(const efl::rect<size_t>& rc, float d);
	void clear_stencil(const efl::rect<size_t>& rc, int32_t s);
};

//DECL_HANDLE(framebuffer, h_framebuffer);

END_NS_SOFTART()

#endif