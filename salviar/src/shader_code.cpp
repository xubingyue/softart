#include <salviar/include/shader_code.h>

#include <salviar/include/sampler_api.h>
#include <eflib/include/platform/dl_loader.h>

#include <iostream>
#include <fstream>

using eflib::dynamic_lib;

using boost::shared_ptr;

using std::cout;
using std::endl;
using std::fstream;

BEGIN_NS_SALVIAR();

shared_ptr<shader_code> shader_code::create( std::string const& code, salviar::languages lang )
{
	std::string dll_name = "sasl_host";
#ifdef EFLIB_DEBUG
	dll_name += "_d";
#endif
	dll_name += ".dll";

	shared_ptr<dynamic_lib> dl = dynamic_lib::load( dll_name );
	void (*create_shader_code)( shared_ptr<shader_code>&, std::string const&, salviar::languages );
	dl->get_function( create_shader_code, std::string("salvia_create_shader") );

	shared_ptr<shader_code> ret;
	create_shader_code( ret, code, lang );

	if( lang == lang_pixel_shader ){
		ret->register_function( &salviar_tex2Dlod_pkg,  "tex2Dlod" );
		ret->register_function( &salviar_tex2Dgrad_pkg, "tex2Dgrad" );
		ret->register_function( &salviar_tex2Dbias_pkg, "__tex2Dbias" );
		ret->register_function( &salviar_tex2Dproj_pkg, "__tex2Dproj" );
	}

	ret->update_native_function();
	
	return ret;
}

END_NS_SALVIAR();

