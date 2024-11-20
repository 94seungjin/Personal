#ifndef FRAME_H
#define FRAME_H

namespace cctv
{
	enum Frame 
	{
		WIDTH = 1280,
		HEIGHT = 720,
		PIXEL_SIZE = 3,
		FRAME_SIZE = (WIDTH * HEIGHT * PIXEL_SIZE)
	};	
}

#endif // FRAME_H