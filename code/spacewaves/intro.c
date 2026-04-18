#include <libdragon.h>

void intro(void)
{
	video_register_codec(&mpeg1_codec);
	fmv_play("rom:/spacewaves/intro.m1v", &(fmv_parms_t){
		.disable_audio = true,
	});
}

